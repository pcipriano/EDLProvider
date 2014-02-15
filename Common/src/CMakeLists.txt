if(COMMON_QT)
    set(SOURCES_QT_DEPENDS
            PathAppender.cpp
            ConfigurationReader.cpp
       )

    set(HEADERS_QT_DEPENDS
            PathAppender.h
            ConfigurationReader.h
            ConfigurationReaderImpl.h
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

include_directories(../../Submodules/easyloggingpp/src)

add_library(Common ${SOURCES} ${HEADERS} ${SOURCES_QT_DEPENDS} ${HEADERS_QT_DEPENDS})
if(COMMON_QT)
    target_link_libraries(Common Qt5::Core Qt5::Xml Qt5::XmlPatterns)
endif()
