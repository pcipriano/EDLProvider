#ifndef COMMON_SRC_PLUGINLOADER_H
#define COMMON_SRC_PLUGINLOADER_H

#include <memory>
#include <QDir>
#include <QPluginLoader>

namespace common
{
namespace util
{

/*!
 * \brief The PluginLoader class is a helper class for loading all the plugins form a specified folder.
 *
 */
template<typename T>
class PluginLoader
{
public:
    /*!
     * \brief PluginLoader constructor.
     * \param pluginsLocation The path location of the directory where the plugins are located.
     */
    PluginLoader(const QString& pluginsLocation);

    /*!
     * \brief ~PluginLoader destructor.
     */
    ~PluginLoader();

    /*!
     * \brief Gets all the implementations of the interface \c T.
     * \return List of instanced interfaces found.
     */
    QList<T*> getInstances();

    /*!
     * \brief Path where this object is checking for plugins.
     * \return The path location of the directory where the plugins are located.
     */
    QString pluginsFolderPath() const { return pluginsFolder_.absolutePath(); }

private:
    QDir pluginsFolder_;        //!< The path where the plugins are located.

    std::vector<std::unique_ptr<QPluginLoader>> plugins_;       //!< List of plugin loaders.
};

}
}

#include "PluginLoaderImpl.h"

#endif // COMMON_SRC_PLUGINLOADER_H
