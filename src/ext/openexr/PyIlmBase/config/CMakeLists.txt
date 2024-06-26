# SPDX-License-Identifier: BSD-3-Clause
# Copyright Contributors to the OpenEXR Project.

### The autoconf setup for this folder generates a PyIlmBaseConfig.h file
### but no source actually uses that, so let's elide that for now

configure_file(PyIlmBaseConfigInternal.h.in_cmake ${CMAKE_CURRENT_BINARY_DIR}/PyIlmBaseConfigInternal.h)
add_library(PyIlmBaseConfig INTERFACE)
target_include_directories(PyIlmBaseConfig INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
install(TARGETS PyIlmBaseConfig EXPORT ${PROJECT_NAME})
add_library(PyIlmBase::Config ALIAS PyIlmBaseConfig)

if(PYILMBASE_INSTALL_PKG_CONFIG)
  # use a helper function to avoid variable pollution, but pretty simple
  function(pyilmbase_pkg_config_help pcinfile)
    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(exec_prefix ${CMAKE_INSTALL_BINDIR})
    set(libdir ${CMAKE_INSTALL_LIBDIR})
    set(includedir ${CMAKE_INSTALL_INCLUDEDIR})
    set(LIB_SUFFIX_DASH ${OPENEXR_LIB_SUFFIX})
    string(REPLACE ".in" "" pcout ${pcinfile})
    configure_file(${pcinfile} ${CMAKE_CURRENT_BINARY_DIR}/${pcout} @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${pcout}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
  endfunction()
  pyilmbase_pkg_config_help(../PyIlmBase.pc.in)
endif()

# The main export of the configuration - This is the
# moral equivalent of a pkg-config file for cmake
# and replaces the Find*.cmake of the "old" cmake
include(CMakePackageConfigHelpers)

write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PYILMBASE_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
install(EXPORT ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  FILE ${PROJECT_NAME}Config.cmake
  NAMESPACE ${PROJECT_NAME}::
  EXPORT_LINK_INTERFACE_LIBRARIES
)
