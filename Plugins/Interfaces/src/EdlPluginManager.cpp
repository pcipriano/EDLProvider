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


plugins::interfaces::EdlInterface* const plugins::interfaces::EdlPluginManager::findEdl(const std::wstring& edlType) const
{
    auto instances = loader_->getInstances();
    QList<plugins::interfaces::EdlInterface*>::const_iterator edl = std::find_if(instances.cbegin(),
                            instances.cend(),
                            [&](const EdlInterface* const interface)
                            { return interface->getEdlName() == edlType; });

    if (edl == instances.cend())
        return nullptr;

    return *edl;
}
