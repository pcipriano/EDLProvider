macro(copy_file_to_target_dir file target)
    add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} $<TARGET_FILE_DIR:${target}>
            COMMENT "Copying file ${file} to $<TARGET_FILE_DIR:${target}>."
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
