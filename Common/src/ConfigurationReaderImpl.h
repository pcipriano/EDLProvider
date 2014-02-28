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
QList<std::pair<QString, T>> ConfigurationReader::getSection(const QString& parentElementName, bool* ok)
{
    return getSection<T>(QStringList() << parentElementName, ok);
}

template<typename T>
QList<std::pair<QString, T>> ConfigurationReader::getSection(const QStringList& elementPath, bool* ok)
{
    if (elementPath.isEmpty())
        return QList<std::pair<QString, T>>();

    if (ok != nullptr)
        *ok = false;

    auto parentElement = getConfiguration().documentElement();

    QDomNode parentNode;
    for (const QString& elementName : elementPath)
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
    if (elementPath.isEmpty())
        return false;

    auto configDoc = getConfiguration();

    auto parentElement = configDoc.documentElement();

    QDomNode parentNode;
    for (const QString& elementName : elementPath)
    {
        parentNode = parentElement.namedItem(elementName);

        if ((parentNode.isNull() || !parentNode.isElement()) && create)
        {
            parentNode = configDoc.createElement(elementName);
            parentElement.appendChild(parentNode);
        }
        else
            return false;

        parentElement = parentNode.toElement();
    }

    QString valueConverted = convertToString(value, typename std::is_fundamental<T>::type());

    if (!parentNode.hasChildNodes() && create)
    {
        parentNode.appendChild(configDoc.createTextNode(valueConverted));
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
                nodeList.at(i).setNodeValue(valueConverted);
                return true;
            }
        }

        //If create and no text node found create it and set the value.
        if (create)
        {
            parentNode.appendChild(configDoc.createTextNode(valueConverted));
            return true;
        }
    }

    return false;
}

}
}

#endif // COMMON_SRC_CONFIGURATIONREADERIMPL_H
