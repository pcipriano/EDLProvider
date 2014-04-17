#ifndef PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
#define PLUGINS_INTERFACES_SRC_EDLINTERFACE_H

#include <string>

namespace plugins
{
namespace interfaces
{

struct EdlInterface
{
    virtual ~EdlInterface() { }

    virtual std::wstring getEdlName() const = 0;
};

}
}

Q_DECLARE_INTERFACE(plugins::interfaces::EdlInterface, "edl.EdlInterface")

#endif // PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
