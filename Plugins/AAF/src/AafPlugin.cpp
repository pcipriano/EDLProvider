#ifndef _LOGGER
#define _LOGGER "AAF"
#endif

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER _LOGGER
#endif

#include "AafPlugin.h"

#include <AAF.h>
#include <AAFTypes.h>
#include <AAFResult.h>
#include <AAFExtEnum.h>
#include <AAFDataDefs.h>
#include <AAFFileKinds.h>
#include <AAFTypeDefUIDs.h>
#include <AAFClassDefUIDs.h>
#include <AAFContainerDefs.h>
#include <AAFCompressionDefs.h>
#include <AAFStoredObjectIDs.h>

INITIALIZE_NULL_EASYLOGGINGPP

using namespace plugins::aaf;

AafPlugin::AafPlugin()
{

}

QByteArray AafPlugin::createEdl(const std::wstring* const edlSequenceName,
                                const fims__RationalType* const edlFrameRate,
                                const std::vector<edlprovider__ClipType*>& clips) const
{
    return QByteArray();
}

void AafPlugin::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}
