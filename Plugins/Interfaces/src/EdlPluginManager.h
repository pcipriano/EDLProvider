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

/*!
 * \brief The EdlPluginManager is a helper class for managing all the EDL's plugins.
 *
 * Loads all the plugins and auto loads new ones if the option is turned on in the configuration.
 *
 */
class EdlPluginManager : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief EdlPluginManager constructor.
     * \param pluginsLocation The EDL plugins folder location, normally 'appworkingdir/plugins'.
     * \param autoUpdate If true EDL plugins added when the application is running will be loaded automatically.
     */
    explicit EdlPluginManager(const QString& pluginsLocation, bool autoUpdate = false);

    /*!
     * \brief Gets all the EDL plugins found.
     * \return List of all available EDL plugins.
     */
    QList<EdlInterface*> getEdls() const { return edlLoader_->getInstances(); }

    /*!
     * \brief Searches for the specified EDL plugin and returns \c nullptr if not found.
     * \param edlType The EDL plugin to search for.
     * \return The EDL plugin interface if found \c nullptr otherwise.
     */
    EdlInterface* const findEdl(const std::wstring& edlType) const;

private Q_SLOTS:
    /*!
     * \brief Handles changes in the plugins directory.
     * \param path The directory path where the changes occurred.
     */
    void handlePluginsDirChange(const QString& path);

private:
    QFileSystemWatcher dirWatcher_;     //!< File system watcher for the plugins directory.

    QScopedPointer<common::util::PluginLoader<SharedLoggerInterface>> loggerLoader_;    //!< Plugin loader for SharedLoggerInterface.

    QScopedPointer<common::util::PluginLoader<EdlInterface>> edlLoader_;    //!< Plugin loader for EdlInterface.
};

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLPLUGINMANAGER_H
