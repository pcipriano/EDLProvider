#include "EDLProviderServer.h"

#include <QThreadPool>

#include "EdlProviderBinding.nsmap"
#include "EDLProviderProcessRequest.h"

using namespace edlprovider::soap;

EDLProviderServer::EDLProviderServer()
{
}

int EDLProviderServer::run(int port)
{
    if (soap_valid_socket(this->master) || soap_valid_socket(bind(NULL, port, 100)))
    {
        Q_FOREVER
        {
            if (!soap_valid_socket(accept()))
                return this->error;

            EDLProviderProcessRequest* processor = new EDLProviderProcessRequest(copy());
            QThreadPool::globalInstance()->start(processor);
        }
    }
    else
        return this->error;

    return SOAP_OK;
}

EdlProviderBindingService* edlprovider::soap::EDLProviderServer::copy()
{
    return new EDLProviderServer(*this);
}

int EDLProviderServer::getInstalledEdls(edlprovider__ArrayOfstring* edlprovider__installedEdlsResponse)
{
    edlprovider__installedEdlsResponse->string = {L"First", L"Second"};

    return SOAP_OK;
}

int EDLProviderServer::getEdl(edlprovider__EdlCreateRequestType* edlprovider__getEdlRequest,
                              edlprovider__EdlCreateResponseType* edlprovider__getEdlResponse)
{
    return SOAP_OK;
}

int EDLProviderServer::getEdlDouble(edlprovider__EdlCreateRequestDoubleType* edlprovider__getEdlDoubleRequest,
                                    edlprovider__EdlCreateResponseType* edlprovider__getEdlResponse)
{
    return SOAP_OK;
}
