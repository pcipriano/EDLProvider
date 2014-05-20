#ifndef PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H
#define PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H

#include <QFileSystemWatcher>

#include "PluginLoader.h"
#include "EdlInterface.h"

namespace plugins
{
namespace interfaces
{

class EdlPluginManager : public QObject
{
    Q_OBJECT

public:
    explicit EdlPluginManager(const QString& pluginsLocation, bool autoUpdate = false);

    QList<EdlInterface*> getEdls() const { return loader_->getInstances(); }

    EdlInterface* const findEdl(const std::wstring& edlType) const;

private Q_SLOTS:
    void handlePluginsDirChange(const QString& path);

private:
    QFileSystemWatcher dirWatcher_;

    QScopedPointer<common::util::PluginLoader<EdlInterface>> loader_;
};

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H
