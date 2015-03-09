#ifndef PLUGINS_INTERFACES_SRC_EDLHELPERS_H
#define PLUGINS_INTERFACES_SRC_EDLHELPERS_H

#include <cstdint>

#include "EDLProviderStub.h"

namespace plugins
{
namespace interfaces
{

uint64_t getNrFrames(const fimstime__TimeType& time, uint32_t frameRateNum, uint32_t frameRateDen);

uint64_t getNrFrames(const fimstime__DurationType& duration, uint32_t frameRateNum, uint32_t frameRateDen);

void processClipInfo(const edlprovider__ClipType* const clip,
                     const fims__BMContentType* clipInfo,
                     const fims__BMContentFormatType* clipFormatInfo,
                     const fims__VideoFormatType* videoInfo,
                     const fims__AudioFormatType* audioInfo,
                     uint16_t& nrAudioTracks,
                     uint16_t& nrAudioChannels);

void logClipInformation(const fims__BMContentFormatType* const clipFormatInfo,
                        const edlprovider__ClipType* const clip,
                        const fims__VideoFormatType* const videoInfo,
                        const fims__AudioFormatType* const audioInfo,
                        uint16_t nrAudioTracks,
                        uint16_t nrAudioChannels);

void getClipDurationInFrames(const edlprovider__ClipType* const clip,
                             const fims__BMContentFormatType* const clipFormatInfo,
                             const fims__VideoFormatType* const videoInfo,
                             uint64_t& fileDurationFrames,
                             uint64_t& clipMarkInFrames,
                             uint64_t& clipMarkOutFrames,
                             uint64_t& clipDurationFrames);

}
}

#endif // PLUGINS_INTERFACES_SRC_EDLHELPERS_H
