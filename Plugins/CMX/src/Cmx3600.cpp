#ifndef _LOGGER
#define _LOGGER "CMX"
#endif

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER _LOGGER
#endif

#include "EdlHelpers.h"

#include "Cmx3600.h"

#include <QBuffer>
#include <QString>
#include <QFileInfo>
#include <QTextStream>

#include "TimeCode.h"
#include "EdlException.h"
#include "LoggingHelpers.h"

INITIALIZE_NULL_EASYLOGGINGPP

const QString DROP_FRAME_ID = "DROP FRAME";
const QString NON_DROP_FRAME_ID = "NON DROP FRAME";

using namespace plugins::cmx;

QByteArray Cmx3600::createEdl(const std::wstring* const edlSequenceName,
                              const fims__RationalType* const edlFrameRate,
                              const std::vector<edlprovider__ClipType*>& clips) const
{
    QString sequenceName;
    if (edlSequenceName != NULL)
        sequenceName = QString::fromStdWString(*edlSequenceName);

    VLOG(1) << "SequenceName:[" << sequenceName << "] "
               "Rate:[" << *edlFrameRate << "]";

    QByteArray outputEdlBuffer;
    QTextStream outputStream(&outputEdlBuffer);

    //Start writing the CMX3600 edl
    sequenceName.truncate(70);   //70 characters maximum
    outputStream << "TITLE: " << sequenceName << "\r\n";      //70 characters maximum

    outputStream << "FCM: " << (isDropFrame(edlFrameRate) ? ::DROP_FRAME_ID : ::NON_DROP_FRAME_ID) << "\r\n";

    uint64_t totalFrameCount = 0;
    uint32_t formStatementCount = 1;
    for (const auto* const clip : clips)
    {
        outputStream << "\r\n";
        const fims__BMContentType* clipInfo = nullptr;
        const fims__BMContentFormatType* clipFormatInfo = nullptr;
        const fims__VideoFormatType* videoInfo = nullptr;
        const fims__AudioFormatType* audioInfo = nullptr;
        uint16_t nrAudioTracks = 0;
        uint16_t nrAudioChannels = 0;

        interfaces::processClipInfo(clip, &clipInfo, &clipFormatInfo, &videoInfo, &audioInfo, nrAudioTracks, nrAudioChannels);
        interfaces::logClipInformation(clipFormatInfo, clip, videoInfo, audioInfo, nrAudioTracks, nrAudioChannels);

        uint64_t fileDurationFrames = 0;
        uint64_t clipMarkInFrames   = 0;
        uint64_t clipMarkOutFrames  = 0;
        uint64_t clipDurationFrames = 0;

        interfaces::getClipDurationInFrames(clip,
                                            clipFormatInfo,
                                            videoInfo,
                                            fileDurationFrames,
                                            clipMarkInFrames,
                                            clipMarkOutFrames,
                                            clipDurationFrames);

        uint32_t frameRateNum = std::stoi(videoInfo->frameRate->numerator) * videoInfo->frameRate->__item;
        uint32_t frameRateDen = std::stoi(videoInfo->frameRate->denominator);

        Timecode markInTimeCode(frameRateNum, frameRateDen, isDropFrame(videoInfo->frameRate), clipMarkInFrames);
        Timecode markOutTimeCode(frameRateNum, frameRateDen, isDropFrame(videoInfo->frameRate), clipMarkOutFrames);
        Timecode recordInTimeCode(frameRateNum, frameRateDen, isDropFrame(videoInfo->frameRate), totalFrameCount);

        totalFrameCount += clipDurationFrames;
        Timecode recordOutTimeCode(frameRateNum, frameRateDen, isDropFrame(videoInfo->frameRate), totalFrameCount);

        uint8_t audioTotalChannels = nrAudioTracks * nrAudioChannels;

        QString formStatementVideo;
        formStatementVideo.sprintf("%03d   AX  %4s     C     ",
                                   formStatementCount,
                                   audioTotalChannels == 1 ? "B" : (audioTotalChannels >= 2 ? "AA/V" : "V"));
        outputStream << formStatementVideo;

        outputStream << QString::fromStdWString(markInTimeCode.toTimecodeString(false));
        outputStream << " " << QString::fromStdWString(markOutTimeCode.toTimecodeString(false));
        outputStream << " " << QString::fromStdWString(recordInTimeCode.toTimecodeString(false));
        outputStream << " " << QString::fromStdWString(recordOutTimeCode.toTimecodeString(false));

        outputStream << "\r\n";

        QFileInfo fInfo(QString::fromStdWString(*clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location));
        outputStream << "* FROM CLIP NAME: " << fInfo.completeBaseName() << "\r\n";

        if (audioTotalChannels > 2)
        {
            outputStream << QString("AUD  3   %1").arg(audioTotalChannels == 4 ? "4" : " ") << "\r\n";
        }

        formStatementCount++;
    }

    outputStream.flush();
    return outputEdlBuffer;
}

bool Cmx3600::isDropFrame(const fims__RationalType* const frameRate) const
{
    bool result = false;
    bool isDrop = false;

    if (frameRate->__item == 24)
    {
        result = true;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 25)
    {
        result = true;
        isDrop = false;
    }
    else if (frameRate->__item == 30)
    {
        result = true;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 50)
    {
        result = true;
        isDrop = false;
    }
    else if (frameRate->__item == 60)
    {
        result = true;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }

    if (!result)
        throw interfaces::EdlException(interfaces::EdlException::EdlError::UNSUPPORTED_FRAME_RATE,
                                       "Sequence frame rate not supported.");

    return isDrop;
}

void Cmx3600::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}
