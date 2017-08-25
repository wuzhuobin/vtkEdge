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
# Set of basic source files
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWECommonSerializationHelper.cxx
  vtkKWESerializableObject.cxx
  vtkKWESerializationHelper.cxx
  vtkKWESerializationHelperMap.cxx
  vtkKWESerializer.cxx
  vtkKWEXMLArchiveReader.cxx
  vtkKWEXMLArchiveWriter.cxx
  vtkKWEXMLElement.cxx
  vtkKWEXMLParser.cxx
  )

set_source_files_properties(
  vtkKWESerializationHelper.cxx
  vtkKWESerializer.cxx
  ABSTRACT
)


# -----------------------------------------------------------------------------
# List the source files that should not be wrapped.
# -----------------------------------------------------------------------------
#SET_SOURCE_FILES_PROPERTIES(
#
#  WRAP_EXCLUDE)


# -----------------------------------------------------------------------------
# List the kits from VTK that are needed by this project
# -----------------------------------------------------------------------------
set(KIT_LIBS
  vtkKWECommon
  vtkIO
  )

# -----------------------------------------------------------------------------
# If building Rendering, add help for serialization
# -----------------------------------------------------------------------------
if(VTK_USE_RENDERING)
  set(KIT_EXTRA_SRCS ${KIT_EXTRA_SRCS}
    vtkKWERenderingSerializationHelper.cxx
  )
  set(KIT_LIBS ${KIT_LIBS} vtkRendering)
endif(VTK_USE_RENDERING)

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
add_library(vtkKWEIO ${KIT_SRCS} ${KIT_EXTRA_SRCS})
target_link_libraries(vtkKWEIO ${KIT_LIBS})

# -----------------------------------------------------------------------------
# Testing
# -----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif(BUILD_TESTING)


# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------
if(NOT VTKEdge_INSTALL_NO_LIBRARIES)
  install_targets(
    ${VTKEdge_INSTALL_LIB_DIR}
    RUNTIME_DIRECTORY ${VTKEdge_INSTALL_BIN_DIR}
    vtkKWEIO
    )
endif(NOT VTKEdge_INSTALL_NO_LIBRARIES)

if(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)
  install_files(${VTKEdge_INSTALL_INCLUDE_DIR} .h ${KIT_SRCS})
endif(NOT VTKEdge_INSTALL_NO_DEVELOPMENT)

# -----------------------------------------------------------------------------
# This make it easy for other projects to get the list of files etc. in this
# kit.
# -----------------------------------------------------------------------------
include(${VTK_CMAKE_DIR}/vtkExportKit.cmake)
vtk_export_kit2("KWEIO" "KWEIO" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")



