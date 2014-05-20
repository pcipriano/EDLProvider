#ifndef PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
#define PLUGINS_INTERFACES_SRC_EDLINTERFACE_H

#define EDL_INTERFACE_IDENTIFIER "edl.EdlInterface"

#include <string>
#include <QtPlugin>

#include "EDLProviderStub.h"

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

    /*!
     * \brief Gets the EDL recommended file extension to use when creating EDL files.
     * \return The EDL file extension (eg: .txt, .xml)
     */
    virtual std::wstring getEdlExtension() const = 0;

    /*!
     * \brief Creates an EDL using the suplied parameters.
     * \param edlSequenceName The sequence name to use inside the EDL, if supported.
     * \param edlFrameRate The frame rate to use on the EDL (some EDLs support clips with different frame rates).
     * \param clips The clips to add in the EDL.
     * \return The EDL data.
     */
    virtual QByteArray createEdl(const std::wstring* const edlSequenceName,
                                 const fims__RationalType* const edlFrameRate,
                                 const std::vector<edlprovider__ClipType*>& clips) const = 0;
};

}
}

Q_DECLARE_INTERFACE(plugins::interfaces::EdlInterface, EDL_INTERFACE_IDENTIFIER)

#endif // PLUGINS_INTERFACES_SRC_EDLINTERFACE_H
