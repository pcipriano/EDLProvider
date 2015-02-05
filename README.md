# EDLProvider #

EDLProvider is a simple application that provides edit decision lists of various formats. The application exposes a SOAP service using FIMS (Framework for Interoperable Media Services) elements to facilitate integration for users already acquainted with the FIMS framework. For more information on FIMS go to http://www.fims.tv/

# Table of contents #

* [EDLProvider](https://github.com/pcipriano/EDLProvider#edlprovider)
* [Introduction](https://github.com/pcipriano/EDLProvider#introduction)
* [EDL Plugins](https://github.com/pcipriano/EDLProvider#edl-plugins)
* [Building](https://github.com/pcipriano/EDLProvider#building)
* [Unit Tests](https://github.com/pcipriano/EDLProvider#unit-tests)
* [EDL Generation](https://github.com/pcipriano/EDLProvider#edl-generation)
* [SOAP Code Generation](https://github.com/pcipriano/EDLProvider#soap-code-generation)
* [Roadmap](https://github.com/pcipriano/EDLProvider#roadmap)

# Introduction #

EDLProvider runs as a service/daemon when installed using the provided packages. The project uses a plugin architecture where all the EDL are implemented in separate libraries (go to [EDL Plugins](https://github.com/pcipriano/EDLProvider#edl-plugins) for information on how to implement one).

# EDL Plugins #

To add an EDL plugin to the project start by creating a library. To simplify copy the contents of "Plugins/FinalCut" to a new folder inside "Plugins" folder and rename everything referring to FinalCut.
Add an entry in the CMakeLists.txt file inside the "Plugins" directory. Now add a class to the library that implements both QObject and interfaces::EdlInterface, check the example:

```
#!c++

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

```
#!c++

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
```
#!xml
<markIn>
  <editUnitNumber editRate="25" factorNumerator="1" factorDenominator="1">100</editUnitNumber>
</markIn>
```
2. normalPlayTime
  *This is the actual time of the day in the format hh:mm:ss.sss with an optional following time zone indicator.
```
#!xml
<markIn>
  <normalPlayTime>10:40:20.400</normalPlayTime>
</markIn>
```
3. timecode
  *This is a timecode that represents the point in time where the mark in/out exists.
```
#!xml
<markIn>
  <timecode>00:11:12:13</timecode>
</markIn>
```

Make sure that markout is bigger than the markin to be able to generate an EDL.

For elements using the FIMS DurationType the normalPlayTime remarked above will have a different meaning and the expected value is simply the duration of the clip in milliseconds.

## Required Values ##


Example of a minimalist request:
```
#!xml
<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:edl="http://temp/edlprovider" xmlns:bas="http://baseTime.fims.tv" xmlns:base="http://base.fims.tv" xmlns:des="http://description.fims.tv">
   <soapenv:Header/>
   <soapenv:Body>
      <edl:getEdlDoubleRequest>
         <edl:edlType>FinalCut</edl:edlType>
         <!--Optional:-->
         <edl:clips>
            <!--Zero or more repetitions:-->
            <edl:clips>
               <edl:markIn>
                  <!--You have a CHOICE of the next 3 items at this level-->
                  <bas:timecode>00:11:12:13</bas:timecode>
               </edl:markIn>
               <edl:markOut>
                  <!--You have a CHOICE of the next 3 items at this level-->
                  <bas:timecode>00:12:02:10</bas:timecode>
               </edl:markOut>
               <edl:clipInfo>
                  <base:resourceID>?</base:resourceID>
                  <!--Optional:-->
                  <base:bmContents>
                     <!--1 or more repetitions:-->
                     <base:bmContent>
                        <base:resourceID>urn:smpte:umid:060a2b34.01010108.0e0a0f20.13000000.5146f23d.c3444642.bb7139eb.05427734</base:resourceID>
                        <!--Optional:-->
                        <base:bmContentFormats>
                           <!--1 or more repetitions:-->
                           <base:bmContentFormat>
                              <base:resourceID>?</base:resourceID>
                              <!--Optional:-->
                              <base:bmEssenceLocators>
                                 <!--1 or more repetitions:-->
                                 <base:bmEssenceLocator>
                                    <base:resourceID>?</base:resourceID>
                                    <!--Optional:-->
                                    <base:revisionID>?</base:revisionID>
                                    <!--Optional:-->
                                    <base:location>c:/TestLocation/Myfile.mxf</base:location>
                                    <!--Optional:-->
                                    <base:containerMimeType typeLabel="?" typeDefinition="?" typeLink="?">?</base:containerMimeType>
                                 </base:bmEssenceLocator>
                                 <!--You may enter ANY elements at this point-->
                              </base:bmEssenceLocators>
                              <!--Optional:-->
                              <base:formatCollection>
                                 <!--Optional:-->
                                 <base:videoFormat>
                                    <base:resourceID>?</base:resourceID>
                                    <!--Optional:-->
                                    <base:notifyAt>
                                       <base:replyTo>?</base:replyTo>
                                       <base:faultTo>?</base:faultTo>
                                    </base:notifyAt>
                                    <!--Optional:-->
                                    <base:ExtensionGroup>
                                       <!--You may enter ANY elements at this point-->
                                    </base:ExtensionGroup>
                                    <!--Optional:-->
                                    <base:ExtensionAttributes/>
                                    <!--Zero or more repetitions:-->
                                    <base:technicalAttribute typeLabel="?" typeDefinition="?" typeLink="?" formatLabel="?" formatDefinition="?" formatLink="?">?</base:technicalAttribute>
                                    <!--Optional:-->
                                    <base:displayWidth unit="?">1920</base:displayWidth>
                                    <!--Optional:-->
                                    <base:displayHeight unit="?">1080</base:displayHeight>
                                    <!--Optional:-->
                                    <base:frameRate numerator="1" denominator="1">25</base:frameRate>
                                    <!--Optional:-->
                                    <base:aspectRatio numerator="16" denominator="9">1</base:aspectRatio>
                                    <!--Optional:-->
                                    <base:videoEncoding typeLabel="?" typeDefinition="?" typeLink="?">
                                       <!--Optional:-->
                                       <base:name>?</base:name>
                                       <!--Optional:-->
                                       <base:vendor>?</base:vendor>
                                       <!--Optional:-->
                                       <base:version>?</base:version>
                                       <!--Optional:-->
                                       <base:family>?</base:family>
                                    </base:videoEncoding>
                                    <!--Zero or more repetitions:-->
                                    <base:videoTrack trackID="?" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?">
                                       <!--Optional:-->
                                       <base:ExtensionGroup>
                                          <!--You may enter ANY elements at this point-->
                                       </base:ExtensionGroup>
                                       <!--Optional:-->
                                       <base:ExtensionAttributes/>
                                    </base:videoTrack>
                                    <!--Optional:-->
                                    <base:lines>1080</base:lines>
                                    <!--Optional:-->
                                    <base:scanningFormat>interlaced</base:scanningFormat>
                                    <!--Optional:-->
                                    <base:scanningOrder>top</base:scanningOrder>
                                 </base:videoFormat>
                                 <!--Optional:-->
                                 <base:audioFormat>
                                    <base:resourceID>?</base:resourceID>
                                    <!--Optional:-->
                                    <base:notifyAt>
                                       <base:replyTo>?</base:replyTo>
                                       <base:faultTo>?</base:faultTo>
                                    </base:notifyAt>
                                    <!--Optional:-->
                                    <base:ExtensionGroup>
                                       <!--You may enter ANY elements at this point-->
                                    </base:ExtensionGroup>
                                    <!--Optional:-->
                                    <base:ExtensionAttributes/>
                                    <!--Zero or more repetitions:-->
                                    <base:technicalAttribute typeLabel="?" typeDefinition="?" typeLink="?" formatLabel="?" formatDefinition="?" formatLink="?">?</base:technicalAttribute>
                                    <!--Optional:-->
                                    <base:samplingRate>48000</base:samplingRate>
                                    <!--Optional:-->
                                    <base:audioEncoding typeLabel="?" typeDefinition="?" typeLink="?">
                                       <!--Optional:-->
                                       <base:name>?</base:name>
                                       <!--Optional:-->
                                       <base:vendor>?</base:vendor>
                                       <!--Optional:-->
                                       <base:version>?</base:version>
                                       <!--Optional:-->
                                       <base:family>?</base:family>
                                    </base:audioEncoding>
                                    <!--Optional:-->
                                    <base:trackConfiguration typeLabel="?" typeDefinition="?" typeLink="?"/>
                                    <!--Zero or more repetitions:-->
                                    <base:audioTrack trackID="0" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="1" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="2" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <base:audioTrack trackID="3" typeLabel="?" typeDefinition="?" typeLink="?" trackName="?" language="?"/>
                                    <!--Optional:-->
                                    <base:channels>1</base:channels>
                                    <!--Optional:-->
                                    <base:sampleSize>24</base:sampleSize>
                                 </base:audioFormat>
                              </base:formatCollection>
                              <!--Optional:-->
                              <base:duration>
                                 <!--You have a CHOICE of the next 3 items at this level-->
                                 <bas:timecode>00:12:02:10</bas:timecode>
                              </base:duration>
                           </base:bmContentFormat>
                           <!--You may enter ANY elements at this point-->
                        </base:bmContentFormats>
                     </base:bmContent>
                     <!--You may enter ANY elements at this point-->
                  </base:bmContents>
               </edl:clipInfo>
            </edl:clips>
         </edl:clips>
         <!--Optional:-->
         <edl:edlSequenceName>My Sequence</edl:edlSequenceName>
         <edl:edlFramesPerSecond>25</edl:edlFramesPerSecond>
      </edl:getEdlDoubleRequest>
   </soapenv:Body>
</soapenv:Envelope>
```

# SOAP Code Generation #

# Roadmap #

The project's future is to add support to more edit decisions list formats. The Advanced Authoring Format (AAF) and Final Cut Pro X xml are the next edl formats to support.
