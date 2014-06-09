#ifndef PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H
#define PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H

#include <QFileSystemWatcher>

#include "PluginLoader.h"
#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

namespace plugins
{
namespace interfaces
{

class EdlPluginManager : public QObject
{
    Q_OBJECT

public:
    explicit EdlPluginManager(const QString& pluginsLocation, bool autoUpdate = false);

    QList<EdlInterface*> getEdls() const { return edlLoader_->getInstances(); }

    EdlInterface* const findEdl(const std::wstring& edlType) const;

private Q_SLOTS:
    void handlePluginsDirChange(const QString& path);

private:
    QFileSystemWatcher dirWatcher_;

    QScopedPointer<common::util::PluginLoader<SharedLoggerInterface>> loggerLoader_;

    QScopedPointer<common::util::PluginLoader<EdlInterface>> edlLoader_;
};

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H
