#ifndef PLUGINS_CMX_SRC_CMX3600_H
#define PLUGINS_CMX_SRC_CMX3600_H

#include <QObject>

#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

namespace plugins
{
namespace cmx
{

/*!
 * \brief The Cmx3600 class provides functionality to write CMX3600.
 */
class Cmx3600 final : public QObject, public interfaces::EdlInterface, private interfaces::SharedLoggerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface plugins::interfaces::SharedLoggerInterface)

public:
    // EdlInterface interface
    /*!
    * \sa {interfaces::EdlInterface::getEdlName}
    */
    std::wstring getEdlName() const override final { return L"CMX3600"; }

    /*!
    * \sa {interfaces::EdlInterface::getEdlExtension}
    */
    std::wstring getEdlExtension() const override final { return L".edl"; }

    /*!
    * \sa {interfaces::EdlInterface::createEdl}
    */
    QByteArray createEdl(const std::wstring* const edlSequenceName,
                         const fims__RationalType* const edlFrameRate,
                         const std::vector<edlprovider__ClipType*>& clips) const override final;

    //SharedLoggerInterface interface
    /*!
    * \sa {interfaces::SharedLoggerInterface::setEasyloggingStorage}
    */
    void setEasyloggingStorage(el::base::type::StoragePointer storage) override final;

private:
    /*!
     * \brief Helper function to check if a frame rate is drop frame or non drop.
     * \param frameRate The frame rate to check.
     * \return \c true if frame rate is drop frame.
     */
    bool isDropFrame(const fims__RationalType* const frameRate) const;
};

}
}

#endif // PLUGINS_CMX_SRC_CMX3600_H
