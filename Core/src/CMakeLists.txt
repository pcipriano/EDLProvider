set(SOURCES
        Main.cpp
        EDLProviderService.cpp
    )

set(HEADERS
        EDLProviderService.h
   )

include_directories(../../Submodules/qt-solutions/qtservice/src)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Core QtSolutions_Service)
