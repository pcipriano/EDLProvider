#include "EDLProviderService.h"

#include <QDebug>

#include "EDLProviderInfo.h"

#include "PathAppender.h"
#include "easylogging++.h"

using namespace edlprovider::core;

EDLProviderService::EDLProviderService(int argc, char** argv)
    : QtService(argc, argv, edlprovider::info::PROJECT_USER_NAME)
{
}

EDLProviderService::~EDLProviderService()
{
}

bool EDLProviderService::setUp()
{
    QString logConfFile = common::util::PathAppender::combine(application()->applicationDirPath(),
                                                              edlprovider::info::PROJECT_LOG_FILE_CONFIG);

    //Configure logger
    el::Configurations conf(logConfFile.toStdString());
    el::Loggers::reconfigureAllLoggers(conf);

    soapServer_.reset(new soap::EDLProviderServer());

    return true;
}

void EDLProviderService::start()
{
    soapServer_->run(8087);
}

void EDLProviderService::stop()
{

}

void EDLProviderService::createApplication(int& argc, char** argv)
{
    QtService::createApplication(argc, argv);
    el::Helpers::setArgs(argc, argv);

    application()->setApplicationName(edlprovider::info::PROJECT_USER_NAME);
    application()->setApplicationVersion(edlprovider::info::EDLPROVIDER_VERSION);
    application()->setObjectName(edlprovider::info::PROJECT_NAME);
}

int EDLProviderService::executeApplication()
{
    if (!setUp())
        return -1;

    return QtService::executeApplication();
}
