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

option(VTKEdge_BUILD_PARAVIEW_PLUGINS "Build Plugins for ParaView" OFF)
if(VTKEdge_BUILD_PARAVIEW_PLUGINS)
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
  set(MOC_SRCS)
  set(UI_SRCS)

  qt4_wrap_cpp(MOC_SRCS 
    pqKWESurfaceLICDisplayPanelDecoratorImplementation.h
    pqKWESurfaceLICDisplayPanelDecorator.h
    pqKWEIlluminatedLinesDisplayPanelDecorator.h
    )

  qt4_wrap_ui(UI_SRCS pqKWESurfaceLICDisplayPanelDecorator.ui)

  include_directories(${QT_INCLUDES} ${PARAVIEW_GUI_INCLUDE_DIRS}
    ${PARAVIEW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
  set(KWE_Interfaces
    pqKWESurfaceLICDisplayPanelDecorator
    )

  set(KWE_Sources
    vtkKWEProxyManagerExtension.cxx
    pqKWESurfaceLICDisplayPanelDecoratorImplementation.cxx
    pqKWESurfaceLICDisplayPanelDecorator.cxx
    pqKWEIlluminatedLinesDisplayPanelDecorator.cxx
    )
  
  set(SM_SOURCES
    vtkSMKWESurfaceLICRepresentationProxy.cxx
    vtkSMKWEIlluminatedLinesRepresentationProxy.cxx
    vtkSMKWEGlyph3DMapperRepresentationProxy.cxx
    )

  set(SM_XMLS
    vtkKWERendering.xml
    )

  set(XML_EXTENSION_HEADERS)
  generate_server_manager_xml_header(XML_EXTENSION_HEADERS
    "VTKEdgeExtension"
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
    GUI_SOURCES ${KWE_Sources} ${MOC_SRCS} ${XML_EXTENSION_HEADERS} ${UI_SRCS}
    )

  target_link_libraries(${PLUGIN_NAME}
    vtkKWERendering
    vtkKWEHybrid
    )

endif(VTKEdge_BUILD_PARAVIEW_PLUGINS)
