#ifndef _LOGGER
#define _LOGGER "AAF"
#endif

#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER _LOGGER
#endif

#include "AafPlugin.h"

#include <numeric>

#include <AAF.h>
#include <AAFResult.h>
#include <AAFExtEnum.h>
#include <AAFDataDefs.h>
#include <AAFFileKinds.h>
#include <AAFTypeDefUIDs.h>
#include <AAFClassDefUIDs.h>
#include <AAFContainerDefs.h>
#include <AAFCompressionDefs.h>
#include <AAFStoredObjectIDs.h>

#include <QFileInfo>
#include <QTemporaryFile>

#include "EdlHelpers.h"
#include "EdlException.h"
#include "LoggingHelpers.h"

INITIALIZE_NULL_EASYLOGGINGPP

using namespace plugins::aaf;

#define check(a)  \
{ \
    if (FAILED(a)) \
    { \
        LOG(ERROR) << "Failed on AAF SDK call with error [" << std::to_string(a) << "]"; \
        goto cleanup; \
    } \
}

struct ClipDetails
{
    ClipDetails(const fims__BMContentType* clipInfo,
                const fims__BMContentFormatType* clipFormatInfo,
                const fims__VideoFormatType* videoInfo,
                const fims__AudioFormatType* audioInfo,
                uint16_t nrAudioTracks,
                uint16_t nrAudioChannels,
                uint64_t fileDurationFrames,
                uint64_t clipMarkInFrames,
                uint64_t clipMarkOutFrames,
                uint64_t clipDurationFrames)
        : clipInfo(clipInfo),
          clipFormatInfo(clipFormatInfo),
          videoInfo(videoInfo),
          audioInfo(audioInfo),
          nrAudioTracks(nrAudioTracks),
          nrAudioChannels(nrAudioChannels),
          fileDurationFrames(fileDurationFrames),
          clipMarkInFrames(clipMarkInFrames),
          clipMarkOutFrames(clipMarkOutFrames),
          clipDurationFrames(clipDurationFrames)
    {
    }

    const fims__BMContentType* const clipInfo;
    const fims__BMContentFormatType* const clipFormatInfo;
    const fims__VideoFormatType* const videoInfo;
    const fims__AudioFormatType* const audioInfo;
    const uint16_t nrAudioTracks;
    const uint16_t nrAudioChannels;
    const uint64_t fileDurationFrames;
    const uint64_t clipMarkInFrames;
    const uint64_t clipMarkOutFrames;
    const uint64_t clipDurationFrames;
};

static std::wstring get_track_name(std::wstring prefix, int number)
{
    return prefix.append(std::to_wstring(number));
}

static void computeMaxValues(const std::vector<ClipDetails>& clipDetails,
                             int* maxChannels,
                             int* maxStereos,
                             int* maxMonos)
{
    *maxChannels = 0;
    *maxStereos = 0;
    *maxMonos = 0;

    for (const ClipDetails& clip : clipDetails)
    {
        *maxChannels = std::max((int) clip.nrAudioChannels, *maxChannels);

        if (clip.nrAudioChannels > 1)
            *maxStereos = std::max((int) clip.nrAudioTracks, *maxStereos);
        else if (clip.nrAudioChannels == 1)
            *maxMonos = std::max((int) clip.nrAudioTracks, *maxMonos);
    }
}

AafPlugin::AafPlugin()
{

}

QByteArray AafPlugin::createEdl(const std::wstring* const edlSequenceName,
                                const fims__RationalType* const edlFrameRate,
                                const std::vector<edlprovider__ClipType*>& clips) const
{
    QString sequenceName;
    if (edlSequenceName != NULL)
        sequenceName = QString::fromStdWString(*edlSequenceName);

    VLOG(1) << "SequenceName:[" << sequenceName << "] "
               "Rate:[" << *edlFrameRate << "]";

    std::vector<ClipDetails> clipsProcessed;

    //Process all the clips and store the values in a special struct
    for (const auto* const clip : clips)
    {
        const fims__BMContentType* clipInfo = nullptr;
        const fims__BMContentFormatType* clipFormatInfo = nullptr;
        const fims__VideoFormatType* videoInfo = nullptr;
        const fims__AudioFormatType* audioInfo = nullptr;
        uint16_t nrAudioTracks = 0;
        uint16_t nrAudioChannels = 0;

        interfaces::processClipInfo(clip, &clipInfo, &clipFormatInfo, &videoInfo, &audioInfo, nrAudioTracks, nrAudioChannels);
        interfaces::logClipInformation(clipFormatInfo, clip, videoInfo, audioInfo, nrAudioTracks, nrAudioChannels);

        uint64_t fileDurationFrames = 0;
        uint64_t clipMarkInFrames   = 0;
        uint64_t clipMarkOutFrames  = 0;
        uint64_t clipDurationFrames = 0;

        interfaces::getClipDurationInFrames(clip,
                                            clipFormatInfo,
                                            videoInfo,
                                            fileDurationFrames,
                                            clipMarkInFrames,
                                            clipMarkOutFrames,
                                            clipDurationFrames);

        clipsProcessed.emplace_back(clipInfo,
                                    clipFormatInfo,
                                    videoInfo,
                                    audioInfo,
                                    nrAudioTracks,
                                    nrAudioChannels,
                                    fileDurationFrames,
                                    clipMarkInFrames,
                                    clipMarkOutFrames,
                                    clipDurationFrames);
    }

    //The code below is an adaptation from CreateSequence example in AAF SDK

    const aafUID_t NIL_UID = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }};

    int maxChannels = 0;
    int maxStereos = 0;
    int maxMonos = 0;
    computeMaxValues(clipsProcessed, &maxChannels, &maxStereos, &maxMonos);

    //Max slots is mainly used to know how much audio sequences need to be done
    int maxSlots = (maxChannels * maxStereos) + maxMonos;

    IAAFDigitalImageDescriptor2*    digitalImageDesc = NULL;
    IAAFCDCIDescriptor2*            cDCIDescripor = NULL;
    IAAFSoundDescriptor*            soundDesc = NULL;

    IAAFCompositionMob*             collectionCompositionMob = NULL;

    IAAFSequence*                   videoSequence = NULL;
    IAAFSequence**                  audioSequences = new IAAFSequence*[maxSlots];

    IAAFMobSlot*                    mobSlot = NULL;
    IAAFTimelineMobSlot2*           timelineMobSlot2 = NULL;
    IAAFFile*                       file = NULL;
    IAAFHeader*                     header = NULL;
    IAAFDictionary*                 dictionary = NULL;
    IAAFMob*                        mob = NULL;
    IAAFMob*                        compMob = NULL;
    IAAFMob2*                       compMob2 = NULL;
    IAAFEssenceDescriptor*          essenceDesc = NULL;
    IAAFMasterMob*                  masterMob = NULL;
    IAAFSourceMob*                  fileMob = NULL;
    IAAFSourceMob*                  tapeMob = NULL;
    IAAFComponent*                  component = NULL;
    IAAFFileDescriptor*             fileDesc = NULL;
    IAAFImportDescriptor*           tapeDesc = NULL;
    IAAFTimelineMobSlot*            newSlot = NULL;
    IAAFSegment*                    segment = NULL;
    IAAFSourceClip*                 compSclp = NULL;
    IAAFComponent*                  compFill = NULL;
    IAAFLocator*                    locator = NULL;
    IAAFNetworkLocator*             netLocator = NULL;
    IAAFClassDef*                   cdCompositionMob = 0;
    IAAFClassDef*                   cdSequence = 0;
    IAAFClassDef*                   cdSourceMob = 0;
    IAAFClassDef*                   cdTapeDescriptor = 0;
    IAAFClassDef*                   cdDigitalImageDescriptor = 0;
    IAAFClassDef*                   cdSoundDescriptor = 0;
    IAAFClassDef*                   cdNetworkLocator = 0;
    IAAFClassDef*                   cdMasterMob = 0;
    IAAFClassDef*                   cdSourceClip = 0;
    IAAFClassDef*                   cdFiller = 0;
    IAAFClassDef*                   cdTimecode = 0;
    IAAFDataDef*                    dDefPicture = 0;
    IAAFDataDef*                    soundDef = 0;
    IAAFDataDef*                    timecodeDef = 0;
    aafMobID_t                      tapeMobID, fileMobID, masterMobID;
    aafTimecode_t                   tapeTC;
    aafLength_t                     fileLen = 0;
    aafLength_t                     segLen = 0;
    aafLength_t                     videoFileLen = 0;
    aafLength_t                     totalLength = 0;
    aafProductIdentification_t      productInfo;
    aafSourceRef_t                  sourceRef;
    aafRational_t                   edlVideoRate;

    aafProductVersion_t version;
    version.major = 0;
    version.minor = 0;
    version.tertiary = 0;
    version.patchLevel = 1;
    version.type = kAAFVersionBeta;
    productInfo.companyName = L"EDLProvider";
    productInfo.productName = L"AAFPlugin";
    productInfo.productVersion = &version;
    productInfo.productVersionString = NULL;
    productInfo.productID = NIL_UID;
    productInfo.platform = NULL;

    //Initialize the AAF file, the contents are stored in a temporary file
    QTemporaryFile tempFile;
    if (!tempFile.open())
    {
        LOG(ERROR) << "Temporary file to write the aaf could not be open.";
        goto cleanup;
    }

    check(AAFFileOpenNewModifyEx(tempFile.fileName().toStdWString().c_str(), &kAAFFileKind_Aaf4KBinary, 0, &productInfo, &file));
    check(file->GetHeader(&header));

    // Get the AAF Dictionary so that we can create valid AAF objects.
    check(header->GetDictionary(&dictionary));

    check(dictionary->LookupClassDef(AUID_AAFCompositionMob, &cdCompositionMob));
    check(dictionary->LookupClassDef(AUID_AAFSequence, &cdSequence));
    check(dictionary->LookupClassDef(AUID_AAFSourceMob, &cdSourceMob));
    check(dictionary->LookupClassDef(AUID_AAFImportDescriptor, &cdTapeDescriptor));
    check(dictionary->LookupClassDef(AUID_AAFCDCIDescriptor, &cdDigitalImageDescriptor));
    check(dictionary->LookupClassDef(AUID_AAFSoundDescriptor, &cdSoundDescriptor));
    check(dictionary->LookupClassDef(AUID_AAFNetworkLocator, &cdNetworkLocator));
    check(dictionary->LookupClassDef(AUID_AAFMasterMob, &cdMasterMob));
    check(dictionary->LookupClassDef(AUID_AAFSourceClip, &cdSourceClip));
    check(dictionary->LookupClassDef(AUID_AAFFiller, &cdFiller));
    check(dictionary->LookupDataDef(kAAFDataDef_Picture, &dDefPicture));
    check(dictionary->LookupDataDef(kAAFDataDef_Sound, &soundDef));
    check(dictionary->LookupDataDef(kAAFDataDef_Timecode, &timecodeDef));
    check(dictionary->LookupClassDef(AUID_AAFTimecode, &cdTimecode));

    // sequence creation code pulled out of the subsequent loop.
    // Create a Composition Mob
    check(cdCompositionMob->CreateInstance(IID_IAAFCompositionMob, (IUnknown**) &collectionCompositionMob));

    check(collectionCompositionMob->QueryInterface(IID_IAAFMob, (void**) &compMob));

    check(cdSequence->CreateInstance(IID_IAAFSequence, (IUnknown**) &videoSequence));

    check(videoSequence->QueryInterface (IID_IAAFSegment, (void**) &segment));

    check(videoSequence->QueryInterface(IID_IAAFComponent, (void**) &component));

    check(component->SetDataDef(dDefPicture));
    component->Release();
    component = NULL;

    check(compMob->QueryInterface(IID_IAAFMob2, (void**) &compMob2));
    compMob2->SetUsageCode(kAAFUsage_TopLevel);

    check(compMob->QueryInterface(IID_IAAFMob, (void**) &mob));

    //Add a Sequence name
    check(mob->SetName(sequenceName.toStdWString().c_str()));

    bool isDrop;
    processFrameRate(edlFrameRate, "Sequence frame rate not supported.", isDrop, edlVideoRate);
    check(mob->AppendNewTimelineSlot(edlVideoRate, segment, 1, L"VideoSlot", 0, &newSlot));

    //Set PhysicalTrackNumber
    check(compMob->LookupSlot(1, &mobSlot));
    check(mobSlot->SetPhysicalNum(1));

    mob->Release();
    mob = NULL;
    newSlot->Release();
    newSlot = NULL;
    segment->Release();
    segment = NULL;
    mobSlot->Release ();
    mobSlot = NULL;

    int audioTimelineSlotPos = 0;
    //AAF will have at least one element in vectorOFClips
    for (int s = 0; s < maxSlots; s++)
    {
        check(cdSequence->CreateInstance(IID_IAAFSequence, (IUnknown**) &audioSequences[audioTimelineSlotPos]));
        check(audioSequences[audioTimelineSlotPos]->QueryInterface (IID_IAAFSegment, (void**) &segment));
        check(audioSequences[audioTimelineSlotPos]->QueryInterface(IID_IAAFComponent, (void**) &component));
        check(component->SetDataDef(soundDef));
        component->Release();
        component = NULL;

        //The slot names will be copied from last item on the vector if files have different audio layouts (eg. 2 mono and 4 mono)
        check (compMob->AppendNewTimelineSlot(edlVideoRate,
                                              segment,
                                              audioTimelineSlotPos + 2,
                                              QString("AudioSlot%1").arg(audioTimelineSlotPos).toStdWString().c_str(),
                                              0,
                                              &newSlot));

        //Set PhysicalTrackNumber
        check(compMob->LookupSlot(audioTimelineSlotPos + 2, &mobSlot));
        check(mobSlot->SetPhysicalNum(audioTimelineSlotPos + 1));

        newSlot->Release();
        newSlot = NULL;
        segment->Release();
        segment = NULL;
        mobSlot->Release ();
        mobSlot = NULL;

        audioTimelineSlotPos++;
    }

    IAAFTimecode* timecode = NULL;
    check(cdTimecode->CreateInstance(IID_IAAFTimecode, (IUnknown**) &timecode));
    check(timecode->QueryInterface(IID_IAAFSegment, (void**) &segment));
    check(timecode->QueryInterface(IID_IAAFComponent, (void**) &component));

    check(component->SetDataDef(timecodeDef));
    component->Release();
    component = NULL;

    //Assume atleast 1 element exist
    tapeTC.drop = isDrop ? kAAFTcDrop : kAAFTcNonDrop;
    tapeTC.fps = edlFrameRate->__item;
    tapeTC.startFrame = 0;

    totalLength = std::accumulate(clipsProcessed.cbegin(), clipsProcessed.cend(), 0,
                                  [](decltype(totalLength) total, const ClipDetails& clip)
    {
        return total + clip.clipDurationFrames;
    });

    check(timecode->Initialize(totalLength, &tapeTC));

    check(compMob->AppendNewTimelineSlot(edlVideoRate, segment, audioTimelineSlotPos + 2, L"timecode", 0, &newSlot));

    //Set PhysicalTrackNumber
    check(compMob->LookupSlot(audioTimelineSlotPos + 2, &mobSlot));
    check(mobSlot->SetPhysicalNum(1));

    newSlot->Release();
    newSlot = NULL;
    segment->Release();
    segment = NULL;
    mobSlot->Release();
    mobSlot = NULL;

    check(header->AddMob(compMob));

    // now looping around the remainder N times to make N components
    for (const ClipDetails& clip : clipsProcessed)
    {
        segLen = clip.clipDurationFrames;
        fileLen = clip.fileDurationFrames;
        videoFileLen = clip.fileDurationFrames;
        QFileInfo fInfo(QString::fromStdWString(*clip.clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location));

        //Make the Tape MOB
        check(cdSourceMob->CreateInstance(IID_IAAFSourceMob, (IUnknown**) &tapeMob));
        check(cdTapeDescriptor->CreateInstance(IID_IAAFImportDescriptor, (IUnknown**) &tapeDesc));
        check(tapeMob->QueryInterface(IID_IAAFMob, (void**) &mob));
        check(tapeDesc->QueryInterface(IID_IAAFEssenceDescriptor, (void**) &essenceDesc));

        // Make a locator, and attach it to the EssenceDescriptor
        check(cdNetworkLocator->CreateInstance(IID_IAAFNetworkLocator, (IUnknown**) &netLocator));
        check(netLocator->QueryInterface(IID_IAAFLocator, (void**) &locator));

        check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
        check(essenceDesc->AppendLocator(locator));
        locator->Release();
        locator = NULL;
        netLocator->Release();
        netLocator = NULL;

        check(tapeMob->SetEssenceDescriptor(essenceDesc));
        essenceDesc->Release();
        essenceDesc = NULL;
        tapeDesc->Release();
        tapeDesc = NULL;

        aafLength_t componentTotalLength = fileLen;
        check(tapeMob->AddNilReference(1, componentTotalLength, dDefPicture, edlVideoRate));

        //Set PhysicalTrackNumber
        check(mob->LookupSlot(1, &mobSlot));
        check(mobSlot->SetPhysicalNum(1));
        check(mobSlot->SetName(L"V1"));

        mobSlot->Release();
        mobSlot = NULL;

        audioTimelineSlotPos = 0;
        for (int s = 0; s < clip.nrAudioTracks; s++)
        {
            check(tapeMob->AddNilReference(audioTimelineSlotPos + 2, componentTotalLength, soundDef, edlVideoRate));

            //Set PhysicalTrackNumber
            check(mob->LookupSlot(audioTimelineSlotPos + 2, &mobSlot));
            check(mobSlot->SetPhysicalNum(audioTimelineSlotPos + 1));
            check(mobSlot->SetName(get_track_name(L"A", audioTimelineSlotPos + 1).c_str()));

            mobSlot->Release();
            mobSlot = NULL;
            audioTimelineSlotPos++;
        }

        check(tapeMob->AppendTimecodeSlot(edlVideoRate,
                                          audioTimelineSlotPos + 2,
                                          tapeTC,
                                          componentTotalLength));

        //Set PhysicalTrackNumber
        check(mob->LookupSlot(audioTimelineSlotPos + 2, &mobSlot));
        check(mobSlot->SetPhysicalNum(1));

        mob->Release();
        mob = NULL;
        mobSlot->Release();
        mobSlot = NULL;

        check(tapeMob->QueryInterface(IID_IAAFMob, (void**) &mob));
        tapeMob->Release();
        tapeMob = NULL;

        //Set tape Mob name
        check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));
        check(header->AddMob(mob));
        check(mob->GetMobID(&tapeMobID));

        mob->Release();
        mob = NULL;

        // Make a FileMob
        check(cdSourceMob->CreateInstance(IID_IAAFSourceMob, (IUnknown**) &fileMob));
        check(cdDigitalImageDescriptor->CreateInstance(IID_IAAFFileDescriptor, (IUnknown**) &fileDesc));
        check(fileDesc->QueryInterface(IID_IAAFEssenceDescriptor, (void**) &essenceDesc));

        //Request the fileMob Image Description
        check(fileDesc->QueryInterface(IID_IAAFDigitalImageDescriptor2, (void**) &digitalImageDesc));
        check(fileDesc->SetLength(videoFileLen));
        check(fileDesc->SetSampleRate(edlVideoRate));

        int displayHeight = std::stoi(clip.videoInfo->displayHeight->__item);
        int displayWidth = std::stoi(clip.videoInfo->displayWidth->__item);

        check(digitalImageDesc->SetDisplayView(displayHeight, displayWidth, 0, 0));
        check(digitalImageDesc->SetSampledView(displayHeight, displayWidth, 0, 0));
        check(digitalImageDesc->SetStoredView(displayHeight, displayWidth));

        aafFrameLayout_t layout = (*clip.videoInfo->scanningFormat) == fims__ScanningFormatType__interlaced ? kAAFSeparateFields : kAAFFullFrame;
        check(digitalImageDesc->SetFrameLayout(layout));

        if (layout == kAAFFullFrame)
        {
            aafInt32 lineMap[] = { 1 };
            check(digitalImageDesc->SetVideoLineMap(1, lineMap));
        }
        else if ((*clip.videoInfo->scanningOrder) == fims__ScanningOrderType__top)
        {
            aafInt32 lineMap[] = {1, 0};
            check(digitalImageDesc->SetVideoLineMap(2, lineMap));
        }
        else
        {
            aafInt32 lineMap[] = {0, 1};
            check(digitalImageDesc->SetVideoLineMap(2, lineMap));
        }

        aafRational_t aspectRatio = { std::stoi(clip.videoInfo->aspectRatio->numerator),
                                      std::stoi(clip.videoInfo->aspectRatio->denominator) };
        check(digitalImageDesc->SetImageAspectRatio(aspectRatio));

        check(essenceDesc->QueryInterface(IID_IAAFCDCIDescriptor2, (void**) &cDCIDescripor));

        digitalImageDesc->Release();
        digitalImageDesc = NULL;

        cDCIDescripor->Release();
        cDCIDescripor = NULL;

        // Make a locator, and attach it to the EssenceDescriptor
        check(cdNetworkLocator->CreateInstance(IID_IAAFNetworkLocator, (IUnknown**) &netLocator));
        check(netLocator->QueryInterface(IID_IAAFLocator, (void**) &locator));

        check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
        check(essenceDesc->AppendLocator(locator));
        locator->Release();
        locator = NULL;
        netLocator->Release();
        netLocator = NULL;

        check(fileMob->SetEssenceDescriptor(essenceDesc));
        essenceDesc->Release();
        essenceDesc = NULL;
        fileDesc->Release();
        fileDesc = NULL;

        sourceRef.sourceID = tapeMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = 0;
        check(fileMob->NewPhysSourceRef(edlVideoRate, 1, dDefPicture, sourceRef, fileLen));

        check(fileMob->QueryInterface(IID_IAAFMob, (void**) &mob));
        check(mob->SetName(L"Video file"));

        check(mob->GetMobID(&fileMobID));
        check(header->AddMob(mob));
        check(mob->LookupSlot(1, &mobSlot));
        check(mobSlot->SetName(L"V1"));
        check(mobSlot->SetPhysicalNum(1));

        mobSlot->Release();
        mobSlot = NULL;
        mob->Release();
        mob = NULL;

        //Make the Master MOB
        check(cdMasterMob->CreateInstance(IID_IAAFMasterMob, (IUnknown**) &masterMob));

        sourceRef.sourceID = fileMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = 0;
        check(masterMob->NewPhysSourceRef(edlVideoRate, 1, dDefPicture, sourceRef, fileLen));

        check(masterMob->QueryInterface(IID_IAAFMob, (void**) &mob));

        check(mob->GetMobID(&masterMobID));

        //Set MarkIn and MarkOut in masterMob
        check(mob->LookupSlot(1, &mobSlot));
        check(mobSlot->SetName(L"V1"));
        check(mobSlot->SetPhysicalNum(1));
        check(mobSlot->QueryInterface(IID_IAAFTimelineMobSlot2, (void**) &timelineMobSlot2));
        check(timelineMobSlot2->SetMarkIn(clip.clipMarkInFrames));
        check(timelineMobSlot2->SetMarkOut(clip.clipMarkOutFrames));

        timelineMobSlot2->Release();
        timelineMobSlot2 = NULL;
        mobSlot->Release();
        mobSlot = NULL;

        //Set masterMobName
        check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));

        check(header->AddMob(mob));
        mob->Release();
        mob = NULL;

        // Create a SourceClip
        check(cdSourceClip->CreateInstance(IID_IAAFSourceClip, (IUnknown**) &compSclp));

        sourceRef.sourceID = masterMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = clip.clipMarkInFrames;
        check(compSclp->SetSourceReference(sourceRef));
        check(compSclp->QueryInterface(IID_IAAFComponent, (void**) &component));
        check(component->SetDataDef(dDefPicture));
        check(component->SetLength(segLen));
        check(videoSequence->AppendComponent(component));

        component->Release();
        component = NULL;

        compSclp->Release();
        compSclp = NULL;

        fileMob->Release();
        fileMob = NULL;

        int audioTrackNumber = 0;
        int audioTimelineSlotPos = 0;

        //Cicle between all the audio files that will be added
        for (int z = 0; z < clip.nrAudioChannels; z++)
        {
            if (clip.nrAudioChannels == 1 && maxStereos != 0)
            {
                for (; audioTimelineSlotPos < (maxStereos * 2); audioTimelineSlotPos++)
                {
                    check(cdFiller->CreateInstance(IID_IAAFComponent, (IUnknown**) &compFill));

                    check(compFill->SetLength(segLen));

                    check(compFill->SetDataDef(soundDef));
                    check(audioSequences[audioTimelineSlotPos]->AppendComponent(compFill));

                    compFill->Release();
                    compFill = NULL;
                }
            }

            // Make a FileMob
            check(cdSourceMob->CreateInstance(IID_IAAFSourceMob, (IUnknown**) &fileMob));

            check(cdSoundDescriptor->CreateInstance(IID_IAAFFileDescriptor, (IUnknown**) &fileDesc));

            check(fileDesc->QueryInterface(IID_IAAFEssenceDescriptor, (void**) &essenceDesc));

            //Request the fileMob Audio Description
            check(fileDesc->QueryInterface (IID_IAAFSoundDescriptor, (void**) &soundDesc));


            auto audioSamplingRateValue = std::stoul(*clip.audioInfo->samplingRate);
            aafRational_t audioSamplingRate = { audioSamplingRateValue, 1 };
            aafLength_t audioLength = (audioSamplingRateValue * fileLen * edlVideoRate.denominator) / edlVideoRate.numerator;

            check(fileDesc->SetLength(audioLength));
            check(fileDesc->SetSampleRate(audioSamplingRate));

            check(soundDesc->SetAudioSamplingRate(audioSamplingRate));
            check(soundDesc->SetChannelCount(clip.nrAudioChannels));
            check(soundDesc->SetQuantizationBits(std::stoul(*clip.audioInfo->sampleSize)));

            soundDesc->Release();
            soundDesc = NULL;

            // Make a locator, and attach it to the EssenceDescriptor
            check(cdNetworkLocator->CreateInstance(IID_IAAFNetworkLocator, (IUnknown**) &netLocator));
            check(netLocator->QueryInterface(IID_IAAFLocator, (void**) &locator));

            check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
            check(essenceDesc->AppendLocator(locator));
            locator->Release();
            locator = NULL;
            netLocator->Release();
            netLocator = NULL;

            check(fileMob->SetEssenceDescriptor(essenceDesc));
            essenceDesc->Release();
            essenceDesc = NULL;
            fileDesc->Release();
            fileDesc = NULL;

            for (int s = 0; s < clip.nrAudioTracks; s++)
            {
                sourceRef.sourceID = tapeMobID;
                sourceRef.sourceSlotID = s + 2;
                sourceRef.startTime = 0;

                check(fileMob->NewPhysSourceRef(edlVideoRate, s + 1, soundDef, sourceRef, fileLen));

                check(fileMob->QueryInterface(IID_IAAFMob, (void**) &mob));
                check(mob->SetName(L"Audio file"));

                check(mob->LookupSlot(s + 1, &mobSlot));
                check(mobSlot->SetName(get_track_name(L"A", z + s + 1).c_str()));
                check(mobSlot->SetPhysicalNum(audioTrackNumber + 1));
                audioTrackNumber++;

                check(mob->GetMobID(&fileMobID));
                check(header->AddMob(mob));

                mob->Release();
                mob = NULL;
                mobSlot->Release();
                mobSlot = NULL;
            }

            sourceRef.sourceID = fileMobID;
            sourceRef.sourceSlotID = 1;
            sourceRef.startTime = 0;
            check(masterMob->AppendPhysSourceRef(edlVideoRate, z + 2, soundDef, sourceRef, fileLen));
            check(masterMob->QueryInterface(IID_IAAFMob, (void**) &mob));
            check(mob->GetMobID(&masterMobID));

            //Set masterMobName
            check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));

            check(mob->LookupSlot(z + 2, &mobSlot));
            check(mobSlot->SetName(get_track_name(L"A", z + 1).c_str()));
            check(mobSlot->SetPhysicalNum(z + 1));
            check(mobSlot->QueryInterface(IID_IAAFTimelineMobSlot2, (void**) &timelineMobSlot2));
            check(timelineMobSlot2->SetMarkIn(clip.clipMarkInFrames));
            check(timelineMobSlot2->SetMarkOut(clip.clipMarkOutFrames));

            timelineMobSlot2->Release();
            timelineMobSlot2 = NULL;

            mobSlot->Release();
            mobSlot = NULL;
            mob->Release();
            mob = NULL;

            // the remaining part of the sequence code, adapted for updating slot names
            for (int s = 0; s < clip.nrAudioTracks; s++)
            {
                // Create a SourceClip
                check(cdSourceClip->CreateInstance(IID_IAAFSourceClip, (IUnknown**) &compSclp));

                sourceRef.sourceID = masterMobID;
                sourceRef.sourceSlotID = z + 2;
                sourceRef.startTime = clip.clipMarkInFrames;
                check(compSclp->SetSourceReference(sourceRef));
                check(compSclp->QueryInterface(IID_IAAFComponent, (void**) &component));
                check(component->SetDataDef(soundDef));
                check(component->SetLength(segLen));
                check(audioSequences[audioTimelineSlotPos]->AppendComponent(component));

                component->Release();
                component = NULL;
                compSclp->Release();
                compSclp = NULL;

                audioTimelineSlotPos++;
            }

            fileMob->Release();
            fileMob = NULL;
        }

        masterMob->Release();
        masterMob = NULL;

        for (; audioTimelineSlotPos < maxSlots; audioTimelineSlotPos++)
        {
            // Create a filler - Get the component interface only (IID_IAAFComponent)
            check(cdFiller->CreateInstance(IID_IAAFComponent, (IUnknown**) &compFill));
            check(compFill->SetLength(segLen));

            check(compFill->SetDataDef(soundDef));
            check(audioSequences[audioTimelineSlotPos]->AppendComponent(compFill));

            compFill->Release();
            compFill = NULL;
        }

        //  end of loop since only one dictionary and header are needed
        //  the file is then saved, closed and released after all modifications
        //  are complete
    }

cleanup:
    // Cleanup and return
    if (netLocator)
        netLocator->Release();

    if (locator)
        locator->Release();

    if (compFill)
        compFill->Release();

    if (compSclp)
        compSclp->Release();

    if (tapeDesc)
        tapeDesc->Release();

    if (fileDesc)
        fileDesc->Release();

    if (tapeMob)
        tapeMob->Release();

    if (fileMob)
        fileMob->Release();

    if (masterMob)
        masterMob->Release();

    if (essenceDesc)
        essenceDesc->Release();

    if (newSlot)
        newSlot->Release();

    if (mob)
        mob->Release();

    if (compMob)
        compMob->Release();

    if (compMob2)
        compMob2->Release();

    if (collectionCompositionMob)
        collectionCompositionMob->Release();

    if (mobSlot)
        mobSlot->Release();

    if (timelineMobSlot2)
        timelineMobSlot2->Release();

    if (segment)
        segment->Release();

    if (component)
        component->Release();

    if (videoSequence)
        videoSequence->Release();

    for (int l = 0; l < maxSlots; l++)
    {
        if (audioSequences[l])
            audioSequences[l]->Release ();
    }

    if (audioSequences)
        delete [] audioSequences;

    if (dictionary)
        dictionary->Release();

    if (header)
        header->Release();

    if (cdCompositionMob)
    {
        cdCompositionMob->Release();
        cdCompositionMob = 0;
    }

    if (cdSequence)
    {
        cdSequence->Release();
        cdSequence = 0;
    }

    if (cdSourceMob)
    {
        cdSourceMob->Release();
        cdSourceMob = 0;
    }

    if (cdTapeDescriptor)
    {
        cdTapeDescriptor->Release();
        cdTapeDescriptor = 0;
    }

    if (cdDigitalImageDescriptor)
    {
        cdDigitalImageDescriptor->Release();
        cdDigitalImageDescriptor = 0;
    }

    if (cdSoundDescriptor)
    {
        cdSoundDescriptor->Release();
        cdSoundDescriptor = 0;
    }

    if (soundDef)
    {
        soundDef->Release();
        soundDef = 0;
    }

    if (cdNetworkLocator)
    {
        cdNetworkLocator->Release();
        cdNetworkLocator = 0;
    }

    if (cdMasterMob)
    {
        cdMasterMob->Release();
        cdMasterMob = 0;
    }

    if (cdSourceClip)
    {
        cdSourceClip->Release();
        cdSourceClip = 0;
    }

    if (cdFiller)
    {
        cdFiller->Release();
        cdFiller = 0;
    }

    if (cdTimecode)
    {
        cdTimecode->Release();
        cdTimecode = 0;
    }

    if (dDefPicture)
    {
        dDefPicture->Release();
        dDefPicture = 0;
    }

    if (timecodeDef)
    {
        timecodeDef->Release();
        timecodeDef = 0;
    }

    if (file)
    {
        file->Save();
        file->Close();
        file->Release();
    }

    return tempFile.readAll();
}

void AafPlugin::setEasyloggingStorage(el::base::type::StoragePointer storage)
{
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Register logger to use in the plugin
    el::Loggers::getLogger(_LOGGER);
}

void AafPlugin::processFrameRate(const fims__RationalType* const frameRate,
                                 const std::string& message,
                                 bool& isDrop,
                                 aafRational_t& editRate) const
{
    bool result = false;

    if (frameRate->__item == 24)
    {
        result = true;
        editRate.numerator = 24 * 1000;
        editRate.denominator = 24 * 1001;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 25)
    {
        result = true;
        editRate.numerator = 25;
        editRate.denominator = 1;
        isDrop = false;
    }
    else if (frameRate->__item == 30)
    {
        result = true;
        editRate.numerator = 30 * 1000;
        editRate.denominator = 30 * 1001;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }
    else if (frameRate->__item == 50)
    {
        result = true;
        editRate.numerator = 50;
        editRate.denominator = 1;
        isDrop = false;
    }
    else if (frameRate->__item == 60)
    {
        result = true;
        editRate.numerator = 60 * 1000;
        editRate.denominator = 60 * 1001;
        if (frameRate->numerator == L"1" && frameRate->denominator == L"1")
            isDrop = false;
        else if (frameRate->numerator == L"1000" && frameRate->denominator == L"1001")
            isDrop = true;
    }

    VLOG_IF(result, 1) << "Input frame rate:[" << *frameRate << "]" << "out ntsc:["
            << std::to_string(isDrop) << "]"
            << "out time base:[" << std::to_string(editRate.numerator) << "/" << std::to_string(editRate.denominator) << "].";

    if (!result)
        throw interfaces::EdlException(interfaces::EdlException::EdlError::UNSUPPORTED_FRAME_RATE, message);
}
