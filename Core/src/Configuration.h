#ifndef CORE_SRC_CONFIGURATION_H
#define CORE_SRC_CONFIGURATION_H

#include "ConfigurationReader.h"

namespace edlprovider
{
namespace core
{

/*!
 * \brief The Configuration class provides the read values for the application to use.
 */
class Configuration
{
public:
    /*!
     * \brief Configuration constructor.
     */
    Configuration();

    /*!
     * \brief Loads the specified configuration file.
     * \param configPath The configuration file path to read.
     * \return True if the configuration is successfully read.
     */
    bool load(const QString& configPath);

    /*!
     * \brief Gets the port to use in the soap service.
     * \return The configured port found in the configuration.
     */
    quint16 getServicePort() const;

    /*!
     * \brief Gets the service host name to use in the soap service.
     *
     *  If not specified local host is used.
     *
     * \return The host name found in the configuration.
     */
    QString getServiceHostName() const;

    /*!
     * \brief Gets flag to automatically update the available EDL plugins.
     * \return The flag to enable/disable the automatic update of the EDL plugins.
     */
    bool getPluginsAutoUpdate() const;

private:
    common::util::ConfigurationReader configReader_;    //!< The configuration reader to get the values from.
};

}
}

#endif // CORE_SRC_CONFIGURATION_H
