#ifndef PLUGINS_AAF_SRC_AAFPLUGIN_H
#define PLUGINS_AAF_SRC_AAFPLUGIN_H

#include <QObject>

#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

#include <AAFTypes.h>

namespace plugins
{
namespace aaf
{

/*!
 * \brief The AAFPlugin class provides functionality to write AAF.
 */
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

private:
    /*!
     * \brief Processes frame rate information to extract details to be used in the AAF EDL.
     * \param frameRate The frame rate information to process.
     * \param message The message to write in case the processing is unsuccessful.
     * \param isDrop Set to \c true if the frame rate is drop frame (NTSC).
     * \param editRate The edit rate extracted from the \p frameRate input.
     */
    void processFrameRate(const fims__RationalType* const frameRate,
                          const std::string& message,
                          bool& isDrop,
                          aafRational_t& editRate) const;
};

}
}

#endif // PLUGINS_AAF_SRC_AAFPLUGIN_H
