#include "EDLProviderProcessRequest.h"

#include "easylogging++.h"

using namespace edlprovider::soap;

EDLProviderProcessRequest::EDLProviderProcessRequest(EdlProviderBindingService* const edlProvider)
    : edlProvider_(edlProvider)
{
}

void EDLProviderProcessRequest::run()
{
    if (edlProvider_->serve() != SOAP_OK)
    {
        std::ostringstream stream;
        edlProvider_->soap_stream_fault(stream);
        LOG(DEBUG) << "Could not process call" << std::endl << stream.str();
        return;
    }

    LOG(DEBUG) << "Processed soap call successfully.";
}
