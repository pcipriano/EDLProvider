set(SOURCES
    Cmx3600.cpp
   )

set(HEADERS
    Cmx3600.h
   )

include_directories(../../../Common/src)
include_directories(../../Interfaces/src)
include_directories(../../../TimeUtils/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(Cmx MODULE ${SOURCES} ${HEADERS})
target_link_libraries(Cmx EdlPlugins Qt5::Core Qt5::Xml Qt5::XmlPatterns)
set_target_properties(Cmx PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")

#Install plugin
install(TARGETS Cmx DESTINATION ${BIN_INSTALL_DIR_PLUGINS} COMPONENT Cmx)
