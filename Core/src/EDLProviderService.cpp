#include "EDLProviderService.h"

#include <QDebug>

#include "EDLProviderInfo.h"

#include "LoggerHelper.h"

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
    common::logging::LoggerHelper::instance().setLoggingFile(application()->applicationDirPath() + "/" + "test.log");

    qDebug() << "Test message";
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
    return QtService::executeApplication();
}
