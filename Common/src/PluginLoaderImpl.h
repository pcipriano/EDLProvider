#ifndef COMMON_SRC_PLUGINLOADERIMPL_H
#define COMMON_SRC_PLUGINLOADERIMPL_H

#include <QPluginLoader>

#include "PluginLoader.h"

using namespace common::util;

template<typename T>
PluginLoader<T>::PluginLoader(const QString& pluginsLocation)
    : pluginsFolder_(pluginsLocation)
{
    for (const QString& fileName : pluginsFolder_.entryList(QDir::Files))
    {
        if (QLibrary::isLibrary(fileName))
        {
            std::unique_ptr<QPluginLoader> loader(new QPluginLoader(fileName));

            if (qobject_cast<T*>(loader->instance()))
                plugins_.push_back(std::move(loader));
        }
    }
}

template<typename T>
PluginLoader<T>::~PluginLoader()
{
    //unload all plugins
    for (auto& plugin : plugins_)
        plugin->unload();
}

template<typename T>
QList<T*> PluginLoader<T>::getInstances()
{
    QList<T*> result;
    for (const auto& plugin : plugins_)
        result.append(qobject_cast<T*>(plugin->instance()));

    return result;
}

#endif // COMMON_SRC_PLUGINLOADERIMPL_H
