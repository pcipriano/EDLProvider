#ifndef PLUGINS_FINALCUT_SRC_FINALCUT_H
#define PLUGINS_FINALCUT_SRC_FINALCUT_H

#include <QObject>

#include "EdlInterface.h"
#include "SharedLoggerInterface.h"

class QFileInfo;
class QXmlStreamWriter;

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

    //SharedLoggerInterface interface
    /*!
    * \sa {SharedLoggerInterface::setEasyloggingStorage}.
    */
    void setEasyloggingStorage(el::base::type::StoragePointer storage) override final;

private:
    /*!
     * \brief Processes frame rate information to extract details to be used in the finalcut EDL.
     * \param frameRate The frame rate information to process.
     * \param isDrop Set to \c True if the frame rate is drop frame (NTSC).
     * \param timeBase The rounded time base of the frame rate.
     */
    void processFrameRate(const fims__RationalType* const frameRate, bool& isDrop, uint32_t& timeBase) const;

    void writeRateSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const;

    void writeTimecodeSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const;

    void writeFormatSection(bool dropFrame,
                            uint32_t timeBase,
                            const fims__VideoFormatType* const videoInfo,
                            QXmlStreamWriter& writer) const;

    void writeAudioDescription(ushort nrAudioTracks, ushort nrAudioChannels, QXmlStreamWriter& writer) const;

    void writeLinkDescriptions(const QString& clipUniqueId,
                               size_t clipPos,
                               ushort nrAudioTracks,
                               ushort nrAudioChannels,
                               const QFileInfo& clipPathInfo,
                               QXmlStreamWriter& writer) const;
};

}
}

#endif // PLUGINS_FINALCUT_SRC_FINALCUT_H
