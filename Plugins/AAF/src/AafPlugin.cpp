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
        throw interfaces::EdlException(interfaces::EdlException::EdlError::GENERIC, \
                                       "Failed on AAF SDK call with error [" + std::to_string(a) + "]" ); \
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

template<typename T>
using UPCustomDel = std::unique_ptr<T, std::function<void(T*)>>;

template<typename T>
UPCustomDel<T> checkResult(HRESULT result, T* const typeExtracted, const std::string& funcName)
{
    if (FAILED(result))
    {
        LOG(ERROR) << "Failed on AAF SDK " << funcName << " call with error [" << std::to_string(result) << "]";
        throw plugins::interfaces::EdlException(plugins::interfaces::EdlException::EdlError::GENERIC,
                                                "Failed on AAF SDK " + funcName + " call with error [" + std::to_string(result) + "]");
    }

    return UPCustomDel<T>(typeExtracted, [](T* val) { val->Release(); });
}

template<typename T>
inline UPCustomDel<T> queryInterfaceHelper(IUnknown* const uInterface, IID guid)
{
    T* interfaceQueried = NULL;
    auto result = uInterface->QueryInterface(guid, (void**) &interfaceQueried);
    return checkResult(result, interfaceQueried, "QueryInterface");
}

template<typename T>
inline UPCustomDel<T> createInstanceHelper(IAAFClassDef* const classDef, IID guid)
{
    T* instanceCreated = NULL;
    auto result = classDef->CreateInstance(guid, (IUnknown**) &instanceCreated);
    return checkResult(result, instanceCreated, "CreateInstance");
}

inline UPCustomDel<IAAFClassDef> lookupClassHelper(IAAFDictionary* const dictionary, _aafUID_t uid)
{
    IAAFClassDef* classDefExtracted = NULL;
    auto result = dictionary->LookupClassDef(uid, &classDefExtracted);
    return checkResult(result, classDefExtracted, "LookupClassDef");
}

inline UPCustomDel<IAAFDataDef> lookupDataDefHelper(IAAFDictionary* const dictionary, _aafUID_t uid)
{
    IAAFDataDef* dataDefFound = NULL;
    auto result = dictionary->LookupDataDef(uid, &dataDefFound);
    return checkResult(result, dataDefFound, "LookupDataDef");
}

template<typename T>
inline UPCustomDel<IAAFMobSlot> lookupSlotHelper(T* const mob, aafSlotID_t id)
{
    IAAFMobSlot* slotFound = NULL;
    auto result = mob->LookupSlot(id, &slotFound);
    return checkResult(result, slotFound, "LookupSlot");
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

    IAAFMob*                        mob = NULL;

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
        throw interfaces::EdlException(interfaces::EdlException::EdlError::GENERIC,
                                       "Failed generating temporary file on disk.");
    }

    IAAFFile* filePtr = NULL;
    check(AAFFileOpenNewModifyEx(tempFile.fileName().toStdWString().c_str(), &kAAFFileKind_Aaf4KBinary, 0, &productInfo, &filePtr));
    auto file = UPCustomDel<IAAFFile>(filePtr, [](IAAFFile* val) { val->Release(); });

    IAAFHeader* headerPtr = NULL;
    check(file->GetHeader(&headerPtr));
    auto header = UPCustomDel<IAAFHeader>(headerPtr, [](IAAFHeader* val) { val->Release(); });

    // Get the AAF Dictionary so that we can create valid AAF objects.
    IAAFDictionary* dictionaryPtr = NULL;
    check(header->GetDictionary(&dictionaryPtr));
    auto dictionary = UPCustomDel<IAAFDictionary>(dictionaryPtr, [](IAAFDictionary* val) { val->Release(); });

    auto cdCompositionMob = lookupClassHelper(dictionary.get(), AUID_AAFCompositionMob);
    auto cdSequence = lookupClassHelper(dictionary.get(), AUID_AAFSequence);
    auto cdSourceMob = lookupClassHelper(dictionary.get(), AUID_AAFSourceMob);
    auto cdTapeDescriptor = lookupClassHelper(dictionary.get(), AUID_AAFImportDescriptor);
    auto cdDigitalImageDescriptor = lookupClassHelper(dictionary.get(), AUID_AAFCDCIDescriptor);
    auto cdSoundDescriptor = lookupClassHelper(dictionary.get(), AUID_AAFSoundDescriptor);
    auto cdNetworkLocator = lookupClassHelper(dictionary.get(), AUID_AAFNetworkLocator);
    auto cdMasterMob = lookupClassHelper(dictionary.get(), AUID_AAFMasterMob);
    auto cdSourceClip = lookupClassHelper(dictionary.get(), AUID_AAFSourceClip);
    auto cdFiller = lookupClassHelper(dictionary.get(), AUID_AAFFiller);
    auto cdTimecode = lookupClassHelper(dictionary.get(), AUID_AAFTimecode);
    auto dDefPicture = lookupDataDefHelper(dictionary.get(), kAAFDataDef_Picture);
    auto soundDef = lookupDataDefHelper(dictionary.get(), kAAFDataDef_Sound);
    auto timecodeDef = lookupDataDefHelper(dictionary.get(), kAAFDataDef_Timecode);

    // sequence creation code pulled out of the subsequent loop.
    // Create a Composition Mob
    auto collectionCompositionMob = createInstanceHelper<IAAFCompositionMob>(cdCompositionMob.get(), IID_IAAFCompositionMob);

    auto compMob = queryInterfaceHelper<IAAFMob>(collectionCompositionMob.get(), IID_IAAFMob);

    auto videoSequence = createInstanceHelper<IAAFSequence>(cdSequence.get(), IID_IAAFSequence);

    auto segment = queryInterfaceHelper<IAAFSegment>(videoSequence.get(), IID_IAAFSegment);

    auto component = queryInterfaceHelper<IAAFComponent>(videoSequence.get(), IID_IAAFComponent);
    check(component->SetDataDef(dDefPicture.get()));

    auto compMob2 = queryInterfaceHelper<IAAFMob2>(compMob.get(), IID_IAAFMob2);
    compMob2->SetUsageCode(kAAFUsage_TopLevel);

    check(compMob->QueryInterface(IID_IAAFMob, (void**) &mob));

    //Add a Sequence name
    check(mob->SetName(sequenceName.toStdWString().c_str()));

    bool isDrop;
    processFrameRate(edlFrameRate, "Sequence frame rate not supported.", isDrop, edlVideoRate);
    IAAFTimelineMobSlot* newSlot;
    check(mob->AppendNewTimelineSlot(edlVideoRate, segment.get(), 1, L"VideoSlot", 0, &newSlot));
    newSlot->Release();
    newSlot = NULL;

    //Set PhysicalTrackNumber
    auto mobSlot = lookupSlotHelper(compMob.get(), 1);
    check(mobSlot->SetPhysicalNum(1));

    mob->Release();
    mob = NULL;

    std::vector<UPCustomDel<IAAFSequence> > audioSequences;
    for (int x = 0; x < maxSlots; x++)
        audioSequences.push_back(createInstanceHelper<IAAFSequence>(cdSequence.get(), IID_IAAFSequence));

    int audioTimelineSlotPos = 0;
    //AAF will have at least one element in vectorOFClips
    for (int s = 0; s < maxSlots; s++)
    {
        segment.reset(queryInterfaceHelper<IAAFSegment>(audioSequences[audioTimelineSlotPos].get(), IID_IAAFSegment).release());
        component.reset(queryInterfaceHelper<IAAFComponent>(audioSequences[audioTimelineSlotPos].get(), IID_IAAFComponent).release());
        check(component->SetDataDef(soundDef.get()));

        //The slot names will be copied from last item on the vector if files have different audio layouts (eg. 2 mono and 4 mono)
        check (compMob->AppendNewTimelineSlot(edlVideoRate,
                                              segment.get(),
                                              audioTimelineSlotPos + 2,
                                              QString("AudioSlot%1").arg(audioTimelineSlotPos).toStdWString().c_str(),
                                              0,
                                              &newSlot));

        newSlot->Release();
        newSlot = NULL;

        //Set PhysicalTrackNumber
        mobSlot.reset(lookupSlotHelper(compMob.get(), audioTimelineSlotPos + 2).release());
        check(mobSlot->SetPhysicalNum(audioTimelineSlotPos + 1));

        audioTimelineSlotPos++;
    }

    auto timecode = createInstanceHelper<IAAFTimecode>(cdTimecode.get(), IID_IAAFTimecode);
    segment.reset(queryInterfaceHelper<IAAFSegment>(timecode.get(), IID_IAAFSegment).release());
    component.reset(queryInterfaceHelper<IAAFComponent>(timecode.get(), IID_IAAFComponent).release());

    check(component->SetDataDef(timecodeDef.get()));

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

    check(compMob->AppendNewTimelineSlot(edlVideoRate, segment.get(), audioTimelineSlotPos + 2, L"timecode", 0, &newSlot));

    newSlot->Release();
    newSlot = NULL;

    //Set PhysicalTrackNumber
    mobSlot.reset(lookupSlotHelper(compMob.get(), audioTimelineSlotPos + 2).release());
    check(mobSlot->SetPhysicalNum(1));

    check(header->AddMob(compMob.get()));

    // now looping around the remainder N times to make N components
    for (const ClipDetails& clip : clipsProcessed)
    {
        segLen = clip.clipDurationFrames;
        fileLen = clip.fileDurationFrames;
        videoFileLen = clip.fileDurationFrames;
        QFileInfo fInfo(QString::fromStdWString(*clip.clipFormatInfo->bmEssenceLocators->bmEssenceLocator.front()->location));

        //Make the Tape MOB
        auto tapeMob = createInstanceHelper<IAAFSourceMob>(cdSourceMob.get(), IID_IAAFSourceMob);
        auto tapeDesc = createInstanceHelper<IAAFImportDescriptor>(cdTapeDescriptor.get(), IID_IAAFImportDescriptor);
        check(tapeMob->QueryInterface(IID_IAAFMob, (void**) &mob));
        auto essenceDesc = queryInterfaceHelper<IAAFEssenceDescriptor>(tapeDesc.get(), IID_IAAFEssenceDescriptor);

        // Make a locator, and attach it to the EssenceDescriptor
        auto netLocator = createInstanceHelper<IAAFNetworkLocator>(cdNetworkLocator.get(), IID_IAAFNetworkLocator);
        auto locator = queryInterfaceHelper<IAAFLocator>(netLocator.get(), IID_IAAFLocator);

        check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
        check(essenceDesc->AppendLocator(locator.get()));

        check(tapeMob->SetEssenceDescriptor(essenceDesc.get()));

        aafLength_t componentTotalLength = fileLen;
        check(tapeMob->AddNilReference(1, componentTotalLength, dDefPicture.get(), edlVideoRate));

        //Set PhysicalTrackNumber
        mobSlot.reset(lookupSlotHelper(mob, 1).release());
        check(mobSlot->SetPhysicalNum(1));
        check(mobSlot->SetName(L"V1"));

        audioTimelineSlotPos = 0;
        for (int s = 0; s < clip.nrAudioTracks; s++)
        {
            check(tapeMob->AddNilReference(audioTimelineSlotPos + 2, componentTotalLength, soundDef.get(), edlVideoRate));

            //Set PhysicalTrackNumber
            mobSlot.reset(lookupSlotHelper(mob, audioTimelineSlotPos + 2).release());
            check(mobSlot->SetPhysicalNum(audioTimelineSlotPos + 1));
            check(mobSlot->SetName(get_track_name(L"A", audioTimelineSlotPos + 1).c_str()));
            audioTimelineSlotPos++;
        }

        check(tapeMob->AppendTimecodeSlot(edlVideoRate,
                                          audioTimelineSlotPos + 2,
                                          tapeTC,
                                          componentTotalLength));

        //Set PhysicalTrackNumber
        mobSlot.reset(lookupSlotHelper(mob, audioTimelineSlotPos + 2).release());
        check(mobSlot->SetPhysicalNum(1));

        mob->Release();
        mob = NULL;

        check(tapeMob->QueryInterface(IID_IAAFMob, (void**) &mob));

        //Set tape Mob name
        check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));
        check(header->AddMob(mob));
        check(mob->GetMobID(&tapeMobID));

        mob->Release();
        mob = NULL;

        // Make a FileMob
        auto fileMob = createInstanceHelper<IAAFSourceMob>(cdSourceMob.get(), IID_IAAFSourceMob);
        auto fileDesc = createInstanceHelper<IAAFFileDescriptor>(cdDigitalImageDescriptor.get(), IID_IAAFFileDescriptor);
        essenceDesc.reset(queryInterfaceHelper<IAAFEssenceDescriptor>(fileDesc.get(), IID_IAAFEssenceDescriptor).release());

        //Request the fileMob Image Description
        auto digitalImageDesc = queryInterfaceHelper<IAAFDigitalImageDescriptor2>(fileDesc.get(), IID_IAAFDigitalImageDescriptor2);
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

        // Make a locator, and attach it to the EssenceDescriptor
        netLocator.reset(createInstanceHelper<IAAFNetworkLocator>(cdNetworkLocator.get(), IID_IAAFNetworkLocator).release());
        locator.reset(queryInterfaceHelper<IAAFLocator>(netLocator.get(), IID_IAAFLocator).release());

        check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
        check(essenceDesc->AppendLocator(locator.get()));

        check(fileMob->SetEssenceDescriptor(essenceDesc.get()));

        sourceRef.sourceID = tapeMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = 0;
        check(fileMob->NewPhysSourceRef(edlVideoRate, 1, dDefPicture.get(), sourceRef, fileLen));

        check(fileMob->QueryInterface(IID_IAAFMob, (void**) &mob));
        check(mob->SetName(L"Video file"));

        check(mob->GetMobID(&fileMobID));
        check(header->AddMob(mob));
        mobSlot.reset(lookupSlotHelper(mob, 1).release());
        check(mobSlot->SetName(L"V1"));
        check(mobSlot->SetPhysicalNum(1));

        mob->Release();
        mob = NULL;

        //Make the Master MOB
        auto masterMob = createInstanceHelper<IAAFMasterMob>(cdMasterMob.get(), IID_IAAFMasterMob);

        sourceRef.sourceID = fileMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = 0;
        check(masterMob->NewPhysSourceRef(edlVideoRate, 1, dDefPicture.get(), sourceRef, fileLen));

        check(masterMob->QueryInterface(IID_IAAFMob, (void**) &mob));

        check(mob->GetMobID(&masterMobID));

        //Set MarkIn and MarkOut in masterMob
        mobSlot.reset(lookupSlotHelper(mob, 1).release());
        check(mobSlot->SetName(L"V1"));
        check(mobSlot->SetPhysicalNum(1));
        auto timelineMobSlot2 = queryInterfaceHelper<IAAFTimelineMobSlot2>(mobSlot.get(), IID_IAAFTimelineMobSlot2);
        check(timelineMobSlot2->SetMarkIn(clip.clipMarkInFrames));
        check(timelineMobSlot2->SetMarkOut(clip.clipMarkOutFrames));

        //Set masterMobName
        check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));

        check(header->AddMob(mob));
        mob->Release();
        mob = NULL;

        // Create a SourceClip
        auto compSclp = createInstanceHelper<IAAFSourceClip>(cdSourceClip.get(), IID_IAAFSourceClip);

        sourceRef.sourceID = masterMobID;
        sourceRef.sourceSlotID = 1;
        sourceRef.startTime = clip.clipMarkInFrames;
        check(compSclp->SetSourceReference(sourceRef));
        component.reset(queryInterfaceHelper<IAAFComponent>(compSclp.get(), IID_IAAFComponent).release());
        check(component->SetDataDef(dDefPicture.get()));
        check(component->SetLength(segLen));
        check(videoSequence->AppendComponent(component.get()));

        int audioTrackNumber = 0;
        int audioTimelineSlotPos = 0;

        //Cicle between all the audio files that will be added
        for (int z = 0; z < clip.nrAudioChannels; z++)
        {
            if (clip.nrAudioChannels == 1 && maxStereos != 0)
            {
                for (; audioTimelineSlotPos < (maxStereos * 2); audioTimelineSlotPos++)
                {
                    auto compFill = createInstanceHelper<IAAFComponent>(cdFiller.get(), IID_IAAFComponent);

                    check(compFill->SetLength(segLen));
                    check(compFill->SetDataDef(soundDef.get()));
                    check(audioSequences[audioTimelineSlotPos]->AppendComponent(compFill.get()));
                }
            }

            // Make a FileMob
            fileMob.reset(createInstanceHelper<IAAFSourceMob>(cdSourceMob.get(), IID_IAAFSourceMob).release());
            fileDesc.reset(createInstanceHelper<IAAFFileDescriptor>(cdSoundDescriptor.get(), IID_IAAFFileDescriptor).release());
            essenceDesc.reset(queryInterfaceHelper<IAAFEssenceDescriptor>(fileDesc.get(), IID_IAAFEssenceDescriptor).release());

            //Request the fileMob Audio Description
            auto soundDesc = queryInterfaceHelper<IAAFSoundDescriptor>(fileDesc.get(), IID_IAAFSoundDescriptor);

            auto audioSamplingRateValue = std::stoul(*clip.audioInfo->samplingRate);
            aafRational_t audioSamplingRate = { audioSamplingRateValue, 1 };
            aafLength_t audioLength = (audioSamplingRateValue * fileLen * edlVideoRate.denominator) / edlVideoRate.numerator;

            check(fileDesc->SetLength(audioLength));
            check(fileDesc->SetSampleRate(audioSamplingRate));

            check(soundDesc->SetAudioSamplingRate(audioSamplingRate));
            check(soundDesc->SetChannelCount(clip.nrAudioChannels));
            check(soundDesc->SetQuantizationBits(std::stoul(*clip.audioInfo->sampleSize)));

            // Make a locator, and attach it to the EssenceDescriptor
            netLocator.reset(createInstanceHelper<IAAFNetworkLocator>(cdNetworkLocator.get(), IID_IAAFNetworkLocator).release());
            locator.reset(queryInterfaceHelper<IAAFLocator>(netLocator.get(), IID_IAAFLocator).release());

            check(locator->SetPath(fInfo.absoluteFilePath().toStdWString().c_str()));
            check(essenceDesc->AppendLocator(locator.get()));

            check(fileMob->SetEssenceDescriptor(essenceDesc.get()));

            check(fileMob->QueryInterface(IID_IAAFMob, (void**) &mob));
            check(mob->SetName(L"Audio file"));

            for (int s = 0; s < clip.nrAudioTracks; s++)
            {
                sourceRef.sourceID = tapeMobID;
                sourceRef.sourceSlotID = s + 2;
                sourceRef.startTime = 0;

                check(fileMob->NewPhysSourceRef(edlVideoRate, s + 1, soundDef.get(), sourceRef, fileLen));

                mobSlot.reset(lookupSlotHelper(mob, s + 1).release());
                check(mobSlot->SetName(get_track_name(L"A", z + s + 1).c_str()));
                check(mobSlot->SetPhysicalNum(audioTrackNumber + 1));
                audioTrackNumber++;
            }

            check(mob->GetMobID(&fileMobID));
            check(header->AddMob(mob));
            mob->Release();
            mob = NULL;

            sourceRef.sourceID = fileMobID;
            sourceRef.sourceSlotID = 1;
            sourceRef.startTime = 0;
            check(masterMob->AppendPhysSourceRef(edlVideoRate, z + 2, soundDef.get(), sourceRef, fileLen));
            check(masterMob->QueryInterface(IID_IAAFMob, (void**) &mob));
            check(mob->GetMobID(&masterMobID));

            //Set masterMobName
            check(mob->SetName(fInfo.completeBaseName().toStdWString().c_str()));

            mobSlot.reset(lookupSlotHelper(mob, z + 2).release());
            check(mobSlot->SetName(get_track_name(L"A", z + 1).c_str()));
            check(mobSlot->SetPhysicalNum(z + 1));
            timelineMobSlot2.reset(queryInterfaceHelper<IAAFTimelineMobSlot2>(mobSlot.get(), IID_IAAFTimelineMobSlot2).release());
            check(timelineMobSlot2->SetMarkIn(clip.clipMarkInFrames));
            check(timelineMobSlot2->SetMarkOut(clip.clipMarkOutFrames));

            mob->Release();
            mob = NULL;

            // the remaining part of the sequence code, adapted for updating slot names
            for (int s = 0; s < clip.nrAudioTracks; s++)
            {
                // Create a SourceClip
                compSclp.reset(createInstanceHelper<IAAFSourceClip>(cdSourceClip.get(), IID_IAAFSourceClip).release());

                sourceRef.sourceID = masterMobID;
                sourceRef.sourceSlotID = z + 2;
                sourceRef.startTime = clip.clipMarkInFrames;
                check(compSclp->SetSourceReference(sourceRef));
                component.reset(queryInterfaceHelper<IAAFComponent>(compSclp.get(), IID_IAAFComponent).release());
                check(component->SetDataDef(soundDef.get()));
                check(component->SetLength(segLen));
                check(audioSequences[audioTimelineSlotPos]->AppendComponent(component.get()));

                audioTimelineSlotPos++;
            }
        }

        for (; audioTimelineSlotPos < maxSlots; audioTimelineSlotPos++)
        {
            // Create a filler - Get the component interface only (IID_IAAFComponent)
            auto compFill = createInstanceHelper<IAAFComponent>(cdFiller.get(), IID_IAAFComponent);

            check(compFill->SetLength(segLen));
            check(compFill->SetDataDef(soundDef.get()));
            check(audioSequences[audioTimelineSlotPos]->AppendComponent(compFill.get()));
        }

        //  end of loop since only one dictionary and header are needed
        //  the file is then saved, closed and released after all modifications
        //  are complete
    }

//cleanup:
    // Cleanup and return

    if (mob)
        mob->Release();

    file->Save();
    file->Close();

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
