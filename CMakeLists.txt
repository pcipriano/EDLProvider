project(EDLProvider)

cmake_minimum_required(VERSION 2.8.11)

set(PROJECT_USER_NAME "EDL Provider")
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

set(${PROJECT_NAME_UPPER}_VERSION_V 0)
set(${PROJECT_NAME_UPPER}_VERSION_I 0)
set(${PROJECT_NAME_UPPER}_VERSION_P 1)

set(${PROJECT_NAME_UPPER}_VERSION ${${PROJECT_NAME_UPPER}_VERSION_V}.${${PROJECT_NAME_UPPER}_VERSION_I}.${${PROJECT_NAME_UPPER}_VERSION_P})

message(STATUS "Building ${PROJECT_NAME} ${${PROJECT_NAME_UPPER}_VERSION} in ${CMAKE_BUILD_TYPE} mode")

configure_file(
    "${PROJECT_SOURCE_DIR}/EDLProviderInfo.h.in"
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h.out"
    @ONLY
  )
configure_file(
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h.out"
    "${PROJECT_BINARY_DIR}/EDLProviderInfo.h"
  )

if(MSVC)
  #Enable multi threaded in Visual Studio
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

#Doxygen
find_package(Doxygen)

#Directories
add_subdirectory(Core)

#Include directories
include(Submodules/qtservice.cmake)

#Specify output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/lib")

enable_testing()

#Doxygen
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY})
    add_custom_target(${PROJECT_NAME}Doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

