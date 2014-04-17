#include "EdlProviderServer.h"

#include <QThreadPool>

#include "EdlProviderBinding.nsmap"
#include "EdlProviderProcessRequest.h"

using namespace edlprovider::soap;

EdlProviderServer::EdlProviderServer()
{
}

int EdlProviderServer::run(int port)
{
    if (soap_valid_socket(this->master) || soap_valid_socket(bind(NULL, port, 100)))
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
    edlprovider__installedEdlsResponse->string = {L"First", L"Second"};

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
