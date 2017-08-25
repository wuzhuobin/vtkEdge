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
# IO is required!
# -----------------------------------------------------------------------------
if(NOT VTKEdge_BUILD_IO_KIT)
  message(FATAL_ERROR "VTKEdge_BUILD_Filtering_KIT requires VTKEdge_BUILD_IO_KIT.")
endif(NOT VTKEdge_BUILD_IO_KIT)

# -----------------------------------------------------------------------------
# Set of basic source files
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWEObjectTreePropertyBase.cxx
  vtkKWEObjectTreeTransformableNode.cxx
  vtkKWEObjectTreeUserProperty.cxx
  vtkKWEObjectTreeNodeBase.cxx
  vtkKWEObjectTreeNodeIterator.cxx
  vtkKWEObjectTreeColorProperty.cxx
  )

set_source_files_properties(
  vtkKWEObjectTreePropertyBase
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
  vtkKWEIO
  )

# -----------------------------------------------------------------------------
# Setup vtkInstantiator registration for this library's classes.
# THIS IS REQUIRED FOR THE SERIALIZATION TO WORK
# -----------------------------------------------------------------------------
INCLUDE(${VTK_CMAKE_DIR}/vtkMakeInstantiator.cmake)
VTK_MAKE_INSTANTIATOR3(vtkKWEFilteringInstantiator FilteringInstantiatorFiles
  "${KIT_SRCS}"
  VTK_EXPORT
  "${CMAKE_CURRENT_BINARY_DIR}" "")

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
add_library(vtkKWEFiltering ${KIT_SRCS} ${KIT_EXTRA_SRCS} ${FilteringInstantiatorFiles})
target_link_libraries(vtkKWEFiltering ${KIT_LIBS})


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
    vtkKWEFiltering
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
vtk_export_kit2("KWEFiltering" "KWEFILTERING" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")




