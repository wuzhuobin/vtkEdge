##=============================================================================
##   This file is part of VTKEdge. See vtkedge.org for more information.
##
##   Copyright (c) 2008 Kitware, Inc.
##
##   VTKEdge may be used under the terms of the GNU General Public License 
##   version 3 as published by the Free Software Foundation and appearing in 
##   the file LICENSE.txt included in the top level directory of this source
##   code distribution. Alternatively you may (at your option) use any later 
##   version of the GNU General Public License if such license has been 
##   publicly approved by Kitware, Inc. (or its successors, if any).
##
##   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
##   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
##   PURPOSE. See LICENSE.txt for additional details.
##
##   VTKEdge is available under alternative license terms. Please visit
##   vtkedge.org or contact us at kitware@kitware.com for further information.
##
##=============================================================================


# -----------------------------------------------------------------------------
# Set the minimum required cmake version. Setting here to 2.4, but really 
# 2.7 is required for testing with bullseye coverage. Will change to 2.8 once
# it is officially released
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.4)
project(VTKEdge)

# -----------------------------------------------------------------------------
# Set cmake policy CMP0003 to NEW. This means cmake will not break up linked
# libraries into separate linker search paths and library names. 
# -----------------------------------------------------------------------------
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)

# -----------------------------------------------------------------------------
# VTKEdge version number.  An even minor number corresponds to a release
# -----------------------------------------------------------------------------
set(VTKEdge_VERSION_MAJOR 0)
set(VTKEdge_VERSION_MINOR 1)
set(VTKEdge_VERSION_BUILD 0)
set(VTKEdge_VERSION
  "${VTKEdge_VERSION_MAJOR}.${VTKEdge_VERSION_MINOR}.${VTKEdge_VERSION_BUILD}")

# -----------------------------------------------------------------------------
# Setup the output paths
# Remark:
# Make sure this section appears before FIND_PACKAGE(VTK) statement.
# In case FIND_PACKAGE(VTK) statement is used before the following lines and
# fails for any reason, EXECUTABLE_OUTPUT_PATH is set to empty, as it is a
# cached variable there would be no change it takes ${PROJECT_BINARY_DIR}/bin
# as its default value.
# Reminder:
# EXECUTABLE_OUTPUT_PATH is no longer used in Cmake 2.6. The day the minimum
# requirement moves from 2.4 to 2.6, it should be replaced by the use of
# RUNTIME_OUTPUT_DIRECTORY target property.
# -----------------------------------------------------------------------------
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
  "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
  "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

set(VTKEdge_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})
set(VTKEdge_RUNTIME_DIRS ${LIBRARY_OUTPUT_PATH})

# Set CXX_TEST_PATH, bullet-proof to empty EXECUTABLE_OUTPUT_PATH.
if(EXECUTABLE_OUTPUT_PATH)
  set(CXX_TEST_PATH "${EXECUTABLE_OUTPUT_PATH}")
else(EXECUTABLE_OUTPUT_PATH)
  set(CXX_TEST_PATH ".")
endif(EXECUTABLE_OUTPUT_PATH)

# -----------------------------------------------------------------------------
# Disable deprecation warnings for standard C and STL functions in VS2005 and
# later (no, we don't need IF(CMAKE_COMPILER_2005) ... )
# -----------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
add_definitions(-D_SCL_SECURE_NO_DEPRECATE)

# -----------------------------------------------------------------------------
# Bring our CMake modules to the CMake path
# -----------------------------------------------------------------------------
set(VTKEdge_CMAKE_DIR "${VTKEdge_SOURCE_DIR}/CMake" CACHE INTERNAL "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${VTKEdge_CMAKE_DIR}")
add_subdirectory(CMake)

# -----------------------------------------------------------------------------
# Setup the installation settings
# -----------------------------------------------------------------------------

set(VTKEdge_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})

set(VTK_INSTALL_NO_DEVELOPMENT ON) # why?

if(NOT VTKEdge_INSTALL_BIN_DIR)
  set(VTKEdge_INSTALL_BIN_DIR "/bin")
endif(NOT VTKEdge_INSTALL_BIN_DIR)

if(NOT VTKEdge_INSTALL_LIB_DIR)
  set(VTKEdge_INSTALL_LIB_DIR "/lib/${PROJECT_NAME}")
endif(NOT VTKEdge_INSTALL_LIB_DIR)

if(NOT VTKEdge_INSTALL_DATA_DIR)
  set(VTKEdge_INSTALL_DATA_DIR "/share/${PROJECT_NAME}" CACHE INTERNAL "")
endif(NOT VTKEdge_INSTALL_DATA_DIR)

if(NOT VTKEdge_INSTALL_INCLUDE_DIR)
  set(VTKEdge_INSTALL_INCLUDE_DIR "/include/${PROJECT_NAME}")
endif(NOT VTKEdge_INSTALL_INCLUDE_DIR)

if(NOT VTKEdge_INSTALL_PACKAGE_DIR)
  set(VTKEdge_INSTALL_PACKAGE_DIR ${VTKEdge_INSTALL_LIB_DIR} CACHE INTERNAL "")
endif(NOT VTKEdge_INSTALL_PACKAGE_DIR)

if(NOT VTKEdge_VTK_INSTALL_PACKAGE_DIR)
  set(VTKEdge_VTK_INSTALL_PACKAGE_DIR ${VTK_INSTALL_PACKAGE_DIR})
endif(NOT VTKEdge_VTK_INSTALL_PACKAGE_DIR)

if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
  set(VTKEdge_INSTALL_NO_DEVELOPMENT 0)
endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)

if(NOT VTKEdge_INSTALL_NO_RUNTIME)
  set(VTKEdge_INSTALL_NO_RUNTIME 0)
endif(NOT VTKEdge_INSTALL_NO_RUNTIME)

if(NOT VTKEdge_INSTALL_NO_DOCUMENTATION)
  set(VTKEdge_INSTALL_NO_DOCUMENTATION 0)
endif(NOT VTKEdge_INSTALL_NO_DOCUMENTATION)

set(VTKEdge_INSTALL_NO_LIBRARIES)
if(VTKEdge_BUILD_SHARED_LIBS)
  if(VTKEdge_INSTALL_NO_RUNTIME AND VTKEdge_INSTALL_NO_DEVELOPMENT)
    set(VTKEdge_INSTALL_NO_LIBRARIES 1)
  endif(VTKEdge_INSTALL_NO_RUNTIME AND VTKEdge_INSTALL_NO_DEVELOPMENT)
else(VTKEdge_BUILD_SHARED_LIBS)
  if(VTKEdge_INSTALL_NO_DEVELOPMENT)
    set(VTKEdge_INSTALL_NO_LIBRARIES 1)
  endif(VTKEdge_INSTALL_NO_DEVELOPMENT)
endif(VTKEdge_BUILD_SHARED_LIBS)

# -----------------------------------------------------------------------------
# Include CTest for testing
# -----------------------------------------------------------------------------
include(CTest)

# -----------------------------------------------------------------------------
# Add in the option for building examples, default to ON
# If BUILD_EXAMPLES was set by a larger project, use it.
# -----------------------------------------------------------------------------
option(VTKEdge_BUILD_EXAMPLES "Build VTKEdge examples." ${BUILD_EXAMPLES})

# -----------------------------------------------------------------------------
# Find VTK - it is a required package
# -----------------------------------------------------------------------------
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


# -----------------------------------------------------------------------------
# Find ITK - it is an optional package that if present allows the usage of 
#            complex paintbrush operations in Widgets/ such as flood fill
#            paintbrushes etc.
# If you are using VTKEdge as a subproject within your project, and you want 
# to enable/disable the use of ITK, just set VTKEdge_USE_ITK_DEFAULT from your
# top level directory
# -----------------------------------------------------------------------------
SET(default_val 0)
IF(DEFINED VTKEdge_USE_ITK_DEFAULT)
  SET(default_val ${VTKEdge_USE_ITK_DEFAULT})
ENDIF(DEFINED VTKEdge_USE_ITK_DEFAULT)
OPTION(VTKEdge_USE_ITK 
  "Build filtered brush widgets that require ITK" ${default_val})
MARK_AS_ADVANCED(VTKEdge_USE_ITK)
IF(VTKEdge_USE_ITK)
  IF(NOT ITK_SOURCE_DIR)
    FIND_PACKAGE(ITK REQUIRED)
    IF(ITK_FOUND)
      INCLUDE(${ITK_USE_FILE})
    ENDIF(ITK_FOUND)
  ENDIF(NOT ITK_SOURCE_DIR)
ENDIF(VTKEdge_USE_ITK)


# -----------------------------------------------------------------------------
# Find CUDA
# -----------------------------------------------------------------------------
option(VTKEdge_USE_CUDA "Use NVIDIAs CUDA library." OFF)
if(VTKEdge_USE_CUDA)
  find_package(Cuda)
endif(VTKEdge_USE_CUDA)

# -----------------------------------------------------------------------------
# Add an option for building shared - default to the same choice made for VTK
# BUILD_SHARED_LIBS is a builtin cmake variable.
# -----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build with shared libraries."
  ${VTK_BUILD_SHARED_LIBS})

# Set VTKEdge_BUILD_SHARED_LIBS to 1 or 0 for use in creating the
# configuration header.
if(BUILD_SHARED_LIBS)
  set(VTKEdge_BUILD_SHARED_LIBS 1)
else(BUILD_SHARED_LIBS)
  set(VTKEdge_BUILD_SHARED_LIBS 0)
endif(BUILD_SHARED_LIBS)

# -----------------------------------------------------------------------------
# Configure VTK_DATA_ROOT for the location of VTKData. VTKData is required for
# some examples and tests. Check this directory, up one, and up two.
# -----------------------------------------------------------------------------
find_path(VTK_DATA_ROOT VTKData.readme
  ${VTKEdge_SOURCE_DIR}/VTKData
  ${VTKEdge_SOURCE_DIR}/../VTKData
  ${VTKEdge_SOURCE_DIR}/../../VTKData
  ${VTKEdge_SOURCE_DIR}/../VTK/VTKData
  ${VTKEdge_SOURCE_DIR}/../../VTK/VTKData
  $ENV{VTK_DATA_ROOT}
  )

# -----------------------------------------------------------------------------
# Enable kits
# -----------------------------------------------------------------------------
set(VTKEdge_AVAILABLE_KITS
  Common
  Filtering
  Graphics
  Hybrid
  Imaging
  IO
  Rendering
  VolumeRendering
  Widgets
  )
set(VTKEdge_ENABLED_KITS)
set(VTKEdge_RELATIVE_INCLUDE_DIRS)

set(libraries)
set(include_paths)

foreach(kit ${VTKEdge_AVAILABLE_KITS})
  if(DEFINED VTKEdge_BUILD_${kit}_KIT_DEFAULT)
    set(default_val ${VTKEdge_BUILD_${kit}_KIT_DEFAULT})
  else(DEFINED VTKEdge_BUILD_${kit}_KIT_DEFAULT)
    set(default_val 1)
  endif(DEFINED VTKEdge_BUILD_${kit}_KIT_DEFAULT)
  option(VTKEdge_BUILD_${kit}_KIT "Build VTKEdge ${kit} kit." ${default_val})
  mark_as_advanced(VTKEdge_BUILD_${kit}_KIT)
  if(VTKEdge_BUILD_${kit}_KIT)
    set(VTKEdge_ENABLED_KITS ${VTKEdge_ENABLED_KITS} ${kit})
    set(libraries ${libraries} vtkKWE${kit})
    set(VTKEdge_RELATIVE_INCLUDE_DIRS ${VTKEdge_RELATIVE_INCLUDE_DIRS} ${kit})
    set(include_paths ${include_paths} ${VTKEdge_SOURCE_DIR}/${kit})
  endif(VTKEdge_BUILD_${kit}_KIT)
endforeach(kit)

set(include_paths ${include_paths} ${PROJECT_BINARY_DIR})

set(VTKEdge_INCLUDE_PATH ${include_paths} CACHE INTERNAL 
  "Include paths for VTKEdge")

set(VTKEdge_LIBRARIES ${libraries} CACHE INTERNAL 
  "Libraries for VTKEdge")

include_directories(${VTKEdge_INCLUDE_PATH})

find_package(DirectX QUIET)
if(DirectX_FOUND)
 set(VTKEdge_USE_DIRECTX 1)
endif(DirectX_FOUND)

find_package(ApplicationServices)
find_package(IOKit)
if(ApplicationServices_FOUND AND IOKit_FOUND)
 set(VTKEdge_USE_CORE_GRAPHICS 1)
endif(ApplicationServices_FOUND AND IOKit_FOUND)

if(VTK_USE_X)
 option(VTKEdge_USE_NVCONTROL "Use NVIDIAs X server extension NVCONTROL." OFF)
 if(VTKEdge_USE_NVCONTROL)
  find_package(NVCtrlLib)
 endif(VTKEdge_USE_NVCONTROL)
endif(VTK_USE_X)

# Find the VTK_VERSION_DATE. The widgets have code so as to deal with changes
# in VTK both before and after the Depth peeling compatibility changes.
FIND_FILE(VTK_VERSION_FILENAME "vtkVersion.h" ${VTK_INCLUDE_DIRS})
MARK_AS_ADVANCED(VTK_VERSION_FILENAME)
FILE(READ ${VTK_VERSION_FILENAME} VTK_VERSION_FILE)
FIND_FILE(VTK_DATE_STAMP_FILENAME "vtksys/DateStamp.h" ${VTK_INCLUDE_DIRS})
IF(EXISTS ${VTK_DATE_STAMP_FILENAME})
  FILE(READ ${VTK_DATE_STAMP_FILENAME} VTK_DATE_STAMP_FILE)
  SET( VTK_VERSION_FILE ${VTK_VERSION_FILE} ${VTK_DATE_STAMP_FILE})
ENDIF(EXISTS ${VTK_DATE_STAMP_FILENAME})
STRING(REGEX MATCH "[0-9][0-9][0-9][0-9][/|-][0-9][0-9][/|-][0-9][0-9]"
  VTKEdge_VTK_VERSION_DATE ${VTK_VERSION_FILE})
STRING(REGEX REPLACE "[/|-]" "" VTKEdge_VTK_VERSION_DATE ${VTKEdge_VTK_VERSION_DATE})

# -----------------------------------------------------------------------------
# The configured header file used to pass CMake settings to the source code.
# DO NOT DEFINE ANY OPTION AFTER THIS SECTION
# Here we configure the .in file to form the .h, that is then stored in the
# binary directory - so we'll need to add that in the include path
# As the config file depends on VTKEdge_BUILD_SHARED_LIBS and VTKedge version
# number, make sure to place this statement after them.
# -----------------------------------------------------------------------------
configure_file(
  "${PROJECT_SOURCE_DIR}/VTKEdgeConfigure.h.in"
  "${PROJECT_BINARY_DIR}/VTKEdgeConfigure.h"
  )

if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
  install_files(${VTKEdge_INSTALL_INCLUDE_DIR} FILES 
    ${VTKEdge_BINARY_DIR}/VTKEdgeConfigure.h)
endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)

# -----------------------------------------------------------------------------
# Dashboard options.
# -----------------------------------------------------------------------------
configure_file(${VTKEdge_CMAKE_DIR}/CTestCustom.cmake.in
               ${PROJECT_BINARY_DIR}/CTestCustom.cmake @ONLY)

# -----------------------------------------------------------------------------
# Add each kit's directory
# DO NOT DEFINE ANY OPTION AFTER THIS SECTION THAT COULD BE USED
# BY A KIT (add_subdirectory is executed *now*)
# -----------------------------------------------------------------------------
foreach(kit ${VTKEdge_ENABLED_KITS})
  add_subdirectory(${kit})
endforeach(kit)

# -----------------------------------------------------------------------------
# Testing
# -----------------------------------------------------------------------------
if(BUILD_TESTING)
  make_directory(${PROJECT_BINARY_DIR}/Testing/Temporary)
endif(BUILD_TESTING)

# -----------------------------------------------------------------------------
# Build the examples if they are turned on
# -----------------------------------------------------------------------------
if(VTKEdge_BUILD_EXAMPLES)
  set(VTKEdge_EXAMPLE_EXECUTABLE_PREFIX ${PROJECT_NAME})
  add_subdirectory(Examples)
endif(VTKEdge_BUILD_EXAMPLES)

# -----------------------------------------------------------------------------
# Build the documentation
# -----------------------------------------------------------------------------
add_subdirectory(Utilities/Doxygen)

# -----------------------------------------------------------------------------
# Build the Paraview plugins
# -----------------------------------------------------------------------------
add_subdirectory(ParaViewPlugins)

# --------------------------------------------------------------------------
# Configure the export configuration files
# --------------------------------------------------------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/ExportConfiguration.cmake)


