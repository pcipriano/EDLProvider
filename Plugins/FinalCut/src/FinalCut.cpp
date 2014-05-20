#include "FinalCut.h"

#include <QXmlStreamWriter>

#ifndef _LOGGER
#define _LOGGER "FinalCut"
#endif

#include "easylogging++.h"

using namespace plugins::finalcut;

FinalCut::FinalCut()
{
    el::Loggers::getLogger("business");
}

QByteArray FinalCut::createEdl(const std::wstring* const edlSequenceName,
                               const fims__RationalType* const edlFrameRate,
                               const std::vector<edlprovider__ClipType*>& clips) const
{
    QXmlStreamWriter xmlWriter;
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument("1.0");

    LOG(ERROR) << "Test logging";

//    for (const edlprovider__ClipType* const clip : clips)
//    {
//        LOG(ERROR) << "Test logging" << clip->clipInfo->resourceID;
//    }

    return QByteArray("sdfs  WOW");
}
