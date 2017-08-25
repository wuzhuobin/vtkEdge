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

#
# This module is provided as VTKEdge_USE_FILE by VTKEdgeConfig.cmake.  
# It can be INCLUDEd in a project to load the needed compiler and linker
# settings to use VTKEdge:
#   find_package(VTKEdge REQUIRED)
#   include(${VTKEdge_USE_FILE})
#

if(NOT VTKEdge_USE_FILE_INCLUDED)
  set(VTKEdge_USE_FILE_INCLUDED 1)

  # Load the compiler settings used for VTKEdge.
  if(VTKEdge_BUILD_SETTINGS_FILE)
    include(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
    cmake_import_build_settings(${VTKEdge_BUILD_SETTINGS_FILE})
  endif(VTKEdge_BUILD_SETTINGS_FILE)

  # Add compiler flags needed to use VTKEdge.
  set(CMAKE_C_FLAGS 
    "${CMAKE_C_FLAGS} ${VTKEdge_REQUIRED_C_FLAGS}")
  set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} ${VTKEdge_REQUIRED_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} ${VTKEdge_REQUIRED_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS 
    "${CMAKE_SHARED_LINKER_FLAGS} ${VTKEdge_REQUIRED_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS 
    "${CMAKE_MODULE_LINKER_FLAGS} ${VTKEdge_REQUIRED_MODULE_LINKER_FLAGS}")

  # Add include directories needed to use VTKEdge.
  include_directories(${VTKEdge_INCLUDE_DIRS})

  # Add link directories needed to use VTKEdge.
  link_directories(${VTKEdge_LIBRARY_DIRS})

  # Add cmake module path.
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${VTKEdge_CMAKE_DIR}")

  # Use VTK.
  if(NOT VTKEdge_NO_USE_VTK)
    set(VTK_DIR ${VTKEdge_VTK_DIR})
    find_package(VTK)
    if(VTK_FOUND)
      include(${VTK_USE_FILE})
    else(VTK_FOUND)
      message("VTK not found in VTKEdge_VTK_DIR=\"${VTKEdge_VTK_DIR}\".")
    endif(VTK_FOUND)
  endif(NOT VTKEdge_NO_USE_VTK)

endif(NOT VTKEdge_USE_FILE_INCLUDED)
