#.rst:
# FindAAF
# -----------
#
# Try to find the AAF(Advanced Authoring Format) libraries
#
# Once done this will define
#
# ::
#
#   AAF_FOUND - AAF found
#   AAF_INCLUDE_DIR - the AAF include directory
#   AAF_LIBRARIES - AAF libraries to link to
#   AAF_MODULE - AAF dynamic library

# If not tried to find aaf libraries set paths
set(AAF_LIB_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(WIN32)
    set(AAF_LIB_PLATFORM_DIR "${AAF_LIB_ROOT_DIR}/win32")
elseif(APPLE)
    #To add support in the future
else()
    set(AAF_LIB_PLATFORM_DIR "${AAF_LIB_ROOT_DIR}/linux")
endif()

find_path(AAF_INCLUDE_DIR NAMES "AAF.h"
    PATHS ${AAF_LIB_ROOT_DIR}
    PATH_SUFFIXES "include"
  )

find_library(AAF_LIBRARIES NAMES AAF AAFIID
    PATHS ${AAF_LIB_PLATFORM_DIR}
  )

find_library(AAF_MODULE NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}AAFCOAPI${CMAKE_SHARED_LIBRARY_SUFFIX}
    PATHS ${AAF_LIB_PLATFORM_DIR}
  )

set(AAF_MODULE ${AAF_LIB_PLATFORM_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}AAFCOAPI${CMAKE_SHARED_LIBRARY_SUFFIX})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AAF
    REQUIRED_VARS AAF_INCLUDE_DIR AAF_LIBRARIES AAF_MODULE
  )

mark_as_advanced(AAF_INCLUDE_DIR
                 AAF_LIBRARIES
                 AAF_MODULE)
