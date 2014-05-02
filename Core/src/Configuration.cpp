#include "Configuration.h"

using namespace edlprovider::core;

static const char* const SCHEMA_RESOURCE_PATH = "qrc:/main/EdlProviderConfig.xsd";

static const char* const SERVICE_PORT = "Port";
static const char* const SERVICE_HOST_NAME = "Host";
static const char* const SERVICE_ROOT_SETTINGS = "Service";

static const char* const PLUGINS_AUTORELOAD = "AutoReload";
static const char* const PLUGINS_ROOT_SETTINGS = "Plugins";

Configuration::Configuration()
    : configReader_(QUrl(::SCHEMA_RESOURCE_PATH))
{
}

bool Configuration::load(const QString& configPath)
{
    return configReader_.loadFile(configPath);
}

quint16 Configuration::getServicePort() const
{
    return configReader_.getValueOrDefault<quint16>(QStringList() << ::SERVICE_ROOT_SETTINGS << ::SERVICE_PORT, 8080);
}

QString Configuration::getServiceHostName() const
{
    return configReader_.getValue<QString>(QStringList() << ::SERVICE_ROOT_SETTINGS << ::SERVICE_HOST_NAME);
}

bool Configuration::getPluginsAutoUpdate() const
{
    return configReader_.getValue<bool>(QStringList() << ::PLUGINS_ROOT_SETTINGS << ::PLUGINS_AUTORELOAD);
}
