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

option(VTKEdge_BUILD_PARAVIEW_PLUGINS "Build Plugins for ParaView" OFF)
if(VTKEdge_BUILD_PARAVIEW_PLUGINS)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
  set(MOC_SRCS)
  set(UI_SRCS)

  qt4_wrap_cpp(MOC_SRCS
    pqKWEIlluminatedLinesDisplayPanelDecorator.h
    )

  include_directories(${QT_INCLUDES} ${PARAVIEW_GUI_INCLUDE_DIRS}
    ${PARAVIEW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
  set(KWE_Interfaces
    )

  set(KWE_Sources
    pqKWEIlluminatedLinesDisplayPanelDecorator.cxx
    )

  set(SM_SOURCES
    vtkSMKWEIlluminatedLinesRepresentationProxy.cxx
    )

  set(SM_XMLS
    vtkKWERendering.xml
    vtkKWEExtension.xml
    )

  set(PLUGIN_NAME VTKEdge)
  set(CS_WRAP)

  include(${VTKEdge_BINARY_DIR}/Rendering/vtkKWERenderingKit.cmake)
  foreach(class ${VTK_KWERENDERING_CLASSES})
    set(CS_WRAP ${CS_WRAP} ${VTKEdge_SOURCE_DIR}/Rendering/${class}.h)
  endforeach(class)

  include(${VTKEdge_BINARY_DIR}/Hybrid/vtkKWEHybridKit.cmake)
  foreach(class ${VTK_KWEHYBRID_CLASSES})
    set(CS_WRAP ${CS_WRAP} ${VTKEdge_SOURCE_DIR}/Hybrid/${class}.h)
  endforeach(class)

  add_paraview_plugin(${PLUGIN_NAME} "1.0"
    SERVER_MANAGER_SOURCES "${SM_SOURCES};${CS_WRAP}"
    SERVER_MANAGER_XML ${SM_XMLS}
    GUI_INTERFACES ${KWE_Interfaces}
    GUI_SOURCES ${KWE_Sources} ${MOC_SRCS} ${UI_SRCS}
    )

  target_link_libraries(${PLUGIN_NAME}
    vtkKWERendering
    vtkKWEHybrid
    )

endif(VTKEdge_BUILD_PARAVIEW_PLUGINS)
