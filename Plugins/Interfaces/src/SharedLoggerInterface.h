#ifndef PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H
#define PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H

#define SHARED_LOGGER_INTERFACE_IDENTIFIER "edl.SharedLoggerInterface"

#include <QtPlugin>

#include "easylogging++.h"

namespace plugins
{
namespace interfaces
{

struct SharedLoggerInterface
{
public:
    virtual ~SharedLoggerInterface() { }

    virtual void setEasyloggingStorage(el::base::type::StoragePointer storage) { el::Helpers::setStorage(storage); }
};

}
}

Q_DECLARE_INTERFACE(plugins::interfaces::SharedLoggerInterface, SHARED_LOGGER_INTERFACE_IDENTIFIER)

#endif // PLUGINS_INTERFACES_SRC_SHAREDLOGGERINTERFACE_H
