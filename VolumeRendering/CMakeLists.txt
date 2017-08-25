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
  vtkKWERepresentativeVolumeImageCreator.cxx
  )

# -----------------------------------------------------------------------------
# Set of OpenGL source files
# -----------------------------------------------------------------------------
set(KIT_OpenGL_SRCS
  )

# -----------------------------------------------------------------------------
# Set of GLSL source files
# -----------------------------------------------------------------------------
set(KIT_GLSL_SRCS
  )

# -----------------------------------------------------------------------------
# Create custom commands to encode each glsl file into a C string literal
# in a header file
# -----------------------------------------------------------------------------

foreach(file ${KIT_GLSL_SRCS})
  set(src ${VTKEdge_SOURCE_DIR}/VolumeRendering/${file}.glsl)
  set(res ${VTKEdge_BINARY_DIR}/VolumeRendering/${file}.cxx)
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
  vtkVolumeRendering
  vtkKWECommon
  )

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
add_library(vtkKWEVolumeRendering ${KIT_SRCS} ${KIT_OpenGL_SRCS} ${KIT_EXTRA_SRCS})
target_link_libraries(vtkKWEVolumeRendering ${KIT_LIBS})

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
    vtkKWEVolumeRendering
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
vtk_export_kit2("KWEVolumeRendering" "KWEVOLUMERENDERING" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")
