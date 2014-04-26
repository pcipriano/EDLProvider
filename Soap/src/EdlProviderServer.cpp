#include "EdlProviderServer.h"

#include <QThreadPool>
#include <QCoreApplication>

#include "PathAppender.h"
#include "EdlProviderBinding.nsmap"
#include "EdlProviderProcessRequest.h"

using namespace edlprovider::soap;
using namespace plugins::interfaces;

const char* const EDL_PLUGIN_FOLDER = "plugins";

EdlProviderServer::EdlProviderServer(const QString& host)
    : pluginManager_(new EdlPluginManager(common::util::PathAppender::combine(QCoreApplication::applicationDirPath(), ::EDL_PLUGIN_FOLDER))),
      hostName_(host)
{
}

int EdlProviderServer::run(int port)
{
    if (soap_valid_socket(this->master) || soap_valid_socket(bind(hostName_.isEmpty() ? NULL : hostName_.toLatin1().data(), port, 100)))
    {
        Q_FOREVER
        {
            if (!soap_valid_socket(accept()))
                return this->error;

            EdlProviderProcessRequest* processor = new EdlProviderProcessRequest(copy());
            QThreadPool::globalInstance()->start(processor);
        }
    }
    else
        return this->error;

    return SOAP_OK;
}

EdlProviderBindingService* edlprovider::soap::EdlProviderServer::copy()
{
    return new EdlProviderServer(*this);
}

int EdlProviderServer::getInstalledEdls(edlprovider__ArrayOfstring* edlprovider__installedEdlsResponse)
{
    //Get all plugin names.
    for (EdlInterface* const edl : pluginManager_->getEdls())
    {
        edlprovider__installedEdlsResponse->string.push_back(edl->getEdlName());
    }

    return SOAP_OK;
}

int EdlProviderServer::getEdl(edlprovider__EdlCreateRequestType* edlprovider__getEdlRequest,
                              edlprovider__EdlCreateResponseType* edlprovider__getEdlResponse)
{
    return SOAP_OK;
}

int EdlProviderServer::getEdlDouble(edlprovider__EdlCreateRequestDoubleType* edlprovider__getEdlDoubleRequest,
                                    edlprovider__EdlCreateResponseType* edlprovider__getEdlResponse)
{
    return SOAP_OK;
}
