project(EDLProvider)

# Build in Debug mode by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()

if(WIN32)
    cmake_minimum_required(VERSION 3.0.0)
else()
    cmake_minimum_required(VERSION 2.8.12)
endif()

set(PROJECT_USER_NAME "EDL Provider")
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

set(${PROJECT_NAME_UPPER}_VERSION_V 0)
set(${PROJECT_NAME_UPPER}_VERSION_I 0)
set(${PROJECT_NAME_UPPER}_VERSION_P 2)

set(${PROJECT_NAME_UPPER}_VERSION ${${PROJECT_NAME_UPPER}_VERSION_V}.${${PROJECT_NAME_UPPER}_VERSION_I}.${${PROJECT_NAME_UPPER}_VERSION_P})

message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME_UPPER}_VERSION} in ${CMAKE_BUILD_TYPE} mode")

#Logging defines
add_definitions(-DELPP_THREAD_SAFE -DELPP_FORCE_USE_STD_THREAD -DELPP_STACKTRACE_ON_CRASH -DELPP_STL_LOGGING -DELPP_QT_LOGGING)
set(LOG_CONFIG_FILE_NAME "logger.conf")

#Configuration file
set(APP_CONFIG_FILE "EdlProviderConfig.xml")

configure_file(
    "${PROJECT_SOURCE_DIR}/EDLProviderInfo.h.in"
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h.out"
    @ONLY
  )
configure_file(
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h.out"
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h"
  )

option(COMMON_QT "Compile common library with classes depending on Qt." ON)
if(UNIX AND NOT APPLE)
    option(TIME_UTILS_QT "Compile TimeUtils library with Qt dependency." ON)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libstdc++ -Wall -Wno-long-long -pedantic -Werror")
endif()

#Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#Enable automoc
set(CMAKE_AUTOMOC ON)
#Include the project binary dir
include_directories(${PROJECT_BINARY_DIR})

#Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5XmlPatterns REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Test QUIET REQUIRED)

get_target_property(QT_LOCATION Qt5::Core LOCATION)
get_filename_component(QT_LOCATION ${QT_LOCATION} DIRECTORY)
message("Qt found at [${QT_LOCATION}]")

#Doxygen
find_package(Doxygen)

#Specify output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/lib")

#Extra includes
include(Macros.cmake)
include(Submodules/qtservice.cmake)

#Set default install directories
set_install_defaults()

enable_testing()

#Directories
add_subdirectory(Core)
add_subdirectory(TimeUtils)
add_subdirectory(Soap)
add_subdirectory(Common)
add_subdirectory(Plugins)

#Doxygen
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY})
    add_custom_target(${PROJECT_NAME}Doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

include(Installer/CPackConfig.cmake)

