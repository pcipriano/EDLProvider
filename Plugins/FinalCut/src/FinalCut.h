#ifndef PLUGINS_FINALCUT_SRC_FINALCUT_H
#define PLUGINS_FINALCUT_SRC_FINALCUT_H

#include <QObject>

#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

namespace plugins
{
namespace finalcut
{

class FinalCut final : public QObject, public interfaces::EdlInterface, private interfaces::SharedLoggerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface plugins::interfaces::SharedLoggerInterface)

public:
    FinalCut();

    // EdlInterface interface
    /*!
    * \sa {EdlInterface::getEdlName}.
    */
    std::wstring getEdlName() const override final { return L"FinalCut"; }

    /*!
    * \sa {EdlInterface::getEdlExtension}.
    */
    std::wstring getEdlExtension() const override final { return L".xml"; }

    /*!
    * \sa {EdlInterface::createEdl}.
    */
    QByteArray createEdl(const std::wstring* const edlSequenceName,
                         const fims__RationalType* const edlFrameRate,
                         const std::vector<edlprovider__ClipType*>& clips) const;

    void setEasyloggingStorage(el::base::type::StoragePointer storage) final override;
};

}
}

#endif // PLUGINS_FINALCUT_SRC_FINALCUT_H
