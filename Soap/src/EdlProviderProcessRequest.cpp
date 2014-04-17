#include "EdlProviderProcessRequest.h"

#include "easylogging++.h"

using namespace edlprovider::soap;

EdlProviderProcessRequest::EdlProviderProcessRequest(EdlProviderBindingService* const edlProvider)
    : edlProvider_(edlProvider)
{
}

void EdlProviderProcessRequest::run()
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
