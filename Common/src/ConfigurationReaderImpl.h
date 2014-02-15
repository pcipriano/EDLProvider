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
T ConfigurationReader::getValue(const QString& elementName, bool* ok) const
{
    if (ok != nullptr)
        *ok = false;

    auto rootElement = getConfiguration().documentElement();

    auto node = rootElement.namedItem(elementName);

    if (node.isNull() || !node.isElement())
        return T();

    auto element = node.toElement();
    if (element.isNull())
        return T();

    return convertToType<T>(element.text(), ok);
}

template<typename T>
T ConfigurationReader::getValueOrDefault(const QString& elementName, const T& defaultValue) const
{
    bool ok;
    auto ret = getValue<T>(elementName, &ok);
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

        parentElement = parentNode;
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

}
}

#endif // COMMON_SRC_CONFIGURATIONREADERIMPL_H
