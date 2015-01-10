set(SOURCES
        TimeCode.cpp
   )

set(HEADERS
        TimeCode.h
   )

add_library(TimeUtils ${SOURCES} ${HEADERS})

if(TIME_UTILS_QT)
    target_link_libraries(TimeUtils Qt5::Core)
    target_compile_definitions(TimeUtils PUBLIC "-DUSING_QT")
endif()
