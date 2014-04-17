#ifndef COMMON_SRC_PLUGINLOADER_H
#define COMMON_SRC_PLUGINLOADER_H

#include <memory>
#include <QDir>
#include <QPluginLoader>

namespace common
{
namespace util
{

template<typename T>
class PluginLoader
{
public:
    PluginLoader(const QString& pluginsLocation);

    ~PluginLoader();

    QList<T*> getInstances();

    QString pluginsFolderPath() const { return pluginsFolder_.absolutePath(); }

private:
    QDir pluginsFolder_;

    std::vector<std::unique_ptr<QPluginLoader>> plugins_;
};

}
}

#include "PluginLoaderImpl.h"

#endif // COMMON_SRC_PLUGINLOADER_H
