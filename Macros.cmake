macro(copy_file_to_target_dir file target)
    add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${file} $<TARGET_FILE_DIR:${target}>
            COMMENT "Copying file ${file} to $<TARGET_FILE_DIR:${target}>."
            VERBATIM
        )
endmacro()
