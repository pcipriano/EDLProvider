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

#Install files
install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_INSTALL_DIR})

#Ancillary files
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/../configs/${LOG_CONFIG_FILE_NAME}.deploy" DESTINATION ${BIN_INSTALL_DIR} RENAME ${LOG_CONFIG_FILE_NAME})
install(FILES "${PROJECT_SOURCE_DIR}/Core/configs/${APP_CONFIG_FILE}" DESTINATION ${BIN_INSTALL_DIR})

if(UNIX AND NOT APPLE)
    #Init daemon script install
    install(PROGRAMS "${PROJECT_BINARY_DIR}/Installer/${PROJECT_NAME_LOWER}" DESTINATION ../etc/init.d/)
endif()

install(CODE "
message(\"CPack install prefix [\${CMAKE_INSTALL_PREFIX}]\")

#Write qt conf file that is used to tell QtLibraries where to look for plugins
file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR_RESOURCES}/qt.conf\" \"\")

file(GLOB_RECURSE QTPLUGINS \"\${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR_PLUGINS}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

set(BU_CHMOD_BUNDLE_ITEMS ON)

include(BundleUtilities)
#Now fixup bundle
fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}\" \"\${QTPLUGINS}\" \"${QT_LOCATION}\")"
)
