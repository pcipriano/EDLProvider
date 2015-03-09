#ifndef PLUGINS_INTERFACES_SRC_EDLHELPERS_H
#define PLUGINS_INTERFACES_SRC_EDLHELPERS_H

#include <cstdint>

#include "EDLProviderStub.h"

namespace plugins
{
namespace interfaces
{

/*!
 * \brief Gets number of frames from a \c fimstime__TimeType value.
 * \param time The time information to compute the frame length.
 * \param frameRateNum The frame rate numerator.
 * \param frameRateDen The frame rate denominator.
 * \return Number of frames computed.
 */
uint64_t getNrFrames(const fimstime__TimeType& time, uint32_t frameRateNum, uint32_t frameRateDen);

/*!
 * \brief Gets number of frames from a \c fimstime__DurationType value.
 * \param duration The duration from where to compute the frame length.
 * \param frameRateNum The frame rate numerator.
 * \param frameRateDen The frame rate denominator.
 * \return Number of frames computed.
 */
uint64_t getNrFrames(const fimstime__DurationType& duration, uint32_t frameRateNum, uint32_t frameRateDen);

/*!
 * \brief Processes clip information and returns pointers to important sections of the object.
 * \param clip The clip information to process.
 * \param clipInfo The clip generic information.
 * \param clipFormatInfo The clip duration and location.
 * \param videoInfo Video format information of the clip.
 * \param audioInfo Audio format information of the clip.
 * \param nrAudioTracks Number of audio tracks in the clip.
 * \param nrAudioChannels Number of audio channels per track in the clip.
 */
void processClipInfo(const edlprovider__ClipType* const clip,
                     const fims__BMContentType* clipInfo,
                     const fims__BMContentFormatType* clipFormatInfo,
                     const fims__VideoFormatType* videoInfo,
                     const fims__AudioFormatType* audioInfo,
                     uint16_t& nrAudioTracks,
                     uint16_t& nrAudioChannels);

/*!
 * \brief Helper function to log the clip information.
 * \param clipFormatInfo The clip duration and location.
 * \param clip The whole clip information.
 * \param videoInfo Video format information of the clip.
 * \param audioInfo Audio format information of the clip.
 * \param nrAudioTracks Number of audio tracks in the clip.
 * \param nrAudioChannels Number of audio channels per track in the clip.
 */
void logClipInformation(const fims__BMContentFormatType* const clipFormatInfo,
                        const edlprovider__ClipType* const clip,
                        const fims__VideoFormatType* const videoInfo,
                        const fims__AudioFormatType* const audioInfo,
                        uint16_t nrAudioTracks,
                        uint16_t nrAudioChannels);

/*!
 * \brief Gets the clip duration, mark in/out in frames.
 * \param clip The whole clip information.
 * \param clipFormatInfo The clip duration and location.
 * \param videoInfo Video format information of the clip.
 * \param fileDurationFrames Sets file duration in frames.
 * \param clipMarkInFrames The clip markin in frames.
 * \param clipMarkOutFrames The clip markout in frames.
 * \param clipDurationFrames The clip duration in frames (\p clipMarkOutFrames - \p clipMarkInFrames).
 * \param logValues If \c true the function logs the new values with the logger being used.
 */
void getClipDurationInFrames(const edlprovider__ClipType* const clip,
                             const fims__BMContentFormatType* const clipFormatInfo,
                             const fims__VideoFormatType* const videoInfo,
                             uint64_t& fileDurationFrames,
                             uint64_t& clipMarkInFrames,
                             uint64_t& clipMarkOutFrames,
                             uint64_t& clipDurationFrames,
                             bool logValues = true);

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLHELPERS_H
