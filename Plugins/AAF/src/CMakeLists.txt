#Find necessary libraries
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
find_package(AAF REQUIRED)

set(SOURCES
    AafPlugin.cpp
   )

set(HEADERS
    AafPlugin.h
   )

include_directories(${AAF_INCLUDE_DIR})
include_directories(../../../Common/src)
include_directories(../../Interfaces/src)
include_directories(../../../TimeUtils/src)
include_directories(../../../Soap/gsoap/src)
include_directories(../../../Submodules/easyloggingpp/src)

add_library(Aaf MODULE ${SOURCES} ${HEADERS})
target_link_libraries(Aaf EdlPlugins TimeUtils Qt5::Core ${AAF_LIBRARY} ${AAFIID_LIBRARY})
set_target_properties(Aaf PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")

if(MSVC)
    target_compile_definitions(Aaf PUBLIC "-DNOMINMAX")
endif()

copy_file_to_destination("${AAF_MODULE}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#Install plugin
install(TARGETS Aaf DESTINATION ${BIN_INSTALL_DIR_PLUGINS} COMPONENT Aaf)
install(FILES ${AAF_MODULE} DESTINATION ${BIN_INSTALL_DIR} COMPONENT Aaf)
