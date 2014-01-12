set(SOURCES
        Submodules/qt-solutions/qtservice/src/qtservice.cpp
        )

set(HEADERS
        Submodules/qt-solutions/qtservice/src/qtservice.h
        Submodules/qt-solutions/qtservice/src/qtservice_p.h
        )

if(WIN32)
    set(EXTRA_INCLUDES
            Submodules/qt-solutions/qtservice/src/qtservice_win.cpp
        )
    set(EXTRA_LIBS
            user32
        )
elseif(UNIX)
    set(EXTRA_INCLUDES
            Submodules/qt-solutions/qtservice/src/qtunixsocket.h
            Submodules/qt-solutions/qtservice/src/qtunixserversocket.h
            Submodules/qt-solutions/qtservice/src/qtservice_unix.cpp
            Submodules/qt-solutions/qtservice/src/qtunixsocket.cpp
            Submodules/qt-solutions/qtservice/src/qtunixserversocket.cpp
        )
    set(EXTRA_LIBS
            Qt5::Network
        )
endif()

add_library(QtSolutions_Service ${SOURCES} ${HEADERS} ${EXTRA_INCLUDES})
target_link_libraries(QtSolutions_Service Qt5::Core ${EXTRA_LIBS})
