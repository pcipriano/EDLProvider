# EDLProvider #

EDLProvider is a simple application that provides edit decision lists of various formats. The application exposes a SOAP service using FIMS (Framework for Interoperable Media Services) elements to facilitate integration for users already acquainted with the FIMS framework. For more information on FIMS go to http://www.fims.tv/

# Table of contents #

* [EDLProvider](https://github.com/pcipriano/EDLProvider#edlprovider)
* [Introduction](https://github.com/pcipriano/EDLProvider#introduction)
* [EDL Plugins](https://github.com/pcipriano/EDLProvider#edl-plugins)
  * [Logger](https://github.com/pcipriano/EDLProvider#logger)
* [Building](https://github.com/pcipriano/EDLProvider#building)
  * [CMake](https://github.com/pcipriano/EDLProvider#cmake)
  * [Qt 5](https://github.com/pcipriano/EDLProvider#qt-5)
* [Unit Tests](https://github.com/pcipriano/EDLProvider#unit-tests)
* [EDL Generation](https://github.com/pcipriano/EDLProvider#edl-generation)
  * [Mark In/Out](https://github.com/pcipriano/EDLProvider#mark-inout)
  * [Values](https://github.com/pcipriano/EDLProvider#values)
* [SOAP Code Generation](https://github.com/pcipriano/EDLProvider#soap-code-generation)
* [Roadmap](https://github.com/pcipriano/EDLProvider#roadmap)

# Introduction #

EDLProvider runs as a service/daemon when installed using the provided packages. The project uses a plugin architecture where all the EDL are implemented in separate libraries (go to [EDL Plugins](https://github.com/pcipriano/EDLProvider#edl-plugins) for information on how to implement one).

# EDL Plugins #

To add an EDL plugin to the project start by creating a library. To simplify copy the contents of "Plugins/FinalCut" to a new folder inside "Plugins" folder and rename everything referring to FinalCut.
Add an entry in the CMakeLists.txt file inside the "Plugins" directory. Now add a class to the library that implements both QObject and interfaces::EdlInterface, check the example:

```cpp

#include <QObject>
#include "EdlInterface.h"
namespace plugins
{

class MyPlugin : public QObject, public interfaces::EdlInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID EDL_INTERFACE_IDENTIFIER)
    Q_INTERFACES(plugins::interfaces::EdlInterface)

public:
    //Implement EdlInterface
    std::wstring getEdlName() const override final { return L"My Plugin Name"; }
    std::wstring getEdlExtension() const override final { return L".txt"; }
    QByteArray createEdl(const std::wstring* const edlSequenceName,
                         const fims__RationalType* const edlFrameRate,
                         const std::vector<edlprovider__ClipType*>& clips) const override final { /*Code for generating the EDL data*/ }
}

}
```
After following the steps above you should be able to test the plugin. Build the project and run the main executable with "-e" argument to run the executable as if it wasn't a service. Make a soap request getInstalledEdls which if everything was done right should return your plugin.

## Logger ##

To use the application wide logger in a plugin just implement interfaces::SharedLoggerInterface. If you want to use a logger id unique in the plugin, override setEasyloggingStorage:

```cpp

void setEasyloggingStorage(el::base::type::StoragePointer storage) override
{
    //Call base implementation
    SharedLoggerInterface::setEasyloggingStorage(storage);

    //Now create a logger with a new id
    el::Loggers::getLogger("CustomLoggerId");
}

```

Look into FinalCut plugin for an example on how to use it.

For more information on the log library [Easylogging++](http://easylogging.muflihun.com/)

# Building #

Building requires CMake and Qt 5. Because many classes use c++11 features for windows, VS2013 is the minimum version required, for Linux gcc 4.8 and Mac clang 3.3 should be enough to compile the project.
For building the installer (msi) on windows WIX needs to be installed.

## CMake ##

To build the project is recommended to use an out of source build (in source build has not been tested). Just create a empty folder and execute for example:
```
CMake -G"Visual Studio 12 2013" "path to project checkout folder"
```
For windows the minimum required version of CMake is 3.0.0 for Linux and Mac 2.8.12. My recommendation is to use the latest version available on their website.

## Qt 5 ##

Any version of Qt 5 can be used to compile the project, although using the latest version is advisable. Take in consideration the fact that not all compilers will build the project when downloading a specific binary version from the Qt website.

[CMake](http://www.cmake.org/)
[Qt](http://www.qt.io/)
[WIX](http://wixtoolset.org/)

# Unit Tests #

To run all the unit tests simply build the project and in the root binary directory run:
```
ctest
```
For more information on how to use CMake CTest command head to Ctest documentation available online.

# EDL Generation #

To generate an EDL some details are required on the soap request. Some of the values might not be required in all EDL types but they should always be filled.

## Mark In/Out ##

Both MarkIn and MarkOut elements use the FIMS TimeType, this type has three possible values:

1. editUnitNumber
  *This is the actual number of frames from the start of the clip.
```xml
<markIn>
  <editUnitNumber editRate="25" factorNumerator="1" factorDenominator="1">100</editUnitNumber>
</markIn>
```
2. normalPlayTime
  *This is the actual time of the day in the format hh:mm:ss.sss with an optional following time zone indicator.
```xml
<markIn>
  <normalPlayTime>10:40:20.400</normalPlayTime>
</markIn>
```
3. timecode
  *This is a timecode that represents the point in time where the mark in/out exists.
```xml
<markIn>
  <timecode>00:11:12:13</timecode>
</markIn>
```

Make sure that markout is bigger than the markin to be able to generate an EDL.

For elements using the FIMS DurationType the normalPlayTime remarked above will have a different meaning and the expected value is simply the duration of the clip in milliseconds.

## Values ##

Different EDL implementations will have more or less requirements on which values need to be provided to create an EDL. Below are the values that will allow a EDL of any type to be created (basically the list of parameters that should be understood by any EDL in general).

A working request with the minimal amount of information provided would be:
```xml
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:edl="http://temp/edlprovider" xmlns:bas="http://baseTime.fims.tv" xmlns:base="http://base.fims.tv" xmlns:des="http://description.fims.tv">
   <soapenv:Header/>
   <soapenv:Body>
   <edl:getEdlDoubleRequest>
      <edl:edlType>FinalCut</edl:edlType>
      <edl:edlFramesPerSecond>25</edl:edlFramesPerSecond>
   </edl:getEdlDoubleRequest>
</soapenv:Body>
</soapenv:Envelope>
```
This is essentially of no use since no clips are on the EDL, so it would produce an EDL with no media inside.

There's two different soap operations available, one will accept the EDL frames per second to be specified as a rounded value the other a precise value.
```xml
<!--Rounded value-->
<edl:edlFramesPerSecond>25</edl:edlFramesPerSecond>
<!--Precise specification of the frame rate-->
<edl:edlFramesPerSecond numerator="1" denominator="1">25</edl:edlFramesPerSecond>
<!--Ntsc frame rate-->
<edl:edlFramesPerSecond numerator="1000" denominator="1001">30</edl:edlFramesPerSecond>
```

The **edlType** specifies the EDL type to generate, the types available can be obtained by using getInstalledEdls soap operation.

To include clips of media in an EDL the request must contain at least one clip element. The clip element contains three top level child elements:

* markIn
* markOut
* clipInfo

The **markIn/markOut** elements have already been described in [Mark In/Out](https://github.com/pcipriano/EDLProvider#mark-inout).

The **clipInfo** uses the FIMS type BMObjectType. Not all elements need to be filled and although FIMS specifies some of the elements as optional they might be required in the context of EDL generation.

Breakdown of the elements understood and/or required by an EDL plugin:

* resourceID
  * Although required by the FIMS specification its value is not really required for the generation of an EDL.
* location
  * The input file location of the clip. This is the element in bmEssenceLocator, at least one bmContentFormat must be provided.
* displayWidth
  * The width of the media being pointed(e.g. 1920, 1280, 640). This element is inside videoFormat FIMS element contained inside the element formatCollection.
* displayHeight
  * The same as displayWidth but now for the media height dimensions.
* frameRate
  * The frame rate of the media.
* aspectRatio
  * The aspect ratio of the media. Refer to the FIMS specification to see which values to use in this element.
* lines
  * Number of scanning lines in the media, normally the same as displayHeight.
* scanningFormat
  * The scanning format of the media. If it's interlaced or progressive media.
* scanningOrder
  * If scanningFormat is interlaced this element will indicate which filed comes first (Top or Bottom fields).
* samplingRate
  * Audio sampling rate of all audio streams, all audio streams in a file must have the same layout (e.g. 48000, 44100).
* audioTrack
  * This will be one element per audio stream in the media.
* channels
  * The number of audio channels per audio stream. Mono will has a value of 1 stereo 2 etc.
* sampleSize
  * The audio sample size or also commonly called number of quantization bits (e.g. 16 or 24).
* edlSequenceName
  * An optional element to give the EDL sequence a name. This information has different meanings depending on which EDL is being generated.
* edlFramesPerSecond
  * This is the frames per second for the complete sequence. Clips inside the sequence can have different frame rates. Some EDL's do not support mixed frame rates in a sequence.

Example of a minimalist request:
```xml
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:edl="http://temp/edlprovider" xmlns:bas="http://baseTime.fims.tv" xmlns:base="http://base.fims.tv" xmlns:des="http://description.fims.tv">
   <soapenv:Header/>
   <soapenv:Body>
      <edl:getEdlDoubleRequest>
         <edl:edlType>FinalCut</edl:edlType>
         <edl:clips>
            <edl:clips>
               <edl:markIn>
                  <bas:timecode>00:11:12:13</bas:timecode>
               </edl:markIn>
               <edl:markOut>
                  <bas:timecode>00:12:02:10</bas:timecode>
               </edl:markOut>
               <edl:clipInfo>
                  <base:resourceID>?</base:resourceID>
                  <base:bmContents>
                     <base:bmContent>
                        <base:resourceID>urn:smpte:umid:050a2b34.85632554.0e0a0f20.13000000.5146f23d.c3444642.bb7139eb.05427734</base:resourceID>
                        <base:bmContentFormats>
                           <base:bmContentFormat>
                              <base:resourceID>?</base:resourceID>
                              <base:bmEssenceLocators>
                                 <base:bmEssenceLocator>
                                    <base:resourceID>?</base:resourceID>
                                    <base:location>c:/TestLocation/Myfile.mxf</base:location>
                                 </base:bmEssenceLocator>
                              </base:bmEssenceLocators>
                              <base:formatCollection>
                                 <base:videoFormat>
                                    <base:resourceID>?</base:resourceID>
                                    <base:displayWidth unit="?">1920</base:displayWidth>
                                    <base:displayHeight unit="?">1080</base:displayHeight>
                                    <base:frameRate numerator="1" denominator="1">25</base:frameRate>
                                    <base:aspectRatio numerator="16" denominator="9">1</base:aspectRatio>
                                    <base:lines>1080</base:lines>
                                    <base:scanningFormat>interlaced</base:scanningFormat>
                                    <base:scanningOrder>top</base:scanningOrder>
                                 </base:videoFormat>
                                 <base:audioFormat>
                                    <base:resourceID>?</base:resourceID>
                                    <base:samplingRate>48000</base:samplingRate>
                                    <!--Zero or more repetitions:-->
                                    <base:audioTrack trackID="0" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="1" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="2" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="3" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:channels>1</base:channels>
                                    <base:sampleSize>24</base:sampleSize>
                                 </base:audioFormat>
                              </base:formatCollection>
                              <base:duration>
                                 <bas:timecode>00:12:02:10</bas:timecode>
                              </base:duration>
                           </base:bmContentFormat>
                        </base:bmContentFormats>
                     </base:bmContent>
                  </base:bmContents>
               </edl:clipInfo>
            </edl:clips>
         </edl:clips>
         <edl:edlSequenceName>My Sequence</edl:edlSequenceName>
         <edl:edlFramesPerSecond>25</edl:edlFramesPerSecond>
      </edl:getEdlDoubleRequest>
   </soapenv:Body>
</soapenv:Envelope>
```

# SOAP Code Generation #

EDLProvider uses gSOAP to generate SOAP server code from a WSDL file. To regenerate all the code automatically in ************ there's a python script that will run the gSOAP tools to generate the server code. Simply run the script and specify the gSOAP root directory for it to work.

[gSOAP](http://www.cs.fsu.edu/~engelen/soap.html)

# Roadmap #

The project's future is to add support to more edit decisions list formats. The Advanced Authoring Format (AAF) and Final Cut Pro X xml are the next edl formats to support.
