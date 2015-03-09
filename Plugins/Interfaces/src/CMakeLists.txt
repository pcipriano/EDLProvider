set(SOURCES
    EdlPluginManager.cpp
    EdlException.cpp
    EdlHelpers.cpp
   )

set(HEADERS
    EdlInterface.h
    EdlPluginManager.h
    LoggingHelpers.h
    SharedLoggerInterface.h
    EdlException.h
    EdlHelpers.h
   )

include_directories(../../../Common/src)
include_directories(../../../TimeUtils/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(EdlPlugins ${SOURCES} ${HEADERS})
target_link_libraries(EdlPlugins TimeUtils Qt5::Core)
