list(APPEND CPACK_MODULE_PATH "${PROJECT_BINARY_DIR}/Installer")

set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME_UPPER}_VERSION_V}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME_UPPER}_VERSION_I}")
set(CPACK_PACKAGE_VERSION_PATCH "${${PROJECT_NAME_UPPER}_VERSION_P}")
set(CPACK_PACKAGE_VERSION "${${PROJECT_NAME_UPPER}_VERSION}")
set(CPACK_PACKAGE_VENDOR "None")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Web service for generating EDLs of various types.")

set(CPACK_STRIP_FILES ON)
set(CPACK_GENERATOR "ZIP")

if(WIN32)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
#    set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/License.rtf")
    set(CPACK_WIX_UPGRADE_GUID "8BDE5863-4FBC-4891-B04C-89459E02BEFD")
    set(CPACK_WIX_PATCH_FILE "${PROJECT_BINARY_DIR}/Installer/wixpatch.xml")
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_EXTENSIONS "WixUtilExtension" "WixFirewallExtension" "${CMAKE_CURRENT_LIST_DIR}/windows/WixCommonUiExtension.dll")
    set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_CURRENT_LIST_DIR}/windows/WixCustomUI.wxs")
elseif(APPLE)
    set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
    set(CMAKE_INSTALL_PREFIX "/")
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_SYSTEM_NAME "OSX")
elseif(UNIX)
    set(CPACK_SET_DESTDIR ON FORCE)
    set(CMAKE_INSTALL_PREFIX "/usr")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_SECTION "misc")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
    find_program(RPM_FOUND rpmbuild)
    if(RPM_FOUND)
        set(CPACK_GENERATOR "${CPACK_GENERATOR};RPM")
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "i686")
        set(CPACK_RPM_PACKAGE_GROUP "Applications/Productivity")
#        set(CPACK_RPM_PACKAGE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
        #LSB functions are required
        set(CPACK_RPM_PACKAGE_REQUIRES "lsb")
        set(CPACK_RPM_PRE_INSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/Installer/preinst")
        set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/Installer/prerm")
        set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/Installer/postinst")
        set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${PROJECT_BINARY_DIR}/Installer/postrm")
    endif()
    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_BINARY_DIR}/Installer/preinst;${PROJECT_BINARY_DIR}/Installer/prerm;${PROJECT_BINARY_DIR}/Installer/postinst;${PROJECT_BINARY_DIR}/Installer/postrm")
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${BIN_INSTALL_DIR}")
include(InstallRequiredSystemLibraries)

if(WIN32)
    #Configure the wix template
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/windows/WIX.template.wxs"
        "${PROJECT_BINARY_DIR}/Installer/WIX.template.in"
        @ONLY
    )

    #Configure the wix patch file
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/windows/wixpatch.xml.in"
        "${PROJECT_BINARY_DIR}/Installer/wixpatch.xml"
    )
elseif(UNIX AND NOT APPLE)
    #Configure the linux init script
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/linux/${PROJECT_NAME_LOWER}.in"
        "${PROJECT_BINARY_DIR}/Installer/${PROJECT_NAME_LOWER}"
    )

    #Configure all the post/pre install scripts used in rpm and deb files
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/linux/postinst.in"
        "${PROJECT_BINARY_DIR}/Installer/postinst"
    )

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/linux/postrm.in"
        "${PROJECT_BINARY_DIR}/Installer/postrm"
    )

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/linux/preinst.in"
        "${PROJECT_BINARY_DIR}/Installer/preinst"
    )

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/linux/prerm.in"
        "${PROJECT_BINARY_DIR}/Installer/prerm"
    )
endif()

include(CPack)

cpack_add_component(FinalCut
    DISPLAY_NAME "Final Cut Pro XML"
    DESCRIPTION "Final Cut Pro XML Interchange Format"
)
