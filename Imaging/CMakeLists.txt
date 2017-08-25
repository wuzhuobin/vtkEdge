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
# Set of basic source files (not the OpenGL, GLSL or CUDA ones)
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWEImageFDKFilter.cxx
  )

# -----------------------------------------------------------------------------
# Set of source files that call Cuda
# -----------------------------------------------------------------------------
if(CUDA_FOUND AND CUDA_CUTIL_FOUND)
  set(KIT_CUDA_DEPENDANT_SRCS
    vtkKWEImageFFT.cxx
    vtkKWEImageRFFT.cxx
    )
  set(KIT_CUDA_SOURCES
    vtkKWEImageFFT.cu
    vtkKWEImageRFFT.cu
    )
  include_directories(
    ${CUDA_INCLUDE_DIR}
    ${CUDA_CUTIL_INCLUDE_DIR}
    )
  cuda_compile(KIT_CUDA_C_SOURCES ${KIT_CUDA_SOURCES})
endif(CUDA_FOUND AND CUDA_CUTIL_FOUND)

# -----------------------------------------------------------------------------
# List the kits from VTK that are needed by this project
# -----------------------------------------------------------------------------
set(KIT_LIBS
  vtkIO
  vtkImaging
  vtkRendering
  vtkVolumeRendering
  )

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
if(KIT_CUDA_C_SOURCES OR KIT_SRCS OR KIT_CUDA_DEPENDANT_SRCS)
  add_library(vtkKWEImaging ${KIT_CUDA_C_SOURCES} ${KIT_SRCS} ${KIT_CUDA_DEPENDANT_SRCS})
  target_link_libraries(vtkKWEImaging ${KIT_LIBS} ${CUDA_LIBRARIES} ${CUDA_CUTIL_LIBRARY})
endif(KIT_CUDA_C_SOURCES OR KIT_SRCS OR KIT_CUDA_DEPENDANT_SRCS)

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
    vtkKWEImaging
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
vtk_export_kit2("KWEImaging" "KWEImaging" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")
