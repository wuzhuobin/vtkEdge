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

# -----------------------------------------------------------------------------
# Set of basic source files (not the OpenGL or GLSL ones)
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWEIlluminatedLinesPainter.cxx
  vtkKWERenderingFactory.cxx
  vtkKWEVBOTStripsPainter.cxx
  vtkKWEVertexBufferObject.cxx
  vtkKWEWireframePainter.cxx
  )

# -----------------------------------------------------------------------------
# Set of GLSL source files
# -----------------------------------------------------------------------------
set(KIT_GLSL_SRCS
  vtkKWEIlluminatedLinesPainter_fs
  vtkKWEIlluminatedLinesPainter_vs
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
#set(KIT_EXTRA_SRCS ${res}) 
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
