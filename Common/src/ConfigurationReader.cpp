#include "ConfigurationReader.h"

#include <QFile>
#include <QXmlSchema>
#include <QTextStream>
#include <QXmlSchemaValidator>

#include "easylogging++.h"

using namespace common::util;

class ConfigurationReader::Impl
{
public:
    bool autoUpdate_;
    QUrl schemaPath_;
    bool isValid_ = false;
    QString configPath_;
    QDomDocument configDoc_;

    Impl(bool autoUpdate)
        : autoUpdate_(autoUpdate)
    {
    }

    Impl(const QUrl& schemaPath, bool autoUpdate)
        : autoUpdate_(autoUpdate),
          schemaPath_(schemaPath)
    {
    }

    bool loadConfigFile(const QString& filePath)
    {
        isValid_ = false;
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
        isValid_ = false;
        int errorLine, errorColumn = 0;
        QString errorMsg;
        if (!configDoc_.setContent(xmlData, &errorMsg, &errorLine, &errorColumn))
        {
            LOG_IF(!configPath_.isEmpty(), TRACE) << "Failed to load configuration from [" << configPath_ << "].";
            LOG(DEBUG) << "Configuration not valid [" << errorMsg << "]-[" << errorLine << "][" << errorColumn << "].";
            return false;
        }

        if (schemaPath_.isValid() && schemaPath_.isLocalFile() && !schemaPath_.isRelative())
        {
            QXmlSchema schema;
            schema.load(schemaPath_);
            if (!schema.isValid())
            {
                LOG(TRACE) << "Schema [" << schemaPath_.toString() << "] not valid.";
                return false;
            }

            QXmlSchemaValidator validator(schema);
            if (!validator.validate(xmlData.toUtf8()))
            {
                LOG_IF(!configPath_.isEmpty(), TRACE) << "Validation of file [" << configPath_ << "] failed.";
                return false;
            }
        }

        isValid_ = true;
        return true;
    }

    bool saveConfig(bool overWrite) const
    {
        return saveConfig(configPath_, overWrite);
    }

    bool saveConfig(const QString& path, bool overWrite) const
    {
        if (path.isEmpty())
        {
            LOG(TRACE) << "Path not set when saving configuration.";
            return false;
        }

        if (!overWrite && QFile::exists(path))
        {
            LOG(TRACE) << "File [" << path << "] already exists and overwrite is false.";
            return false;
        }

        QFile saveFile(path);
        if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            LOG(TRACE) << "Could not open file [" << path << "] for saving.";
            return false;
        }

        QTextStream streamWriter(&saveFile);
        configDoc_.save(streamWriter, 4);
        return true;
    }
};

ConfigurationReader::ConfigurationReader(bool autoUpdate)
    : impl_(autoUpdate)
{
}

ConfigurationReader::ConfigurationReader(const QString& schemaPath, bool autoUpdate)
    : impl_(QUrl::fromLocalFile(schemaPath), autoUpdate)
{
}

ConfigurationReader::~ConfigurationReader()
{
}

bool ConfigurationReader::getIsValid()
{
    return impl_->isValid_;
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
    return impl_->saveConfig(overwrite);
}

bool ConfigurationReader::save(const QString& newPath, bool overwrite) const
{
    return impl_->saveConfig(newPath, overwrite);
}

QDomDocument ConfigurationReader::getConfiguration(bool detached) const
{
    return detached ? impl_->configDoc_.cloneNode().toDocument() : impl_->configDoc_;
}
