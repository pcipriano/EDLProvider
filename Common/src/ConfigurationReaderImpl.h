#ifndef COMMON_SRC_CONFIGURATIONREADERIMPL_H
#define COMMON_SRC_CONFIGURATIONREADERIMPL_H

#include <QVariant>

namespace common
{
namespace util
{

template<typename T>
static T convertToType(const QString& value, bool* ok)
{
    //If value is empty return true because that means it exists but has no value
    //it can be a choice element for example.
    if (value.isEmpty())
    {
        if (ok != nullptr)
            *ok = true;

        return T();
    }

    auto variant = QVariant::fromValue(value);
    if (variant.canConvert<T>())
    {
        if (ok != nullptr)
            *ok = true;

        return variant.value<T>();
    }

    return T();
}

template<typename T>
static QString convertToString(const T& value, std::true_type)
{
    return QString::number(value);
}

template<typename T>
static QString convertToString(const T& value, std::false_type)
{
    return value.toString();
}

template<typename T>
T ConfigurationReader::getValue(const QString& elementName, bool* ok) const
{
    return getValue<T>(QStringList() << elementName, ok);
}

template<typename T>
T ConfigurationReader::getValue(const QStringList& elementPath, bool* ok) const
{
    if (ok != nullptr)
        *ok = false;

    if (elementPath.isEmpty())
        return T();

    auto parentElement = getConfiguration().documentElement();

    QDomNode parentNode;
    for (const QString& elementName : elementPath)
    {
        parentNode = parentElement.namedItem(elementName);

        if (parentNode.isNull() || !parentNode.isElement())
            return T();

        parentElement = parentNode.toElement();
    }

    auto element = parentNode.toElement();
    return convertToType<T>(element.text(), ok);
}

template<typename T>
T ConfigurationReader::getValueOrDefault(const QString& elementName, const T& defaultValue) const
{
    return getValueOrDefault<T>(QStringList() << elementName, defaultValue);
}

template<typename T>
T ConfigurationReader::getValueOrDefault(const QStringList& elementPath, const T& defaultValue) const
{
    bool ok;
    auto ret = getValue<T>(elementPath, &ok);
    return ok ? ret : defaultValue;
}

template<typename T>
T ConfigurationReader::getAttribute(const QString& attributeName, const QString& elementName, bool* ok) const
{
    return getAttribute<T>(attributeName, elementName.isEmpty() ? QStringList() : QStringList() << elementName, ok);
}

template<typename T>
T ConfigurationReader::getAttribute(const QString& attributeName, const QStringList& elementPath, bool* ok) const
{
    if (ok != nullptr)
        *ok = false;

    auto parentElement = getConfiguration().documentElement();

    QDomNode parentNode;
    for (const QString& elementName : elementPath)
    {
        parentNode = parentElement.namedItem(elementName);

        if (parentNode.isNull() || !parentNode.isElement())
            return T();

        parentElement = parentNode.toElement();
    }

    if (!parentElement.hasAttribute(attributeName))
            return T();

    return convertToType<T>(parentElement.attribute(attributeName), ok);
}

template<typename T>
T ConfigurationReader::getAttributeOrDefault(const QString& attributeName, const QString& elementName, const T& defaultValue) const
{
    bool ok;
    auto ret = getAttribute<T>(attributeName, elementName, &ok);
    return ok ? ret : defaultValue;
}

template<typename T>
T ConfigurationReader::getAttributeOrDefault(const QString& attributeName, const QStringList& elementPath, const T& defaultValue) const
{
    bool ok;
    auto ret = getAttribute<T>(attributeName, elementPath, &ok);
    return ok ? ret : defaultValue;
}

template<typename T>
QList<std::pair<QString, T>> ConfigurationReader::getSection(const QString& parentElementName, bool* ok)
{
    return getSection<T>(QStringList() << parentElementName, ok);
}

template<typename T>
QList<std::pair<QString, T>> ConfigurationReader::getSection(const QStringList& parentElementPath, bool* ok)
{
    if (parentElementPath.isEmpty())
        return QList<std::pair<QString, T>>();

    if (ok != nullptr)
        *ok = false;

    auto parentElement = getConfiguration().documentElement();

    QDomNode parentNode;
    for (const QString& elementName : parentElementPath)
    {
        parentNode = parentElement.namedItem(elementName);

        if (parentNode.isNull() || !parentNode.isElement())
            return QList<std::pair<QString, T>>();

        parentElement = parentNode.toElement();
    }

    bool valid = false;
    QList<std::pair<QString, T>> result;
    QDomNodeList nodes = parentNode.childNodes();
    for (int i = 0; i < nodes.length(); i++)
    {
        if (nodes.at(i).isElement())
        {
            T value = convertToType<T>(nodes.at(i).toElement().text(), &valid);

            if (valid)
                result.append(std::make_pair(nodes.at(i).nodeName(), value));
            else
                break;
        }
    }

    if (ok != nullptr)
        *ok = valid;

    return result;
}

template<typename T>
bool ConfigurationReader::setValue(const QString& elementName, const T& value, bool create)
{
    return setValue(QStringList() << elementName, value, create);
}

template<typename T>
bool ConfigurationReader::setValue(const QStringList& elementPath, const T& value, bool create)
{
    return setValue(elementPath, convertToString(value, typename std::is_fundamental<T>::type()), create);
}

}
}

#endif // COMMON_SRC_CONFIGURATIONREADERIMPL_H
