if(COMMON_QT)
    set(SOURCES_QT_DEPENDS
            PathAppender.cpp
       )

    set(HEADERS_QT_DEPENDS
            PathAppender.h
       )
endif()

set(SOURCES
   )

set(HEADERS
        Uncopyable.h
        Pimpl.h
        PimplImpl.h
        SingletonHelper.h
        SingletonHelperImpl.h
   )


add_library(Common ${SOURCES} ${HEADERS} ${SOURCES_QT_DEPENDS} ${HEADERS_QT_DEPENDS})
if(COMMON_QT)
    target_link_libraries(Common Qt5::Core)
endif()
