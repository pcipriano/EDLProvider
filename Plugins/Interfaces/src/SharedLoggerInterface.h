#ifndef PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H
#define PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H

#define SHARED_LOGGER_INTERFACE_IDENTIFIER "edl.SharedLoggerInterface"

#include <QtPlugin>

#include "easylogging++.h"

namespace plugins
{
namespace interfaces
{

/*!
 * \brief The SharedLoggerInterface struct is a helper interface to help on using the application logger in EDL plugins.
 */
struct SharedLoggerInterface
{
public:
    /*!
     * \brief ~SharedLoggerInterface destructor.
     */
    virtual ~SharedLoggerInterface() { }

    /*!
     * \brief Sets the easy logging storage.
     * \param storage The easy logging storage to set.
     */
    virtual void setEasyloggingStorage(el::base::type::StoragePointer storage) { el::Helpers::setStorage(storage); }
};

}
}

Q_DECLARE_INTERFACE(plugins::interfaces::SharedLoggerInterface, SHARED_LOGGER_INTERFACE_IDENTIFIER)

#endif // PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H
