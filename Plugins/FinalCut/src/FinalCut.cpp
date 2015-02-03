#ifndef _LOGGER
#define _LOGGER "FinalCut"
#endif

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER _LOGGER
#endif

#include <QTime>
#include <QFileInfo>

#include "FinalCut.h"

#include <cstdint>
#include <QBuffer>
#include <QXmlStreamWriter>

#include "TimeCode.h"
#include "LoggingHelpers.h"

INITIALIZE_NULL_EASYLOGGINGPP

using namespace plugins::finalcut;

static uint64_t getNrFrames(const fimstime__DurationType& duration, uint32_t frameRateNum, uint32_t frameRateDen)
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

static uint64_t getNrFrames(const fimstime__TimeType& time, uint32_t frameRateNum, uint32_t frameRateDen)
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

inline bool operator==(const fims__LengthType& lhs, const fims__LengthType rhs)
{
    return (lhs.unit == rhs.unit) && (lhs.__item == rhs.__item);
}

struct AudioInformation
{
    AudioInformation(bool isDrop,
                     uint32_t timeBase,
                     uint64_t markInFrames,
                     uint64_t markOutFrames,
                     uint64_t durationFrames,
                     const std::wstring& path,
                     ushort nrAudioTracks,
                     ushort nrAudioChannels)
        : isDrop(isDrop),
          timeBase(timeBase),
          markInFrames(markInFrames),
          markOutFrames(markOutFrames),
          durationFrames(durationFrames),
          path(path),
          nrAudioTracks(nrAudioTracks),
          nrAudioChannels(nrAudioChannels)
    {
    }

    bool isDrop;

    uint32_t timeBase;

    uint64_t markInFrames;

    uint64_t markOutFrames;

    uint64_t durationFrames;

    std::wstring path;

    ushort nrAudioTracks;

    ushort nrAudioChannels;
};

FinalCut::FinalCut()
{
}

QByteArray FinalCut::createEdl(const std::wstring* const edlSequenceName,
                               const fims__RationalType* const edlFrameRate,
                               const std::vector<edlprovider__ClipType*>& clips) const
{
    QString sequenceName;
    if (edlSequenceName != NULL)
        sequenceName = QString::fromStdWString(*edlSequenceName);

    QXmlStreamWriter xmlWriter;
    QBuffer xmlBuffer;
    xmlBuffer.open(QIODevice::WriteOnly);
    xmlWriter.setDevice(&xmlBuffer);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument("1.0");
    xmlWriter.writeDTD("<!DOCTYPE xmeml>");

    VLOG(1) << "SequenceName:[" << sequenceName << "] "
               "Rate:[" << *edlFrameRate << "]";

    //Start writing the final cut xml
    xmlWriter.writeStartElement("xmeml");
    xmlWriter.writeAttribute("version", "4");
    xmlWriter.writeStartElement("sequence");
    xmlWriter.writeAttribute("id", sequenceName);
    xmlWriter.writeTextElement("name", sequenceName);

    bool isDropFrame = false;
    uint32_t timeBase = 0;
    processFrameRate(edlFrameRate, isDropFrame, timeBase);

    //Rate
    writeRateSection(isDropFrame, timeBase, xmlWriter);

    //Timecode
    writeTimecodeSection(isDropFrame, timeBase, xmlWriter);

    xmlWriter.writeTextElement("in", "-1");
    xmlWriter.writeTextElement("out", "-1");

    xmlWriter.writeStartElement("media");

    //Video
    xmlWriter.writeStartElement("video");

    size_t nItem = 1;
    uint64_t totalTrackFramesDuration = 0;
    std::vector<AudioInformation> audioInfos;
    for (auto clipIt = clips.cbegin(); clipIt != clips.cend(); clipIt++)
    {
        const edlprovider__ClipType* const clip = *clipIt;

        if (clip->clipInfo->bmContents == NULL)
            throw std::invalid_argument("bmContents must be set.");

        fims__BMContentType* clipInfo = clip->clipInfo->bmContents->bmContent.front();
        fims__BMContentFormatType* clipFormatInfo = clipInfo->bmContentFormats->bmContentFormat.front();
        fims__VideoFormatType* videoInfo = clipFormatInfo->formatCollection->videoFormat;
        fims__AudioFormatType* audioInfo = clipFormatInfo->formatCollection->audioFormat;
        ushort nrAudioTracks = audioInfo->audioTrack.size();
        ushort nrAudioChannels = std::stoi(*audioInfo->channels);

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

        uint32_t frameRateNum = std::stoi(videoInfo->frameRate->numerator) * videoInfo->frameRate->__item;
        uint32_t frameRateDen = std::stoi(videoInfo->frameRate->denominator);

        uint64_t fileDurationFrames = getNrFrames(*clipFormatInfo->duration, frameRateNum, frameRateDen);
        uint64_t clipMarkInFrames = getNrFrames(*clip->markIn, frameRateNum, frameRateDen);
        uint64_t clipMarkOutFrames = getNrFrames(*clip->markOut, frameRateNum, frameRateDen);
        uint64_t clipDurationFrames = clipMarkOutFrames - clipMarkInFrames;

        VLOG(2) << "File Frames Duration:[" << fileDurationFrames << "] "
                   "MarkIn Frames Position:[" << clipMarkInFrames << "] "
                   "MarkOut Frames Position:[" << clipMarkOutFrames << "] "
                   "Clip Frames Duration:[" << clipDurationFrames << "]";

        processFrameRate(videoInfo->frameRate, isDropFrame, timeBase);

        //Add new audio information to use later
        audioInfos.emplace_back(isDropFrame,
                                timeBase,
                                clipMarkInFrames,
                                clipMarkOutFrames,
                                clipDurationFrames,
                                *clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location,
                                nrAudioTracks,
                                nrAudioChannels);

        //Start writing video section
        if (clipIt != clips.cbegin())
        {
            const edlprovider__ClipType* const previousClip = *(clipIt - 1);
            fims__BMContentType* previousClipInfo = previousClip->clipInfo->bmContents->bmContent.front();
            fims__BMContentFormatType* previousClipFormatInfo = previousClipInfo->bmContentFormats->bmContentFormat.front();
            const fims__VideoFormatType* const previousClipVideoInfo = previousClipFormatInfo->formatCollection->videoFormat;

            //Check if clip is different.
            if ((videoInfo->displayWidth != previousClipVideoInfo->displayWidth) ||
                (videoInfo->displayHeight != previousClipVideoInfo->displayHeight) ||
                (videoInfo->frameRate->numerator != previousClipVideoInfo->frameRate->numerator) ||
                (videoInfo->frameRate->denominator != previousClipVideoInfo->frameRate->denominator))
            {
                xmlWriter.writeTextElement("enabled", "true");
                xmlWriter.writeTextElement("locked", "false");

                //</track>
                xmlWriter.writeEndElement();        //Close previous track section

                writeFormatSection(isDropFrame, timeBase, videoInfo, xmlWriter);

                xmlWriter.writeStartElement("track");       //Start again a track section
            }
        }
        else
        {
            writeFormatSection(isDropFrame, timeBase, videoInfo, xmlWriter);
            xmlWriter.writeStartElement("track");
        }

        QFileInfo fInfo(QString::fromStdWString(*clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location));

        //Add clip
        xmlWriter.writeStartElement("clipitem");
        xmlWriter.writeAttribute("id", QString::fromStdWString(clip->clipInfo->resourceID));

        xmlWriter.writeTextElement("name", fInfo.fileName());
        xmlWriter.writeTextElement("duration", QString::number(clipDurationFrames));
        writeRateSection(isDropFrame, timeBase, xmlWriter);
        xmlWriter.writeTextElement("in", QString::number(clipMarkInFrames));
        xmlWriter.writeTextElement("out", QString::number(clipMarkOutFrames));
        xmlWriter.writeTextElement("start", QString::number(totalTrackFramesDuration));
        xmlWriter.writeTextElement("end", QString::number(totalTrackFramesDuration += clipDurationFrames));
        xmlWriter.writeTextElement("enabled", "true");
        xmlWriter.writeTextElement("masterclipid", fInfo.fileName() + "1");
        xmlWriter.writeStartElement("file");

        xmlWriter.writeAttribute("id", fInfo.completeBaseName());
        xmlWriter.writeTextElement("name", fInfo.completeBaseName());
        xmlWriter.writeTextElement("pathurl", QString::fromStdWString(*clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location));
        writeRateSection(isDropFrame, timeBase, xmlWriter);
        xmlWriter.writeTextElement("duration", QString::number(fileDurationFrames));

        xmlWriter.writeStartElement("media");
        xmlWriter.writeStartElement("video");
        xmlWriter.writeTextElement("duration", QString::number(fileDurationFrames));
        xmlWriter.writeStartElement("samplecharacteristics");

        xmlWriter.writeTextElement("width", QString::fromStdWString(videoInfo->displayWidth->__item));
        xmlWriter.writeTextElement("height", QString::fromStdWString(videoInfo->displayHeight->__item));

        //</samplecharacteristics>
        xmlWriter.writeEndElement();

        //</video>
        xmlWriter.writeEndElement();

        writeAudioDescription(nrAudioTracks, nrAudioChannels, xmlWriter);

        //</media>
        xmlWriter.writeEndElement();

        //</file>
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("sourcetrack");

        xmlWriter.writeTextElement("mediatype", "video");
        xmlWriter.writeTextElement("trackindex", "1");

        //</sourcetrack>
        xmlWriter.writeEndElement();

        writeLinkDescriptions(QString::fromStdWString(clip->clipInfo->resourceID),
                              nItem,
                              nrAudioTracks,
                              nrAudioChannels,
                              fInfo,
                              xmlWriter);

        if ((clipInfo->descriptions != NULL) && (!clipInfo->descriptions->description.empty()))
        {
            fims__DescriptionType* descriptions = clipInfo->descriptions->description.front();
            if (!descriptions->fimsdescription__bmContentDescription->description.empty())
            {
                fimsdescription__descriptionType* description = descriptions->fimsdescription__bmContentDescription->description.front();
                xmlWriter.writeStartElement("comments");
                xmlWriter.writeTextElement("clipcommenta", QString::fromStdWString(description->__item));
                //</comments>
                xmlWriter.writeEndElement();
            }
        }

        //</clipitem>
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeTextElement("enabled", "true");
    xmlWriter.writeTextElement("locked", "false");

    //</track>
    xmlWriter.writeEndElement();

    //</video>
    xmlWriter.writeEndElement();

    //Now write audio segment
    xmlWriter.writeStartElement("audio");

    xmlWriter.writeTextElement("in", "-1");
    xmlWriter.writeTextElement("out", "-1");

    totalTrackFramesDuration = 0;
    nItem = 2;   //Progressing id
    for (auto audInfoIt = audioInfos.cbegin(); audInfoIt != audioInfos.cend(); audInfoIt++, nItem++)
    {
        QFileInfo fInfo(QString::fromStdWString(audInfoIt->path));

        xmlWriter.writeStartElement("track");

        for (int i = 0; i < (audInfoIt->nrAudioChannels * audInfoIt->nrAudioTracks); i++)
        {
            xmlWriter.writeStartElement("clipitem");

            xmlWriter.writeAttribute("id", fInfo.fileName() + QString::number(nItem));
            xmlWriter.writeTextElement("name", fInfo.fileName());
            xmlWriter.writeTextElement("duration", QString::number(audInfoIt->durationFrames));
            writeRateSection(audInfoIt->isDrop, audInfoIt->timeBase, xmlWriter);
            xmlWriter.writeTextElement("in", QString::number(audInfoIt->markInFrames));
            xmlWriter.writeTextElement("out", QString::number(audInfoIt->markOutFrames));
            xmlWriter.writeTextElement("start", QString::number(totalTrackFramesDuration));
            xmlWriter.writeTextElement("end", QString::number(totalTrackFramesDuration + audInfoIt->durationFrames));
            xmlWriter.writeTextElement("enabled", "true");
            xmlWriter.writeTextElement("masterclipid", fInfo.fileName() + "1");
            xmlWriter.writeStartElement("file");
            xmlWriter.writeAttribute("id", fInfo.completeBaseName());
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement("sourcetrack");
            xmlWriter.writeTextElement("mediatype", "audio");
            xmlWriter.writeTextElement("trackindex", QString::number(i + 1));
            xmlWriter.writeEndElement();

            //</clipitem>
            xmlWriter.writeEndElement();

            xmlWriter.writeTextElement("outputchannelindex", QString::number(i + 1));
        }

        xmlWriter.writeTextElement("enabled", "true");
        xmlWriter.writeTextElement("locked", "false");

        //</track>
        xmlWriter.writeEndElement();

        totalTrackFramesDuration += audInfoIt->durationFrames;
    }

    //</audio>
    xmlWriter.writeEndElement();

    //</media>
    xmlWriter.writeEndElement();

    xmlWriter.writeTextElement("duration", QString::number(totalTrackFramesDuration));

    xmlWriter.writeTextElement("ismasterclip", "false");

    xmlWriter.writeEndDocument();

    return xmlBuffer.buffer();
}

void FinalCut::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}

void FinalCut::processFrameRate(const fims__RationalType* const frameRate, bool& isDrop, uint32_t& timeBase) const
{
    if (frameRate->__item == 24)
    {
        timeBase = 24;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 25)
    {
        timeBase = 25;
        isDrop = false;
    }
    else if (frameRate->__item == 30)
    {
        timeBase = 30;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 50)
    {
        timeBase = 50;
        isDrop = false;
    }
    else if (frameRate->__item == 60)
    {
        timeBase = 60;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }

    VLOG(1) << "Input frame rate:[" << *frameRate << "]" << "out ntsc:["
            << std::to_string(isDrop) << "]"
            << "out time base:[" << std::to_string(timeBase) << "].";
}

void FinalCut::writeRateSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const
{
    writer.writeStartElement("rate");
    writer.writeTextElement("ntsc", QString::number(dropFrame));
    writer.writeTextElement("timebase", QString::number(timeBase));
    writer.writeEndElement();
}

void FinalCut::writeTimecodeSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const
{
    writer.writeStartElement("timecode");
    writeRateSection(dropFrame, timeBase, writer);
    writer.writeTextElement("frame", "0");
    writer.writeTextElement("displayformat", dropFrame ? "DF" : "NDF");
    writer.writeEndElement();
}

void FinalCut::writeFormatSection(bool dropFrame,
                                  uint32_t timeBase,
                                  const fims__VideoFormatType* const videoInfo,
                                  QXmlStreamWriter& writer) const
{
    writer.writeStartElement("format");
    writer.writeStartElement("samplecharacteristics");
    writeRateSection(dropFrame, timeBase, writer);     //Rate
    writer.writeTextElement("width", QString::fromStdWString(videoInfo->displayWidth->__item));
    writer.writeTextElement("height", QString::fromStdWString(videoInfo->displayHeight->__item));
    writer.writeEndElement();
    writer.writeEndElement();
}

void FinalCut::writeAudioDescription(ushort nrAudioTracks, ushort nrAudioChannels, QXmlStreamWriter& writer) const
{
    for (int i = 0; i < nrAudioTracks; i++)
    {
        writer.writeStartElement("audio");
        writer.writeTextElement("channelcount", QString::number(nrAudioChannels));
        writer.writeTextElement("layout", nrAudioChannels == 2 ? "stereo" : "mono");
        writer.writeEndElement();
    }
}

void FinalCut::writeLinkDescriptions(const QString& clipUniqueId,
                                     size_t clipPos,
                                     ushort nrAudioTracks,
                                     ushort nrAudioChannels,
                                     const QFileInfo& clipPathInfo,
                                     QXmlStreamWriter& writer) const
{
    writer.writeStartElement("link");
    writer.writeTextElement("linkclipref", clipUniqueId);
    writer.writeTextElement("mediatype", "video");
    writer.writeTextElement("trackindex", "1");
    writer.writeTextElement("clipindex", QString::number(clipPos));
    writer.writeEndElement();

    int group = 1;
    int audId = 2;
    int audTrackIndex = 1;
    for (int i = 0; i < nrAudioTracks; i++)
    {
        if (nrAudioChannels > 1)
        {
            for (int u = 0; u < nrAudioChannels; u++)
            {
                writer.writeStartElement("link");
                writer.writeTextElement("linkclipref", clipPathInfo.fileName() + QString::number(audId++));
                writer.writeTextElement("mediatype", "audio");
                writer.writeTextElement("trackindex", QString::number(audTrackIndex++));
                writer.writeTextElement("clipindex", QString::number(clipPos));
                writer.writeTextElement("groupindex", QString::number(group));
                writer.writeEndElement();
            }
            group++;
        }
        else
        {
            writer.writeStartElement("link");
            writer.writeTextElement("linkclipref", clipPathInfo.fileName() + QString::number(audId++));
            writer.writeTextElement("mediatype", "audio");
            writer.writeTextElement("trackindex", QString::number(audTrackIndex++));
            writer.writeTextElement("clipindex", QString::number(clipPos));
            writer.writeEndElement();
        }
    }
}
