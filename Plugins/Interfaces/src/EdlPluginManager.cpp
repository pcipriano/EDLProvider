#include "EdlPluginManager.h"

using namespace plugins::interfaces;

EdlPluginManager::EdlPluginManager(const QString& pluginsLocation, bool autoUpdate)
    : loader_(new common::util::PluginLoader<EdlInterface>(pluginsLocation))
{
    if (autoUpdate)
    {
        dirWatcher_.addPath(pluginsLocation);

        connect(&dirWatcher_,
                &QFileSystemWatcher::directoryChanged,
                this,
                &EdlPluginManager::handlePluginsDirChange);
    }
}

void EdlPluginManager::handlePluginsDirChange(const QString& path)
{
    //Update the plugins list
    loader_.reset(new common::util::PluginLoader<EdlInterface>(path));
}
