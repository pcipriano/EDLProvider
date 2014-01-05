project(EDLProvider)

cmake_minimum_required(VERSION 2.8.11)

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

find_package(Doxygen)

#Directories
add_subdirectory(Core)

#Doxygen
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY})
    add_custom_target(${PROJECT_NAME}Doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

