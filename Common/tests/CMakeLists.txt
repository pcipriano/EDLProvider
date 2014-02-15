set(SOURCES
        Main.cpp
        PimplTest.cpp
        PathAppenderTest.cpp
        ConfigurationReaderTest.cpp
   )

set(HEADERS
        AutoTest.h
        PimplTest.h
        PathAppenderTest.h
        ConfigurationReaderTest.h
   )

include_directories(../src)
include_directories(../../Submodules/easyloggingpp/src)

add_executable(CommonTest ${SOURCES} ${HEADERS})
target_link_libraries(CommonTest Common Qt5::Test)

add_test(Common ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CommonTest)
