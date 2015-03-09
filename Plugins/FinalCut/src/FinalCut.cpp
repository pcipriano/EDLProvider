#ifndef _LOGGER
#define _LOGGER "FinalCut"
#endif

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER _LOGGER
#endif

#include "EdlHelpers.h"
#include <QFileInfo>

#include "FinalCut.h"

#include <QBuffer>
#include <QXmlStreamWriter>

#include "EdlException.h"
#include "LoggingHelpers.h"

INITIALIZE_NULL_EASYLOGGINGPP

using namespace plugins::finalcut;

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
                     uint16_t nrAudioTracks,
                     uint16_t nrAudioChannels)
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

    uint16_t nrAudioTracks;

    uint16_t nrAudioChannels;
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
    processFrameRate(edlFrameRate, "Sequence frame rate not supported.", isDropFrame, timeBase);

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

        processFrameRate(videoInfo->frameRate, "Clip has an unsupported frame rate.", isDropFrame, timeBase);

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

void FinalCut::processFrameRate(const fims__RationalType* const frameRate,
                                const std::string& message,
                                bool& isDrop,
                                uint32_t& timeBase) const
{
    bool result = false;

    if (frameRate->__item == 24)
    {
        result = true;
        timeBase = 24;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 25)
    {
        result = true;
        timeBase = 25;
        isDrop = false;
    }
    else if (frameRate->__item == 30)
    {
        result = true;
        timeBase = 30;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 50)
    {
        result = true;
        timeBase = 50;
        isDrop = false;
    }
    else if (frameRate->__item == 60)
    {
        result = true;
        timeBase = 60;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }

    VLOG_IF(result, 1) << "Input frame rate:[" << *frameRate << "]" << "out ntsc:["
            << std::to_string(isDrop) << "]"
            << "out time base:[" << std::to_string(timeBase) << "].";

    if (!result)
        throw interfaces::EdlException(interfaces::EdlException::EdlError::UNSUPPORTED_FRAME_RATE, message);
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

void FinalCut::writeAudioDescription(uint16_t nrAudioTracks, uint16_t nrAudioChannels, QXmlStreamWriter& writer) const
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
                                     uint16_t nrAudioTracks,
                                     uint16_t nrAudioChannels,
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
