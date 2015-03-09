#include <QTime>

#include "EdlHelpers.h"

#include "TimeCode.h"
#include "EdlException.h"
#include "LoggingHelpers.h"

uint64_t plugins::interfaces::getNrFrames(const fimstime__TimeType& time,
                                          uint32_t frameRateNum,
                                          uint32_t frameRateDen)
{
    if (time.__union_TimeType == SOAP_UNION__fimstime__union_TimeType_editUnitNumber)
    {
        return time.union_TimeType.editUnitNumber->__item;
    }
    else if (time.__union_TimeType == SOAP_UNION__fimstime__union_TimeType_timecode)
    {
        Timecode tc = Timecode::fromTimecodeString(*time.union_TimeType.timecode, frameRateNum, frameRateDen);
        return tc.getTotalFrames();
    }
    else
    {
        QTime timeCount = QTime::fromString(QString::fromStdWString(*time.union_TimeType.normalPlayTime));
        Timecode tc = Timecode::fromMilliseconds(timeCount.msec(), frameRateNum, frameRateDen, true);
        return tc.getTotalFrames();
    }
}

uint64_t plugins::interfaces::getNrFrames(const fimstime__DurationType& duration,
                                          uint32_t frameRateNum,
                                          uint32_t frameRateDen)
{
    if (duration.__union_DurationType == SOAP_UNION__fimstime__union_DurationType_editUnitNumber)
    {
        return duration.union_DurationType.editUnitNumber->__item;
    }
    else if (duration.__union_DurationType == SOAP_UNION__fimstime__union_DurationType_timecode)
    {
        Timecode tc = Timecode::fromTimecodeString(*duration.union_DurationType.timecode, frameRateNum, frameRateDen);
        return tc.getTotalFrames();
    }
    else
    {
        Timecode tc = Timecode::fromMilliseconds(*duration.union_DurationType.normalPlayTime, frameRateNum, frameRateDen, true);
        return tc.getTotalFrames();
    }
}


void plugins::interfaces::processClipInfo(const edlprovider__ClipType* const clip,
                                          const fims__BMContentType* clipInfo,
                                          const fims__BMContentFormatType* clipFormatInfo,
                                          const fims__VideoFormatType* videoInfo,
                                          const fims__AudioFormatType* audioInfo,
                                          uint16_t& nrAudioTracks,
                                          uint16_t& nrAudioChannels)
{
    if (clip->clipInfo->bmContents == NULL)
        throw std::invalid_argument("bmContents must be set.");

    clipInfo = clip->clipInfo->bmContents->bmContent.front();
    clipFormatInfo = clipInfo->bmContentFormats->bmContentFormat.front();
    videoInfo = clipFormatInfo->formatCollection->videoFormat;
    audioInfo = clipFormatInfo->formatCollection->audioFormat;
    nrAudioTracks = audioInfo->audioTrack.size();
    nrAudioChannels = std::stoi(*audioInfo->channels);
}


void plugins::interfaces::logClipInformation(const fims__BMContentFormatType* const clipFormatInfo,
                                             const edlprovider__ClipType* const clip,
                                             const fims__VideoFormatType* const videoInfo,
                                             const fims__AudioFormatType* const audioInfo,
                                             uint16_t nrAudioTracks,
                                             uint16_t nrAudioChannels)
{
    VLOG(1) << "MarkIn:[" << *clip->markIn << "] "
               "MarkOut:[" << *clip->markOut << "] "
               "Id:[" << clip->clipInfo->resourceID << "] "
               "Duration:[" << *clipFormatInfo->duration << "] "
               "Path:[" << *clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location << "] "
               "Clip Video Rate:[" << *videoInfo->frameRate << "] "
               "Display Width:[" << *videoInfo->displayWidth << "] "
               "Display Height:[" << *videoInfo->displayHeight << "] "
               "Aspect Ratio:[" << *videoInfo->aspectRatio << "] "
               "Scanning order:[" << (*videoInfo->scanningOrder == fims__ScanningOrderType__top ? "top" : "bottom") << "] "
               "Scanning format:[" << (*videoInfo->scanningFormat == fims__ScanningFormatType__interlaced ? "interlaced" : "progressive") << "]";

    //Log audio information if audio tracks present
    VLOG_IF(nrAudioTracks > 0, 1) << "Audio Sampling Rate:[" << *audioInfo->samplingRate << "] "
                                     "Audio Sample Size:[" << *audioInfo->sampleSize << "] "
                                     "Audio Channels:[" << nrAudioChannels << "] "
                                     "Audio Tracks:[" << nrAudioTracks << "]";
}


void plugins::interfaces::getClipDurationInFrames(const edlprovider__ClipType* const clip,
                                                  const fims__BMContentFormatType* const clipFormatInfo,
                                                  const fims__VideoFormatType* const videoInfo,
                                                  uint64_t& fileDurationFrames,
                                                  uint64_t& clipMarkInFrames,
                                                  uint64_t& clipMarkOutFrames,
                                                  uint64_t& clipDurationFrames,
                                                  bool logValues)
{
    uint32_t frameRateNum = std::stoi(videoInfo->frameRate->numerator) * videoInfo->frameRate->__item;
    uint32_t frameRateDen = std::stoi(videoInfo->frameRate->denominator);

    fileDurationFrames = interfaces::getNrFrames(*clipFormatInfo->duration, frameRateNum, frameRateDen);
    clipMarkInFrames = interfaces::getNrFrames(*clip->markIn, frameRateNum, frameRateDen);
    clipMarkOutFrames = interfaces::getNrFrames(*clip->markOut, frameRateNum, frameRateDen);

    if (clipMarkInFrames > clipMarkOutFrames)
        throw interfaces::EdlException(interfaces::EdlException::EdlError::MARKIN_BIGGER_THAN_MARKOUT);

    if ((clipMarkInFrames > fileDurationFrames) || (clipMarkOutFrames > fileDurationFrames))
        throw interfaces::EdlException(interfaces::EdlException::EdlError::MARK_INOUT_OUTSIDE_DURATION);

    clipDurationFrames = clipMarkOutFrames - clipMarkInFrames;

    VLOG_IF(logValues, 2) << "File Frames Duration:[" << fileDurationFrames << "] "
                             "MarkIn Frames Position:[" << clipMarkInFrames << "] "
                             "MarkOut Frames Position:[" << clipMarkOutFrames << "] "
                             "Clip Frames Duration:[" << clipDurationFrames << "]";
}
