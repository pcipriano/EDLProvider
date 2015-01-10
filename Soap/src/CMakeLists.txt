set(SOURCES
    ../gsoap/src/EDLProviderC.cpp
    ../gsoap/src/duration.cpp
    ../gsoap/src/stdsoap2.cpp
    ../gsoap/src/EDLProviderEdlProviderBindingService.cpp
    EdlProviderServer.cpp
    EdlProviderProcessRequest.cpp
    EdlProviderServerThread.cpp
   )

set(HEADERS
    ../gsoap/src/EDLProviderEdlProviderBindingService.h
    ../gsoap/src/EDLProviderH.h
    ../gsoap/src/EDLProviderStub.h
    ../gsoap/src/stdsoap2.h
    ../gsoap/src/EdlProviderBinding.nsmap
    ../gsoap/src/duration.h
    EdlProviderServer.h
    EdlProviderProcessRequest.h
    EdlProviderServerThread.h
   )

include_directories(../gsoap/src)
include_directories(../../Common/src)
include_directories(../../Plugins/Interfaces/src)
include_directories(../../Submodules/easyloggingpp/src)

add_library(SoapServer ${SOURCES} ${HEADERS})
target_link_libraries(SoapServer EdlPlugins Qt5::Core)
set_target_properties(SoapServer PROPERTIES COMPILE_DEFINITIONS "WITH_PURE_VIRTUAL")
