#include "EDLProviderService.h"

#include "EDLProviderInfo.h"

using namespace edlprovider::core;

EDLProviderService::EDLProviderService(int argc, char** argv)
    : QtService(argc, argv, edlprovider::info::PROJECT_USER_NAME)
{
}

EDLProviderService::~EDLProviderService()
{
}

void EDLProviderService::start()
{

}

void EDLProviderService::stop()
{

}

void EDLProviderService::createApplication(int& argc, char** argv)
{
    QtService::createApplication(argc, argv);

    application()->setApplicationName(edlprovider::info::PROJECT_USER_NAME);
    application()->setApplicationVersion(edlprovider::info::EDLPROVIDER_VERSION);
    application()->setObjectName(edlprovider::info::PROJECT_NAME);
}

int EDLProviderService::executeApplication()
{
    return 0;
}
