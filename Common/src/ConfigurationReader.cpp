#include "ConfigurationReader.h"

#include <QFile>
#include <QXmlSchema>
#include <QTextStream>
#include <QFileSystemWatcher>
#include <QXmlSchemaValidator>

#include "easylogging++.h"

using namespace common::util;

class ConfigurationReader::Impl : public QObject
{
public:
    bool autoUpdate_;
    QUrl schemaPath_;
    bool isValid_ = false;
    QString configPath_;
    QDomDocument configDoc_;
    QFileSystemWatcher fileWatcher_;

    Impl(bool autoUpdate)
        : Impl(QUrl(), autoUpdate)
    {
    }

    Impl(const QUrl& schemaPath, bool autoUpdate)
        : autoUpdate_(autoUpdate),
          schemaPath_(schemaPath)
    {
        if (autoUpdate)
        {
            connect(&fileWatcher_,
                    &QFileSystemWatcher::fileChanged,
                    this,
                    &Impl::handleConfigurationChange);
        }
    }

    bool loadConfigFile(const QString& filePath)
    {
        isValid_ = false;
        configPath_ = filePath;
        bool ret = readConfigFile(filePath);
        if (ret)
            updateWatcherPath(filePath);

        return ret;
    }

    bool readConfigFile(const QString& filePath)
    {
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

    void changeFilePath(const QString& filePath)
    {
        if (filePath != configPath_)
        {
            configPath_ = filePath;
            updateWatcherPath(filePath);
        }
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

    bool setValue(const QStringList& elementPath, const QString& value, bool create)
    {
        if (elementPath.isEmpty())
            return false;

        auto parentElement = configDoc_.documentElement();

        QDomNode parentNode;
        for (const QString& elementName : elementPath)
        {
            parentNode = parentElement.namedItem(elementName);

            if (parentNode.isNull() && create)
            {
                parentNode = configDoc_.createElement(elementName);
                parentElement.appendChild(parentNode);
            }
            else if (parentNode.isNull() || !parentNode.isElement())
            {
                return false;
            }

            parentElement = parentNode.toElement();
        }

        if (!parentNode.hasChildNodes() && create)
        {
            parentNode.appendChild(configDoc_.createTextNode(value));
            return true;
        }
        else
        {
            QDomNodeList nodeList = parentNode.childNodes();
            for (int i = 0; i < nodeList.length(); i++)
            {
                //On first text node found change the value.
                if (nodeList.at(i).isText())
                {
                    nodeList.at(i).setNodeValue(value);
                    return true;
                }
            }

            //If create and no text node found create it and set the value.
            if (create)
            {
                parentNode.appendChild(configDoc_.createTextNode(value));
                return true;
            }
        }

        return false;
    }

    void updateWatcherPath(const QString& newPath)
    {
        if (autoUpdate_)
        {
            //Remove file being watched and add new path
            fileWatcher_.removePaths(fileWatcher_.files());
            fileWatcher_.addPath(newPath);
        }
    }

public Q_SLOTS:
    void handleConfigurationChange(const QString& path)
    {
        //Check first if file was not renamed or removed
        if (!QFile::exists(path))
        {
            LOG(TRACE) << "Configuration file [" << path << "] does not exist anymore in the filesystem.";
            return;
        }

        if (!readConfigFile(path))
            LOG(TRACE) << "Failed to load configuration file [" << path << "] after file changed.";
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

void ConfigurationReader::setFilePath(const QString& filePath)
{
    impl_->changeFilePath(filePath);
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

bool ConfigurationReader::setValue(const QString& elementName, const char* value, bool create)
{
    return setValue(elementName, QString(value), create);
}

bool ConfigurationReader::setValue(const QStringList& elementPath, const char* value, bool create)
{
    return setValue(elementPath, QString(value), create);
}

bool ConfigurationReader::setValue(const QString& elementName, const QString& value, bool create)
{
    return setValue(QStringList() << elementName, value, create);
}

bool ConfigurationReader::setValue(const QStringList& elementPath, const QString& value, bool create)
{
    return impl_->setValue(elementPath, value, create);
}
