set(SOURCES
        Main.cpp
        EdlProviderService.cpp
        Configuration.cpp
    )

set(HEADERS
        EdlProviderService.h
        Configuration.h
   )

set (RESOURCES
        ../configs/Core.qrc
)

#Configure log configuration
set_log_path("${PROJECT_NAME}Logs")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../configs/${LOG_CONFIG_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/../configs/${LOG_CONFIG_FILE_NAME}"
  )

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../configs/${LOG_CONFIG_FILE_NAME}.deploy.in"
    "${CMAKE_CURRENT_BINARY_DIR}/../configs/${LOG_CONFIG_FILE_NAME}.deploy"
  )


include_directories(../../Soap/src)
include_directories(../../Common/src)
include_directories(../../Soap/gsoap/src)
include_directories(../../Plugins/Interfaces/src)
include_directories(../../Submodules/easyloggingpp/src)
include_directories(../../Submodules/qt-solutions/qtservice/src)

qt5_add_resources(QRC_SOURCES ${RESOURCES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${QRC_SOURCES})
target_link_libraries(${PROJECT_NAME} SoapServer Qt5::Core QtSolutions_Service Common)

copy_file_to_target_dir("${PROJECT_SOURCE_DIR}/Core/configs/${APP_CONFIG_FILE}" ${PROJECT_NAME})
copy_file_to_target_dir("${CMAKE_CURRENT_BINARY_DIR}/../configs/${LOG_CONFIG_FILE_NAME}" ${PROJECT_NAME})
