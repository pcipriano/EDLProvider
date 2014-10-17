set(SOURCES
    FinalCut.cpp
   )

set(HEADERS
    FinalCut.h
   )

include_directories(../../../Common/src)
include_directories(../../Interfaces/src)
include_directories(../../../TimeUtils/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(FinalCut MODULE ${SOURCES} ${HEADERS})
target_link_libraries(FinalCut EdlPlugins TimeUtils Qt5::Core Qt5::Xml Qt5::XmlPatterns)
set_target_properties(FinalCut PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
