set(SOURCES
        Main.cpp
        PathAppenderTest.cpp
   )

set(HEADERS
        AutoTest.h
        PathAppenderTest.h
   )

include_directories(../src)

add_executable(CommonTest ${SOURCES} ${HEADERS})
target_link_libraries(CommonTest Common Qt5::Test)

add_test(Common ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CommonTest)
