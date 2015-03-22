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

/*!
 * \brief The FinalCut class provides functionality to write Final Cut Pro XML interchange format.
 */
class FinalCut final : public QObject, public interfaces::EdlInterface, private interfaces::SharedLoggerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface plugins::interfaces::SharedLoggerInterface)

public:
    // EdlInterface interface
    /*!
    * \sa {interfaces::EdlInterface::getEdlName}
    */
    std::wstring getEdlName() const override final { return L"FinalCut"; }

    /*!
    * \sa {interfaces::EdlInterface::getEdlExtension}
    */
    std::wstring getEdlExtension() const override final { return L".xml"; }

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
     * \brief Processes frame rate information to extract details to be used in the finalcut EDL.
     * \param frameRate The frame rate information to process.
     * \param message The message to write in case the processing is unsuccessful.
     * \param isDrop Set to \c true if the frame rate is drop frame (NTSC).
     * \param timeBase The rounded time base of the frame rate.
     */
    void processFrameRate(const fims__RationalType* const frameRate,
                          const std::string& message,
                          bool& isDrop,
                          uint32_t& timeBase) const;

    /*!
     * \brief Helper function to write rate section of FCP XML.
     * \param dropFrame True if is drop frame.
     * \param timeBase The time base of the frame rate.
     * \param writer The XML stream writer where to write the rate section.
     */
    void writeRateSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const;

    /*!
     * \brief Helper function to write timecode section of FCP XML.
     * \param dropFrame True if is drop frame.
     * \param timeBase The time base of the frame rate.
     * \param writer The XML stream writer where to write the rate section.
     */
    void writeTimecodeSection(bool dropFrame, uint32_t timeBase, QXmlStreamWriter& writer) const;

    /*!
     * \brief Helper function to write format section of FCP XML.
     * \param dropFrame True if is drop frame.
     * \param timeBase The time base of the frame rate.
     * \param videoInfo Video information.
     * \param writer The XML stream writer where to write the rate section.
     */
    void writeFormatSection(bool dropFrame,
                            uint32_t timeBase,
                            const fims__VideoFormatType* const videoInfo,
                            QXmlStreamWriter& writer) const;

    /*!
     * \brief Helper function to write audio description section of FCP XML.
     * \param audioInfo Audio information.
     * \param nrAudioTracks The number of audio tracks in the clip.
     * \param nrAudioChannels The number of audio channels in each track of the clip.
     * \param writer The XML stream writer where to write the rate section.
     */
    void writeAudioDescription(const fims__AudioFormatType* const audioInfo,
                               uint16_t nrAudioTracks,
                               uint16_t nrAudioChannels,
                               QXmlStreamWriter& writer) const;

    /*!
     * \brief Helper function to write link descriptions section of FCP XML.
     * \param clipUniqueId The clip unique id to use for referencing when linking.
     * \param clipPos The clip position in the index.
     * \param nrAudioTracks The number of audio tracks in the clip.
     * \param nrAudioChannels The number of audio channels for each track.
     * \param clipPathInfo The file path of the clip.
     * \param writer The XML stream writer where to write the rate section.
     */
    void writeLinkDescriptions(const QString& clipUniqueId,
                               size_t clipPos,
                               uint16_t nrAudioTracks,
                               uint16_t nrAudioChannels,
                               const QFileInfo& clipPathInfo,
                               QXmlStreamWriter& writer) const;
};

}
}

#endif // PLUGINS_FINALCUT_SRC_FINALCUT_H
