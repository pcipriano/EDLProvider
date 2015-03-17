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
#   AAF_LIBRARY - AAF library to link to
#   AAFIID_LIBRARY - AAFIID library to link to
#   AAF_MODULE - AAF dynamic library

if(WIN32)
    set(AAF_LIB_PLATFORM_DIR "${CMAKE_CURRENT_LIST_DIR}/win32")
elseif(APPLE)
    #To add support in the future
else()
    set(AAF_LIB_PLATFORM_DIR "${CMAKE_CURRENT_LIST_DIR}/linux")
endif()

find_path(AAF_INCLUDE_DIR NAMES "AAF.h"
    PATHS ${AAF_LIB_PLATFORM_DIR}
    PATH_SUFFIXES "include"
  )

find_library(AAF_LIBRARY NAMES AAF aaflib
    PATHS ${AAF_LIB_PLATFORM_DIR}
  )

find_library(AAFIID_LIBRARY NAMES AAFIID aafiid
    PATHS ${AAF_LIB_PLATFORM_DIR}
  )

find_library(AAF_MODULE NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}com-api${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_SHARED_LIBRARY_PREFIX}AAFCOAPI${CMAKE_SHARED_LIBRARY_SUFFIX}
    PATHS ${AAF_LIB_PLATFORM_DIR}
  )

if(WIN32)
    set(AAF_MODULE ${AAF_LIB_PLATFORM_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}AAFCOAPI${CMAKE_SHARED_LIBRARY_SUFFIX})
elseif(APPLE)
    #To add support in the future
else()
    set(AAF_MODULE ${AAF_LIB_PLATFORM_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}com-api${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AAF
    REQUIRED_VARS AAF_INCLUDE_DIR AAF_LIBRARY AAFIID_LIBRARY AAF_MODULE
  )

mark_as_advanced(AAF_INCLUDE_DIR
                 AAF_LIBRARY
                 AAFIID_LIBRARY
                 AAF_MODULE)
