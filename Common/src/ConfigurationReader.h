#ifndef COMMON_SRC_CONFIGURATIONREADER_H
#define COMMON_SRC_CONFIGURATIONREADER_H

#include <QUrl>
#include <QString>
#include <QDomDocument>

#include "Pimpl.h"
#include "Uncopyable.h"

namespace common
{
namespace util
{

/*!
 * \brief The ConfigurationReader manages configurations, reading and writing modifications.
 *
 *  The configuration must be in XML format.
 */
class ConfigurationReader : Uncopyable
{
public:
    explicit ConfigurationReader(bool autoUpdate = false);

    explicit ConfigurationReader(const QString& schemaPath, bool autoUpdate = false);

    explicit ConfigurationReader(const QUrl& schemaPath, bool autoUpdate = false);

    ~ConfigurationReader();

    bool getIsValid();

    bool loadFile(const QString& filePath);

    bool loadXml(const QString& xmlData);

    void setFilePath(const QString& filePath);

    bool save(bool overwrite = true) const;

    bool save(const QString& newPath, bool overwrite = true) const;

    QDomDocument getConfiguration(bool detached = false) const;

    template<typename T>
    T getValue(const QString& elementName, bool* ok = nullptr) const;

    template<typename T>
    T getValue(const QStringList& elementPath, bool* ok = nullptr) const;

    template<typename T>
    T getValueOrDefault(const QString& elementName, const T& defaultValue = T()) const;

    template<typename T>
    T getValueOrDefault(const QStringList& elementPath, const T& defaultValue = T()) const;

    template<typename T>
    QList<std::pair<QString, T>> getSection(const QString& parentElementName, bool* ok = nullptr);

    template<typename T>
    QList<std::pair<QString, T>> getSection(const QStringList& elementPath, bool* ok = nullptr);

    template<typename T>
    bool setValue(const QString& elementName, const T& value, bool create = false);

    template<typename T>
    bool setValue(const QStringList& elementPath, const T& value, bool create = false);

private:
    class Impl;
    Pimpl<Impl> impl_;
};

}
}

#include "ConfigurationReaderImpl.h"

#endif // COMMON_SRC_CONFIGURATIONREADER_H
