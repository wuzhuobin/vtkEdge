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


# - Find UUID
# Find the native UUID includes and library
# This module defines
#  UUID_INCLUDE_DIR, where to find jpeglib.h, etc.
#  UUID_LIBRARIES, the libraries needed to use UUID.
#  UUID_FOUND, If false, do not try to use UUID.
# also defined, but not for general use are
#  UUID_LIBRARY, where to find the UUID library.

FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h
/usr/local/include
/usr/include
)

FIND_LIBRARY(UUID_LIBRARY
  NAMES uuid
  PATHS /lib /usr/lib /usr/local/lib
  )

IF (UUID_LIBRARY AND UUID_INCLUDE_DIR)
  SET(UUID_LIBRARIES ${UUID_LIBRARY})
  SET(UUID_FOUND "YES")
ELSE (UUID_LIBRARY AND UUID_INCLUDE_DIR)
  SET(UUID_FOUND "NO")
ENDIF (UUID_LIBRARY AND UUID_INCLUDE_DIR)


IF (UUID_FOUND)
   IF (NOT UUID_FIND_QUIETLY)
      MESSAGE(STATUS "Found UUID: ${UUID_LIBRARIES}")
   ENDIF (NOT UUID_FIND_QUIETLY)
ELSE (UUID_FOUND)
   IF (UUID_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find UUID library")
   ENDIF (UUID_FIND_REQUIRED)
ENDIF (UUID_FOUND)

# Deprecated declarations.
#SET (NATIVE_UUID_INCLUDE_PATH ${UUID_INCLUDE_DIR} )
#GET_FILENAME_COMPONENT (NATIVE_UUID_LIB_PATH ${UUID_LIBRARY} PATH)

MARK_AS_ADVANCED(
  UUID_LIBRARY
  UUID_INCLUDE_DIR
  )
