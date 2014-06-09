set(SOURCES
    EdlPluginManager.cpp
   )

set(HEADERS
    EdlInterface.h
    EdlPluginManager.h
    loggingHelpers.h
    SharedLoggerInterface.h
   )

include_directories(../../../Common/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(EdlPlugins ${SOURCES} ${HEADERS})
target_link_libraries(EdlPlugins Qt5::Core)
