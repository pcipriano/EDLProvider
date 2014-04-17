set(SOURCES
    EdlPluginManager.cpp
   )

set(HEADERS
    EdlInterface.h
    EdlPluginManager.h
   )

include_directories(../../../Common/src)

add_library(EdlPlugins ${SOURCES} ${HEADERS})
target_link_libraries(EdlPlugins Qt5::Core)
