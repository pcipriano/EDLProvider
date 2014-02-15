#include "ConfigurationReader.h"

#include <QFile>
#include <QTextStream>

#include "easylogging++.h"

using namespace common::util;

class ConfigurationReader::Impl
{
public:
    bool autoUpdate_;
    QString configPath_;
    QDomDocument configDoc_;

    Impl(bool autoUpdate)
        : autoUpdate_(autoUpdate)
    {
    }

    bool loadConfigFile(const QString& filePath)
    {
        configPath_ = filePath;
        QFile confFile(filePath);
        if (!confFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            LOG(TRACE) << "Failed to open configuration file [" << configPath_ << "].";
            return false;
        }

        QTextStream streamReader(&confFile);
        return loadXmlString(streamReader.readAll());
    }

    bool loadXmlString(const QString& xmlData)
    {
        int errorLine, errorColumn = 0;
        QString errorMsg;
        if (!configDoc_.setContent(xmlData, &errorMsg, &errorLine, &errorColumn))
        {
            LOG_IF(!configPath_.isEmpty(), TRACE) << "Failed to load configuration from [" << configPath_ << "].";
            LOG(DEBUG) << "Configuration not valid [" << errorMsg << "]-[" << errorLine << "][" << errorColumn << "].";
            return false;
        }

        return true;
    }
};

ConfigurationReader::ConfigurationReader(bool autoUpdate)
    : impl_(autoUpdate)
{
}

ConfigurationReader::~ConfigurationReader()
{
}

bool ConfigurationReader::loadFile(const QString& filePath)
{
    return impl_->loadConfigFile(filePath);
}

bool ConfigurationReader::loadXml(const QString& xmlData)
{
    return impl_->loadXmlString(xmlData);
}

bool ConfigurationReader::save(bool overwrite) const
{
    return true;
}

bool ConfigurationReader::save(const QString& newPath, bool overwrite) const
{
    return true;
}

QDomDocument ConfigurationReader::getConfiguration(bool detached) const
{
    return detached ? impl_->configDoc_.cloneNode().toDocument() : impl_->configDoc_;
}
