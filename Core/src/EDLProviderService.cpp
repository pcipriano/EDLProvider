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

}

int EDLProviderService::executeApplication()
{
    return 0;
}
