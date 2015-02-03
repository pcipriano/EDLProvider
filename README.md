# EDLProvider #

EDLProvider is a simple application that provides edit decision lists of various formats. The application exposes a SOAP service using FIMS (Framework for Interoperable Media Services) elements to facilitate integration for users already acquainted with the FIMS framework. For more information on FIMS go to http://www.fims.tv/

# Table of contents #

* [EDLProvider](https://github.com/pcipriano/EDLProvider#edlprovider)
* [Introduction](https://github.com/pcipriano/EDLProvider#introduction)
* [EDL Plugins](https://github.com/pcipriano/EDLProvider#edl-plugins)
* [Building](https://github.com/pcipriano/EDLProvider#building)
* [Tests](https://github.com/pcipriano/EDLProvider#tests)
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

# Tests #

To run all the unit tests simply build the project and in the root binary directory run:
```
ctest
```
For more information on how to use CMake CTest command head to Ctest documentation available online.

# Roadmap #

The project's future is to add support to more edit decisions list formats. The Advanced Authoring Format (AAF) and Final Cut Pro X xml are the next edl formats to support.