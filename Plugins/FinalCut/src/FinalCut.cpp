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
    QXmlStreamWriter xmlWriter;
    QBuffer xmlBuffer;
    xmlBuffer.open(QIODevice::WriteOnly);
    xmlWriter.setDevice(&xmlBuffer);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument("1.0");

    for (const edlprovider__ClipType* const clip : clips)
    {
        auto clipss = clip->markIn->union_TimeType.timecode;
        VLOG(1) << "MarkIn:[" << *clip->markIn << "]" << " "
                   "MarkOut:[" << *clip->markOut << "]";
    }

    return xmlBuffer.buffer();
}

void FinalCut::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}
