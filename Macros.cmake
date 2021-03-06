macro(copy_file_to_target_dir file target)
    add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} $<TARGET_FILE_DIR:${target}>
            COMMENT "Copying file ${file} to $<TARGET_FILE_DIR:${target}>."
            VERBATIM
        )
endmacro()

macro(copy_file_to_destination file destination)
    if (NOT TARGET CopyFiles)
        add_custom_target(CopyFiles ALL COMMENT "Copies files to specified destinations")
    endif()

    add_custom_command(TARGET CopyFiles
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} ${destination}
                       COMMENT "Copying ${file} to ${destination}"
                       VERBATIM
        )
endmacro()

macro(set_log_path folder)
    set(LOG_FILES_FOLDER "${folder}")

    if(WIN32)
        set(LOG_FILES_FOLDER_PATH "${LOG_FILES_FOLDER}")
    elseif(APPLE)
        set(LOG_FILES_FOLDER_PATH "")
    else()
        set(LOG_FILES_FOLDER_PATH "/var/log/${LOG_FILES_FOLDER}")
    endif()
endmacro()

macro(set_install_defaults)
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@loader_path")
        set(EXECUTABLE "${PROJECT_NAME}.app")
        set(BIN_INSTALL_DIR ".")
        set(DOC_INSTALL_DIR "${BIN_INSTALL_DIR}/${EXECUTABLE}/Contents/SharedSupport")
        set(BIN_INSTALL_DIR_PLUGINS "${EXECUTABLE}/Contents/PlugIns")
        set(BIN_INSTALL_DIR_RESOURCES "${BIN_INSTALL_DIR}/${EXECUTABLE}/Contents/Resources")
        set(BIN_INSTALL_DIR_FRAMEWORKS "${BIN_INSTALL_DIR}/${EXECUTABLE}/Contents/Frameworks")
    elseif(UNIX)
        set(CMAKE_INSTALL_RPATH "\$ORIGIN")
        set(BIN_INSTALL_DIR "lib/${PROJECT_NAME_LOWER}")
        set(EXECUTABLE "${BIN_INSTALL_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
        set(DOC_INSTALL_DIR "share/doc/${PROJECT_NAME_LOWER}/")
        set(BIN_INSTALL_DIR_PLUGINS "${BIN_INSTALL_DIR}/plugins")
        set(BIN_INSTALL_DIR_RESOURCES "${BIN_INSTALL_DIR}")
    else()
        set(EXECUTABLE "${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
        set(BIN_INSTALL_DIR ".")
        set(DOC_INSTALL_DIR ".")
        set(BIN_INSTALL_DIR_PLUGINS "plugins")
        set(BIN_INSTALL_DIR_RESOURCES "${BIN_INSTALL_DIR}")
    endif()
endmacro()
