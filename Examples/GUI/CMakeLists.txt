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

option(VTKEdge_BUILD_KWWidgets_EXAMPLES "Build examples that use the KWWidgets libraries." OFF)
if (VTKEdge_BUILD_KWWidgets_EXAMPLES)
  IF(NOT KWWidgets_SOURCE_DIR)
    FIND_PACKAGE(KWWidgets REQUIRED)
    IF(NOT KWWidgets_FOUND)
      MESSAGE(FATAL_ERROR
       "KWWidgets is required to build the these examples. Plase specify KWWidgets_DIR")
    ENDIF(NOT KWWidgets_FOUND)
    INCLUDE(${KWWidgets_USE_FILE})
  ENDIF(NOT KWWidgets_SOURCE_DIR)

  SUBDIRS( KWWidgets )
endif (VTKEdge_BUILD_KWWidgets_EXAMPLES)

