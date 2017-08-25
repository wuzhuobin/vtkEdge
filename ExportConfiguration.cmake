##=============================================================================
##   This file is part of VTKEdge. See vtkedge.org for more information.
##
##   Copyright (c) 2010 Kitware, Inc.
##
##   VTKEdge may be used under the terms of the BSD License
##   Please see the file Copyright.txt in the root directory of
##   VTKEdge for further information.
##
##   Alternatively, you may see: 
##
##   http://www.vtkedge.org/vtkedge/project/license.html
##
##
##   For custom extensions, consulting services, or training, please
##   this or any other Kitware supported open source project, please
##   contact Kitware at sales@kitware.com.
##
##
##=============================================================================

# Generate the VTKEdgeConfig.cmake file in the build tree. Also configure
# one for installation. The file tells external projects how to use
# VTKEdge.

# Help store a literal dollar in a string.  CMake 2.2 allows escaped
# dollars but we have to support CMake 2.0.
set(DOLLAR "$")

#-----------------------------------------------------------------------------
# Settings shared between the build tree and install tree.

export_library_dependencies(
  ${VTKEdge_BINARY_DIR}/VTKEdgeLibraryDepends.cmake)
include(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
cmake_export_build_settings(
  ${VTKEdge_BINARY_DIR}/VTKEdgeBuildSettings.cmake)

if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
  install_files(${VTKEdge_INSTALL_PACKAGE_DIR} FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/UseVTKEdge.cmake
    ${VTKEdge_BINARY_DIR}/VTKEdgeLibraryDepends.cmake
    ${VTKEdge_BINARY_DIR}/VTKEdgeBuildSettings.cmake
    )
endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# The install-only section is empty for the build tree.
set(VTKEdge_CONFIG_INSTALL_ONLY)

# The "use" file.
set(VTKEdge_USE_FILE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/UseVTKEdge.cmake)

# The build settings file.
set(VTKEdge_BUILD_SETTINGS_FILE_CONFIG
  ${VTKEdge_BINARY_DIR}/VTKEdgeBuildSettings.cmake)

# The library directories.
set(VTKEdge_LIBRARY_DIRS_CONFIG ${VTKEdge_LIBRARY_DIRS})

# The runtime directories.
set(VTKEdge_RUNTIME_DIRS_CONFIG ${VTKEdge_RUNTIME_DIRS})

# The include directories.
set(VTKEdge_INCLUDE_DIRS_CONFIG ${VTKEdge_INCLUDE_PATH})

# The library dependencies file.
set(VTKEdge_LIBRARY_DEPENDS_FILE
  ${VTKEdge_BINARY_DIR}/VTKEdgeLibraryDepends.cmake)

# The CMake macros dir.
set(VTKEdge_CMAKE_DIR_CONFIG ${VTKEdge_CMAKE_DIR})

# The VTK options.
set(VTKEdge_VTK_DIR_CONFIG ${VTK_DIR})

# The build configuration information.
set(VTKEdge_CONFIGURATION_TYPES_CONFIG ${VTKEdge_CONFIGURATION_TYPES})
set(VTKEdge_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

# Configure VTKEdgeConfig.cmake for the build tree.
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/VTKEdgeConfig.cmake.in
  ${VTKEdge_BINARY_DIR}/VTKEdgeConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# The "use" file.
set(VTKEdge_USE_FILE_CONFIG
  ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_PACKAGE_DIR}/UseVTKEdge.cmake)

# The build settings file.
set(VTKEdge_BUILD_SETTINGS_FILE_CONFIG
  ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_PACKAGE_DIR}/VTKEdgeBuildSettings.cmake)

# The library directories.
if(CYGWIN AND VTKEdge_BUILD_SHARED_LIBS)
  # In Cygwin programs directly link to the .dll files.
  set(VTKEdge_LIBRARY_DIRS_CONFIG
    ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_BIN_DIR})
else(CYGWIN AND VTKEdge_BUILD_SHARED_LIBS)
  set(VTKEdge_LIBRARY_DIRS_CONFIG
    ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_LIB_DIR})
endif(CYGWIN AND VTKEdge_BUILD_SHARED_LIBS)

# The runtime directories.
if(WIN32)
  set(VTKEdge_RUNTIME_DIRS_CONFIG
    ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_BIN_DIR})
else(WIN32)
  set(VTKEdge_RUNTIME_DIRS_CONFIG
    ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_LIB_DIR})
endif(WIN32)

# The include directories.
set(VTKEdge_INCLUDE_DIRS_CONFIG
  ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_INCLUDE_DIR})

# The library dependencies file.
set(VTKEdge_LIBRARY_DEPENDS_FILE
  ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_PACKAGE_DIR}/VTKEdgeLibraryDepends.cmake)

# The CMake macros dir.
set(VTKEdge_CMAKE_DIR_CONFIG
  ${DOLLAR}{VTKEdge_INSTALL_PREFIX}${VTKEdge_INSTALL_PACKAGE_DIR}/CMake)

# The VTK options.
# If VTK_BINARY_DIR is set, then VTKEdge is part of a larger parent project
# that also builds VTK. In that case, VTKEdge_VTK_INSTALL_PACKAGE_DIR
# should be set to the value of VTK_INSTALL_PACKAGE_DIR. Otherwise, if not set,
# then we are using an outside VTK whose location will not change when we
# install.
if(VTK_BINARY_DIR)
  if(NOT VTKEdge_VTK_INSTALL_PACKAGE_DIR)
    message(
      "VTKEdge_VTK_INSTALL_PACKAGE_DIR must be set by the parent project to the value of VTK_INSTALL_PACKAGE_DIR it uses to configure VTK.")
  endif(NOT VTKEdge_VTK_INSTALL_PACKAGE_DIR)
  set(VTKEdge_VTK_DIR_CONFIG ${VTKEdge_VTK_INSTALL_PACKAGE_DIR})
else(VTK_BINARY_DIR)
  set(VTKEdge_VTK_DIR_CONFIG ${VTK_DIR})
endif(VTK_BINARY_DIR)

# The build configuration information.
# The install tree only has one configuration.
set(VTKEdge_CONFIGURATION_TYPES_CONFIG)

# Configure VTKEdgeConfig.cmake for the install tree.

# Construct the proper number of GET_FILENAME_COMPONENT(... PATH)
# calls to compute the installation prefix from VTKEdge_DIR.
string(REGEX REPLACE "/" ";" VTKEdge_INSTALL_PACKAGE_DIR_COUNT
  "${VTKEdge_INSTALL_PACKAGE_DIR}")
set(VTKEdge_CONFIG_INSTALL_ONLY "
# Compute the installation prefix from VTKEdge_DIR.
set(VTKEdge_INSTALL_PREFIX \"${DOLLAR}{VTKEdge_DIR}\")
")
foreach(p ${VTKEdge_INSTALL_PACKAGE_DIR_COUNT})
  set(VTKEdge_CONFIG_INSTALL_ONLY
    "${VTKEdge_CONFIG_INSTALL_ONLY}GET_FILENAME_COMPONENT(VTKEdge_INSTALL_PREFIX \"${DOLLAR}{VTKEdge_INSTALL_PREFIX}\" PATH)\n"
    )
endforeach(p)

if(CMAKE_CONFIGURATION_TYPES)
  # There are multiple build configurations.  Configure one
  # VTKEdgeConfig.cmake for each configuration.
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    set(VTKEdge_BUILD_TYPE_CONFIG ${config})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/VTKEdgeConfig.cmake.in
      ${VTKEdge_BINARY_DIR}/Install/${config}/VTKEdgeConfig.cmake
      @ONLY IMMEDIATE)
  endforeach(config)

  # Install the config file corresponding to the build configuration
  # specified when building the install target.  The BUILD_TYPE variable
  # will be set while CMake is processing the install files.
  if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
    install_files(${VTKEdge_INSTALL_PACKAGE_DIR} FILES
      ${VTKEdge_BINARY_DIR}/Install/${DOLLAR}{BUILD_TYPE}/VTKEdgeConfig.cmake)
  endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
else(CMAKE_CONFIGURATION_TYPES)
  # There is only one build configuration. Configure one VTKEdgeConfig.cmake.
  set(VTKEdge_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/VTKEdgeConfig.cmake.in
    ${VTKEdge_BINARY_DIR}/Install/VTKEdgeConfig.cmake @ONLY IMMEDIATE)

  # Setup an install rule for the config file.
  if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
    install_files(${VTKEdge_INSTALL_PACKAGE_DIR} FILES
      ${VTKEdge_BINARY_DIR}/Install/VTKEdgeConfig.cmake)
  endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
endif(CMAKE_CONFIGURATION_TYPES)
