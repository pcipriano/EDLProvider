#ifndef PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
#define PLUGINS_INTERFACES_SRC_EDLINTERFACE_H

#define EDL_INTERFACE_IDENTIFIER "edl.EdlInterface"

#include <string>
#include <QtPlugin>

namespace plugins
{
namespace interfaces
{

/*!
 * \brief The EdlInterface is the interface defining the operations of each EDL.
 */
struct EdlInterface
{
    /*!
     * \brief ~EdlInterface destructor.
     */
    virtual ~EdlInterface() { }

    /*!
     * \brief Gets the EDL identifier name.
     * \return A \c std::wstring identifying the EDL plugin.
     */
    virtual std::wstring getEdlName() const = 0;
};

}
}

Q_DECLARE_INTERFACE(plugins::interfaces::EdlInterface, EDL_INTERFACE_IDENTIFIER)

#endif // PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
