#include "EdlPluginManager.h"

using namespace plugins::interfaces;

EdlPluginManager::EdlPluginManager(const QString& pluginsLocation, bool autoUpdate)
    : loggerLoader_(new common::util::PluginLoader<SharedLoggerInterface>(pluginsLocation)),
      edlLoader_(new common::util::PluginLoader<EdlInterface>(pluginsLocation))
{
    for (auto sharedLogger : loggerLoader_->getInstances())
        sharedLogger->setEasyloggingStorage(el::Helpers::storage());

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
    loggerLoader_.reset(new common::util::PluginLoader<SharedLoggerInterface>(path));
    edlLoader_.reset(new common::util::PluginLoader<EdlInterface>(path));

    for (auto sharedLogger : loggerLoader_->getInstances())
        sharedLogger->setEasyloggingStorage(el::Helpers::storage());

}


plugins::interfaces::EdlInterface* const plugins::interfaces::EdlPluginManager::findEdl(const std::wstring& edlType) const
{
    auto instances = edlLoader_->getInstances();
    auto edl = std::find_if(instances.cbegin(),
                            instances.cend(),
                            [&](const EdlInterface* const edlInterface)
                            { return edlInterface->getEdlName() == edlType; });

    if (edl == instances.cend())
        return nullptr;

    return *edl;
}
