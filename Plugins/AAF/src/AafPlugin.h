#ifndef PLUGINS_AAF_SRC_AAFPLUGIN_H
#define PLUGINS_AAF_SRC_AAFPLUGIN_H

#include <QObject>

#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

namespace plugins
{
namespace aaf
{

class AafPlugin final : public QObject, public interfaces::EdlInterface, private interfaces::SharedLoggerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface plugins::interfaces::SharedLoggerInterface)

public:
    /*!
     * \brief AafPlugin constructor.
     */
    AafPlugin();

    // EdlInterface interface
    /*!
    * \sa {interfaces::EdlInterface::getEdlName}
    */
    std::wstring getEdlName() const override final { return L"AAF"; }

    /*!
    * \sa {interfaces::EdlInterface::getEdlExtension}
    */
    std::wstring getEdlExtension() const override final { return L".aaf"; }

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
};

}
}

#endif // PLUGINS_AAF_SRC_AAFPLUGIN_H
