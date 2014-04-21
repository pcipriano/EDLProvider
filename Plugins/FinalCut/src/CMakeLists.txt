set(SOURCES
    FinalCut.cpp
   )

set(HEADERS
    FinalCut.h
   )

include_directories(../../../Common/src)
include_directories(../../Interfaces/src)

add_library(FinalCut MODULE ${SOURCES} ${HEADERS})
target_link_libraries(FinalCut EdlPlugins Qt5::Core)
set_target_properties(FinalCut PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
