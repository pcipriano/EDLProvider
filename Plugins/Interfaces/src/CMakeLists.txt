set(SOURCES
    EdlPluginManager.cpp
    EdlException.cpp
   )

set(HEADERS
    EdlInterface.h
    EdlPluginManager.h
    LoggingHelpers.h
    SharedLoggerInterface.h
    EdlException.h
   )

include_directories(../../../Common/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(EdlPlugins ${SOURCES} ${HEADERS})
target_link_libraries(EdlPlugins Qt5::Core)
