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

if(NOT VTKEdge_SOURCE_DIR)
  find_package(VTKEdge REQUIRED)
  include(${VTKEdge_USE_FILE})
endif(NOT VTKEdge_SOURCE_DIR)

if(NOT KWWidgets_SOURCE_DIR)
  find_package(KWWidgets REQUIRED)
  if(NOT KWWidgets_FOUND)
    message(FATAL_ERROR 
     "KWWidgets is required to build the Paintbrush examples. Plase specify KWWidgets_DIR")
  endif(NOT KWWidgets_FOUND)
  include(${KWWidgets_USE_FILE})
endif(NOT KWWidgets_SOURCE_DIR)

if (VTKEdge_USE_ITK)
  if(NOT ITK_SOURCE_DIR)
    find_package(ITK REQUIRED)
    if(NOT ITK_FOUND)
      message(FATAL_ERROR 
       "ITK is required to build the Paintbrush examples. Plase specify ITK_DIR")
    endif(NOT ITK_FOUND)
    include(${ITK_USE_FILE})
  endif(NOT ITK_SOURCE_DIR)
endif (VTKEdge_USE_ITK)

include_regular_expression("^.*$")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

include("${KWWidgets_CMAKE_DIR}/KWWidgetsWrappingMacros.cmake")
kwwidgets_wrap_tcl("KWMyWindowLib" LIB_TCL_SRCS "vtkKWMyWindow.cxx" "")
add_library(KWMyWindowLib STATIC ${LIB_TCL_SRCS} "vtkKWMyWindow.cxx")
target_link_libraries(KWMyWindowLib ${VTKEdge_LIBRARIES})

set(cxx_names
  PaintbrushExample1
  PaintbrushExample2
  PaintbrushExample3
  PaintbrushExample4
  PaintbrushLabelMapSimpleExample
  PaintbrushLabelMapChangeNoLabelValue
  PaintbrushExample8
  PaintbrushExample9
  )

set(itk_cxx_names)
if(ITK_FOUND OR ITK_SOURCE_DIR)
  set(itk_cxx_names
    PaintbrushExample5
    PaintbrushExample6
    PaintbrushExample7
    )
  set(cxx_names ${cxx_names} ${itk_cxx_names})
endif(ITK_FOUND OR ITK_SOURCE_DIR)

foreach(cxx_name ${cxx_names})

  set(exe ${VTKEdge_EXAMPLE_EXECUTABLE_PREFIX}${cxx_name})

  # On Win32 platforms, let's create a .rc resource file to setup a decent
  # application icon as well as some additional information in the "Version"
  # tab of the properties panel. This is also required for static Tk

  set(RC_FILENAME)
  if(WIN32 AND NOT BORLAND AND NOT CYGWIN)
    include("${KWWidgets_CMAKE_DIR}/KWWidgetsResourceMacros.cmake")
    set(RC_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/${exe}.rc")
    kwwidgets_create_rc_file(
      RC_FILENAME "${RC_FILENAME}"
      RC_APPLICATION_NAME "${exe}"
      RC_COMPANY_NAME "Kitware, Inc.")
  endif(WIN32 AND NOT BORLAND AND NOT CYGWIN)

  add_executable(${exe} WIN32 ${cxx_name}.cxx ${RC_FILENAME})
  target_link_libraries(${exe} 
    KWMyWindowLib ${KWWidgets_LIBRARIES} ${VTKEdge_LIBRARIES})
  install_targets(${VTKEdge_INSTALL_BIN_DIR} ${exe})
endforeach(cxx_name)

# NOT BUILD_SHARED_LIBS: until I figure out why the damn:
# 'itk::ImageToVTKImageFilter<TInputImage>::...' definition of dllimport function not allowed

if(ITK_FOUND OR ITK_SOURCE_DIR)
  foreach(cxx_name ${itk_cxx_names})
    set(exe ${VTKEdge_EXAMPLE_EXECUTABLE_PREFIX}${cxx_name})
    target_link_libraries(${exe} ${ITK_LIBRARIES})
  endforeach(cxx_name)
endif(ITK_FOUND OR ITK_SOURCE_DIR)

# If needed, copy the Tcl/Tk support files required at run-time 
# to initialize Tcl/Tk. This is only triggered if VTK was built
# against a Tcl/Tk static library.

include("${KWWidgets_CMAKE_DIR}/KWWidgetsTclTkMacros.cmake")
kwwidgets_copy_tcl_tk_support_files("${PROJECT_BINARY_DIR}/lib")
kwwidgets_install_tcl_tk_support_files("/lib")

