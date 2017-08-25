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

# --------------------------------------------------------------------------
# Sources
#
SET(KIT_SRCS
  vtkKWEAbstractPaintbrushWidget.cxx
  vtkKWEBoundingBoxRepresentation2D.cxx
  vtkKWEBoundingBoxWidget2D.cxx
  vtkKWEPaintbrushAnnotationRepresentation.cxx
  vtkKWEPaintbrushAnnotationWidget.cxx
  vtkKWEPaintbrushBlend.cxx
  vtkKWEPaintbrushData.cxx
  vtkKWEPaintbrushDataStatistics.cxx
  vtkKWEPaintbrushDrawing.cxx
  vtkKWEPaintbrushDrawingStatistics.cxx
  vtkKWEPaintbrushGrayscaleData.cxx
  vtkKWEPaintbrushHighlightActors.cxx
  vtkKWEPaintbrushLabelData.cxx
  vtkKWEPaintbrushMergeSketches.cxx
  vtkKWEPaintbrushOperation.cxx
  vtkKWEPaintbrushProperty.cxx
  vtkKWEPaintbrushPropertyManager.cxx
  vtkKWEPaintbrushRepresentation.cxx
  vtkKWEPaintbrushRepresentation2D.cxx
  vtkKWEPaintbrushRepresentationGrayscale2D.cxx
  vtkKWEPaintbrushSelectionWidget.cxx
  vtkKWEPaintbrushSelectionRepresentation.cxx
  vtkKWEPaintbrushSelectionRepresentation2D.cxx
  vtkKWEPaintbrushShape.cxx
  vtkKWEPaintbrushShapeBox.cxx
  vtkKWEPaintbrushShapeEllipsoid.cxx
  vtkKWEPaintbrushSketch.cxx
  vtkKWEPaintbrushStencilData.cxx
  vtkKWEPaintbrushStroke.cxx
  vtkKWEPaintbrushUtilities.cxx
  vtkKWEPaintbrushWidget.cxx
  vtkKWEPaintbrushTesting.cxx
  vtkKWEStencilContourFilter.cxx
  vtkKWEVoxelAlignedImageActorPointPlacer.cxx
  vtkKWEWidgetGroup.cxx
)

SET_SOURCE_FILES_PROPERTIES(
  vtkKWEAbstractPaintbrushWidget.cxx
  vtkKWEPaintbrushData.cxx
  vtkKWEPaintbrushRepresentation.cxx
  vtkKWEPaintbrushShape.cxx
  ABSTRACT
  )

IF(VTKEdge_USE_ITK)
  SET(KIT_SRCS ${KIT_SRCS}
    vtkKWEITKConnectedThresholdPaintbrushOperation.cxx
    vtkKWEITKConfidenceConnectedPaintbrushOperation.cxx
    vtkKWEITKImage.cxx
    vtkKWEITKImageToStencilFilter.cxx
    vtkKWEITKPaintbrushOperation.cxx
    vtkKWEITKPaintbrushExtractConnectedComponents.cxx
    )
  SET_SOURCE_FILES_PROPERTIES(
    vtkKWEITKImageToStencilFilter.cxx
    vtkKWEITKPaintbrushOperation.cxx
    ABSTRACT
    )
ENDIF(VTKEdge_USE_ITK)

# --------------------------------------------------------------------------
# Include dirs
#
# If we are in a tree that includes the ITK source dir already, then 
# we need to explicitly set the ITK include paths. We could let the
# super-project takes care of it (since it is the one that bundles us
# with ITK), but including ITK paths can bring many extra-paths that could
# push the limit of the compiler. Let's do it only when needed.

IF(VTKEdge_USE_ITK AND ITK_SOURCE_DIR)
  INCLUDE(${ITK_SOURCE_DIR}/Utilities/itkThirdParty.cmake)
  INCLUDE(${ITK_SOURCE_DIR}/itkIncludeDirectories.cmake)
  INCLUDE_DIRECTORIES(
    ${ITK_INCLUDE_DIRS_BUILD_TREE}
    ${ITK_INCLUDE_DIRS_BUILD_TREE_CXX}
    ${ITK_INCLUDE_DIRS_SYSTEM}
    )
ENDIF(VTKEdge_USE_ITK AND ITK_SOURCE_DIR)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# -----------------------------------------------------------------------------
# Configure the vtkKWEPaintbrushEnums header.
# -----------------------------------------------------------------------------
set(VTKEdge_PAINTBRUSH_LABEL_TYPE VTK_UNSIGNED_SHORT CACHE STRING "The scalar type used for paintbrush label data.  This impacts memory requirements for the label map image.  Valid values are VTK_CHAR, VTK_UNSIGNED_CHAR, VTK_SHORT, VTK_UNSIGNED_SHORT.")
mark_as_advanced(VTKEdge_PAINTBRUSH_LABEL_TYPE)
set(VTKEdge_PAINTBRUSH_LABEL_TYPEDEF)
set(typedef_VTK_CHAR           "char")
set(typedef_VTK_UNSIGNED_CHAR  "unsigned char")
set(typedef_VTK_SHORT          "short")
set(typedef_VTK_UNSIGNED_SHORT "unsigned short")
set(VTKEdge_PAINTBRUSH_LABEL_TYPEDEF ${typedef_${VTKEdge_PAINTBRUSH_LABEL_TYPE}})
if(NOT VTKEdge_PAINTBRUSH_LABEL_TYPEDEF)
  message(SEND_ERROR "VTKEdge_PAINTBRUSH_LABEL_TYPE has been set to an invalid value.  Please choose from VTK_CHAR, VTK_UNSIGNED_CHAR, VTK_SHORT, VTK_UNSIGNED_SHORT")
endif(NOT VTKEdge_PAINTBRUSH_LABEL_TYPEDEF)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vtkKWEPaintbrushEnums.h.in"
  "${PROJECT_BINARY_DIR}/vtkKWEPaintbrushEnums.h")
set(KIT_SRCS ${KIT_SRCS} "${PROJECT_BINARY_DIR}/vtkKWEPaintbrushEnums.h")

# --------------------------------------------------------------------------
# Build the library
#
set(KIT_LIBS vtkWidgets 
             vtkRendering 
             vtkFiltering 
             vtkCommon 
             vtkImaging )
IF(VTKEdge_USE_ITK)
  set (KIT_LIBS ${KIT_LIBS} ITKCommon ITKBasicFilters ) 
ENDIF(VTKEdge_USE_ITK)
add_library(vtkKWEWidgets ${KIT_SRCS})
target_link_libraries(vtkKWEWidgets ${KIT_LIBS})

# --------------------------------------------------------------------------
# Testing
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
    vtkKWEWidgets
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
vtk_export_kit2("KWEWidgets" "KWEWIDGETS" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")
