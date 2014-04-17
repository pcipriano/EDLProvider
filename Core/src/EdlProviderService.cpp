#include "EdlProviderService.h"

#include <QDebug>

#include "EDLProviderInfo.h"

#include "PathAppender.h"
#include "easylogging++.h"

using namespace edlprovider::core;

EdlProviderService::EdlProviderService(int argc, char** argv)
    : QtService(argc, argv, edlprovider::info::PROJECT_USER_NAME)
{
}

EdlProviderService::~EdlProviderService()
{
}

bool EdlProviderService::setUp()
{
    QString logConfFile = common::util::PathAppender::combine(application()->applicationDirPath(),
                                                              edlprovider::info::PROJECT_LOG_FILE_CONFIG);

    //Configure logger
    el::Configurations conf(logConfFile.toStdString());
    el::Loggers::reconfigureAllLoggers(conf);

    soapServer_.reset(new soap::EdlProviderServer());

    return true;
}

void EdlProviderService::start()
{
    soapServer_->run(8087);
}

void EdlProviderService::stop()
{

}

void EdlProviderService::createApplication(int& argc, char** argv)
{
    QtService::createApplication(argc, argv);
    el::Helpers::setArgs(argc, argv);

    application()->setApplicationName(edlprovider::info::PROJECT_USER_NAME);
    application()->setApplicationVersion(edlprovider::info::EDLPROVIDER_VERSION);
    application()->setObjectName(edlprovider::info::PROJECT_NAME);
}

int EdlProviderService::executeApplication()
{
    if (!setUp())
        return -1;

    return QtService::executeApplication();
}
