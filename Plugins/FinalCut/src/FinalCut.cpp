#ifndef _LOGGER
#define _LOGGER "FinalCut"
#endif

#include "FinalCut.h"

#include <QBuffer>
#include <QXmlStreamWriter>

#include "LoggingHelpers.h"

_INITIALIZE_NULL_EASYLOGGINGPP

using namespace plugins::finalcut;

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

    VLOG(1) << "SequenceName:[" << sequenceName << "] "
               "Rate:[" << *edlFrameRate << "]";

    for (const edlprovider__ClipType* const clip : clips)
    {
        if (clip->clipInfo->bmContents == NULL)
            throw std::invalid_argument("bmContents must be set.");

        fims__BMContentType* clipInfo = clip->clipInfo->bmContents->bmContent.front();
        fims__BMContentFormatType* clipFormatInfo = clipInfo->bmContentFormats->bmContentFormat.front();

        VLOG(1) << "MarkIn:[" << *clip->markIn << "] "
                   "MarkOut:[" << *clip->markOut << "] "
                   "Id:[" << clip->clipInfo->resourceID << "] "
                   "Duration:[" << *clipFormatInfo->duration << "]"
                   "Path:[" << *clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location << "]";
    }

    return xmlBuffer.buffer();
}

void FinalCut::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}
