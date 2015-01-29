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

# Building #

# Tests #

To run all the unit tests simply build the project and in the root binary directory run:
```
ctest
```
For more information on how to use CMake CTest command head to Ctest documentation available online.

# Roadmap #

The project's future is to add support to more edit decisions list formats. The Advanced Authoring Format (AAF) and Final Cut Pro X xml are the next edl formats to support.