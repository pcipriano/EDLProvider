#include "EdlProviderService.h"

#include "EdlProviderServer.h"
#include "EdlProviderServerThread.h"

#include <QDebug>

#include "EDLProviderInfo.h"

#include "PathAppender.h"
#include "easylogging++.h"

using namespace edlprovider::core;

EdlProviderService::EdlProviderService(int argc, char** argv)
    : QtService(argc, argv, edlprovider::info::PROJECT_USER_NAME)
{
    setServiceFlags(QtServiceBase::Default);
}

EdlProviderService::~EdlProviderService()
{
}

bool EdlProviderService::setUp()
{
    QString logConfFile = common::util::PathAppender(application()->applicationDirPath(),
                                                     edlprovider::info::PROJECT_LOG_FILE_CONFIG);

    //Configure logger
    el::Configurations conf(logConfFile.toStdString());
    el::Loggers::setDefaultConfigurations(conf, true);

    return true;
}

void EdlProviderService::start()
{
    QString appConfigFile = common::util::PathAppender(application()->applicationDirPath(),
                                                       edlprovider::info::PROJECT_CONFIG_FILE);

    //Start configuration
    configuration_.reset(new Configuration());
    if (!configuration_->load(appConfigFile))
        LOG(FATAL) << "Configuration from [" << appConfigFile << "].";

    soapServer_.reset(new soap::EdlProviderServer(configuration_->getServiceHostName(),
                                                  configuration_->getPluginsAutoUpdate()));

    soapServerThread_.reset(new soap::EdlProviderServerThread());

    QObject::connect(soapServerThread_.data(),
                     &soap::EdlProviderServerThread::finished,
                     soapServerThread_.data(),
                     &soap::EdlProviderServerThread::deleteLater);


    QObject::connect(soapServerThread_.data(),
                     &soap::EdlProviderServerThread::started,
                     [this]()
    {
        if(soapServer_->run(configuration_->getServicePort()) != SOAP_OK)
            LOG(ERROR) << "Service could not be started.";
    });

    //Now start the soap server thread
    soapServerThread_->start();
}

void EdlProviderService::stop()
{
    LOG(INFO) << "Stopping [" << edlprovider::info::PROJECT_USER_NAME << "].";

    if (soapServerThread_)
        soapServerThread_->quit();
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
