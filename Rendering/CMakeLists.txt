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
# Set of basic source files (not the OpenGL or GLSL ones)
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWEColorMaterialHelper.cxx
  vtkKWEDataTransferHelper.cxx
  vtkKWEGlyph3DMapper.cxx
  vtkKWEIlluminatedLinesPainter.cxx
  vtkKWELineIntergralConvolution2D.cxx
  vtkKWERenderingFactory.cxx
  vtkKWESurfaceLICDefaultPainter.cxx
  vtkKWESurfaceLICPainter.cxx
  vtkKWEVBOTStripsPainter.cxx
  vtkKWEVertexBufferObject.cxx
  vtkKWEWireframePainter.cxx
  )

set(KIT_EXTRA_SRCS
  vtkKWELightingHelper.cxx
  )

# -----------------------------------------------------------------------------
# List the source files that should not be wrapped.
# -----------------------------------------------------------------------------
set_source_files_properties(
  vtkKWELightingHelper.cxx
  WRAP_EXCLUDE
  )

# -----------------------------------------------------------------------------
# Set of GLSL source files
# -----------------------------------------------------------------------------
set(KIT_GLSL_SRCS
  vtkKWEColorMaterialHelper_vs
  vtkKWEIlluminatedLinesPainter_fs
  vtkKWEIlluminatedLinesPainter_vs
  vtkKWELightingHelper_s
  vtkKWELineIntergralConvolution2D_fs
  vtkKWELineIntergralConvolution2D_fs1
  vtkKWELineIntergralConvolution2D_fs2
  vtkKWESurfaceLICPainter_fs1
  vtkKWESurfaceLICPainter_fs2
  vtkKWESurfaceLICPainter_vs1
  vtkKWEWireframePainter_fs
  vtkKWEWireframePainter_vs
  )

# -----------------------------------------------------------------------------
# Create custom commands to encode each glsl file into a C string literal
# in a header file
# -----------------------------------------------------------------------------

foreach(file ${KIT_GLSL_SRCS})
  set(src ${CMAKE_CURRENT_SOURCE_DIR}/${file}.glsl)
  set(res ${CMAKE_CURRENT_BINARY_DIR}/${file}.cxx)
  add_custom_command(
    OUTPUT ${res}
    DEPENDS ${src}
    COMMAND ${VTK_ENCODESTRING_EXE}
    ARGS ${res} ${src} ${file}
    )
  set(KIT_EXTRA_SRCS ${KIT_EXTRA_SRCS} ${res})
endforeach(file)

# -----------------------------------------------------------------------------
# List the kits from VTK that are needed by this project
# -----------------------------------------------------------------------------
set(KIT_LIBS
  vtkRendering
  )

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
add_library(vtkKWERendering ${KIT_SRCS} ${KIT_EXTRA_SRCS})
target_link_libraries(vtkKWERendering ${KIT_LIBS})

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
    vtkKWERendering
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
vtk_export_kit2("KWERendering" "KWERENDERING" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")
