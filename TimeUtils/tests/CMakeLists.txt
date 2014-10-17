set(SOURCES
        Main.cpp
        TimeCodeTest.cpp
   )

set(HEADERS
        TimeCodeTest.h
   )

include_directories(../src)
include_directories(../../Common/tests)
include_directories(../../Submodules/easyloggingpp/src)

add_executable(TimeUtilsTest ${SOURCES} ${HEADERS})
target_link_libraries(TimeUtilsTest TimeUtils Qt5::Test)

add_test(TimeUtils ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TimeUtilsTest)
