set(SOURCES
        Main.cpp
        EDLProviderService.cpp
    )

set(HEADERS
        EDLProviderService.h
   )

include_directories(../../Soap/src)
include_directories(../../Common/src)
include_directories(../../Soap/gsoap/src)
include_directories(../../Submodules/easyloggingpp/src)
include_directories(../../Submodules/qt-solutions/qtservice/src)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} SoapServer Qt5::Core QtSolutions_Service Common)

copy_file_to_target_dir("${PROJECT_SOURCE_DIR}/Core/configs/${LOG_CONFIG_FILE_NAME}" ${PROJECT_NAME})
