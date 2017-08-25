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


#
# Build the documentation
#
#INCLUDE (Documentation)

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake)

IF (BUILD_DOCUMENTATION)
  SET(BUILD_DOXYGEN ON)
  SET(DOCUMENTATION_DOWNLOAD_VTK_TAGFILE ON)
  SET(DOCUMENTATION_HTML_TARZ ON)
  SET(DOXYGEN_PROJECT_NAME "VTKEdge")

# path to Perl scripts used to produce the VTK doc in Doxygen format.
  FIND_PATH(VTK_SOURCE_DIR vtkConfigure.h.in ${VTKEdge_SOURCE_DIR}/.. DOC "root of the VTK source directory")

  MESSAGE("VTK_SOURCE_DIR=${VTK_SOURCE_DIR}")

  SET(DOXYGEN_SOURCE_DIR "${VTK_SOURCE_DIR}/Utilities/Doxygen")

  SET(DOXYGEN_PROJECT_SOURCE_DIR "${VTKEdge_SOURCE_DIR}")
  SET(VTK_DOXYGEN_HOME "${VTK_SOURCE_DIR}/Utilities/Doxygen")
#  SET(DOXYGEN_CVSWEB_CHECKOUT "http://public.kitware.com/cgi-bin/cvsweb.cgi/~checkout~/ParaView3/")
#  SET(DOXYGEN_CVSWEB_CHECKOUT_SUFFIX "?cvsroot=ParaView3")
  SET(DOXYGEN_SOURCE_LOCATIONS_DIR "${VTKEdge_SOURCE_DIR}")
  SET(DOXYGEN_SOURCE_LOCATIONS
       "\"${VTKEdge_SOURCE_DIR}/Common\" \\
       \"${VTKEdge_SOURCE_DIR}/Filtering\" \\
       \"${VTKEdge_SOURCE_DIR}/Graphics\" \\
       \"${VTKEdge_SOURCE_DIR}/Hybrid\" \\
       \"${VTKEdge_SOURCE_DIR}/Imaging\" \\
       \"${VTKEdge_SOURCE_DIR}/IO\" \\
       \"${VTKEdge_SOURCE_DIR}/Rendering\" \\
       \"${VTKEdge_SOURCE_DIR}/Widgets\" \\
       \"${VTKEdge_SOURCE_DIR}/VolumeRendering\"")
  SET(DOXYGEN_BINARY_LOCATIONS_DIR "${VTKEdge_BINARY_DIR}")
  SET(DOXYGEN_BINARY_LOCATIONS
       "")
  SET(DOXYGEN_PARSED_INPUT_DIRS
  "\"${CMAKE_CURRENT_BINARY_DIR}/dox/Common\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Filtering\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Graphics\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Hybrid\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Imaging\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/IO\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Rendering\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/Widgets\" \\
       \"${CMAKE_CURRENT_BINARY_DIR}/dox/VolumeRendering\"")


  SET(DOXYGEN_IGNORE_PREFIX "vtkKWE vtk")

  #
  # Configure the script and the doxyfile, then add target
  #
  IF(NOT DOT_PATH)
    GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  ENDIF(NOT DOT_PATH)

  SET(TAGFILES "\"${CMAKE_CURRENT_BINARY_DIR}/vtkNightlyDoc.tag=http://www.vtk.org/doc/nightly/html\"")

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY IMMEDIATE)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc_makeall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh @ONLY IMMEDIATE)

  ADD_CUSTOM_TARGET(${DOXYGEN_PROJECT_NAME}DoxygenDoc
    ${BASH}
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh)

ENDIF (BUILD_DOCUMENTATION)
