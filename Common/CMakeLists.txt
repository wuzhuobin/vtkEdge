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
# Set of basic source files
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWEDataArrayStreamer.cxx
  vtkKWEFunctionToGLSL.cxx
  vtkKWEUUID.cxx
  vtkKWEInformationKeyMap.cxx
  )

# -----------------------------------------------------------------------------
# List the kits from VTK that are needed by this project
# Need filtering for now because older VTK has vtkInformationKey in
# vtkFiltering, not vtkCommon. Specifically trying to support
# VolView build which still uses an older version of VTK
# -----------------------------------------------------------------------------
set(KIT_LIBS
  vtkCommon
  vtkFiltering
  )

# -----------------------------------------------------------------------------
# UUID support
# -----------------------------------------------------------------------------
find_package(UUID)
if(UUID_FOUND)
  set(SYSTEM_UUID_FOUND 1)
  set(KIT_LIBS ${KIT_LIBS} ${UUID_LIBRARIES})
endif(UUID_FOUND)

if(WIN32)
  if (NOT BORLAND)
    # rpcrt4 -> UuidCreate
    #snmpapi -> getting MAC address
    set(KIT_LIBS ${KIT_LIBS} rpcrt4 snmpapi)
  endif(NOT BORLAND)
endif(WIN32)

# Check if header file exists and add it to the list.
include (${CMAKE_ROOT}/Modules/CheckIncludeFiles.cmake)
MACRO(CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
  CHECK_INCLUDE_FILES("${KWECommon_INCLUDES};${FILE}" ${VARIABLE})
  if(${VARIABLE})
    set(KWECommon_INCLUDES ${KWECommon_INCLUDES} ${FILE})
  endif(${VARIABLE})
ENDMACRO(CHECK_INCLUDE_FILE_CONCAT)

# Need these headers for GetMacAddress in vtkKWEUUID
# Tricky as you need to figure out the proper order to test the inclusion of files
CHECK_INCLUDE_FILE_CONCAT("unistd.h"       CMAKE_HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CONCAT("stdlib.h"       CMAKE_HAVE_STDLIB_H)
if(UNIX) #Avoid polluting Win32 cmakecache
  CHECK_INCLUDE_FILE_CONCAT("sys/ioctl.h"    CMAKE_HAVE_SYS_IOCTL_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/socket.h"   CMAKE_HAVE_SYS_SOCKET_H)
  CHECK_INCLUDE_FILE_CONCAT("sys/sockio.h"   CMAKE_HAVE_SYS_SOCKIO_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if.h"       CMAKE_HAVE_NET_IF_H)
  CHECK_INCLUDE_FILE_CONCAT("netinet/in.h"   CMAKE_HAVE_NETINET_IN_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_dl.h"    CMAKE_HAVE_NET_IF_DL_H)
  CHECK_INCLUDE_FILE_CONCAT("net/if_arp.h"   CMAKE_HAVE_NET_IF_ARP_H)
endif(UNIX)

# Test if os defines a length for sockaddr
# only doing the test when we have sys/socket
if( ${CMAKE_HAVE_SYS_SOCKET_H} )
  if("HAVE_SA_LEN" MATCHES "^HAVE_SA_LEN$")
    STRING(ASCII 35 POUND)
    FILE(WRITE ${PROJECT_BINARY_DIR}/CMakeTmp/kweCommonTestHAVESALEN.c
      "${POUND}include <sys/types.h>\n"
      "${POUND}include <sys/socket.h>\n"
      "int main() { struct sockaddr sa; sa.sa_len = 0; }\n")
    message(STATUS "Checking if ifreq has a sa_len")
    TRY_COMPILE(HAVE_SA_LEN
      ${PROJECT_BINARY_DIR}
      ${PROJECT_BINARY_DIR}/CMakeTmp/kweCommonTestHAVESALEN.c
      OUTPUT_VARIABLE OUTPUT)
    if(HAVE_SA_LEN)
      message(STATUS "Checking if ifreq has a sa_len -- yes")
      set(HAVE_SA_LEN 1 CACHE INTERNAL "Support if ifreq has a sa_len")
      write_file(${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
        "Determining if ifreq has a sa_len"
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    else(HAVE_SA_LEN)
      message(STATUS "Checking if ifreq has a sa_len -- no")
      set(HAVE_SA_LEN 0 CACHE INTERNAL "Support if ifreq has a sa_len")
      write_file(${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
        "Determining if ifreq has a sa_len"
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    endif(HAVE_SA_LEN)
  endif("HAVE_SA_LEN" MATCHES "^HAVE_SA_LEN$")
endif( ${CMAKE_HAVE_SYS_SOCKET_H} )

# The configured header file used to pass CMake settings to the source code.
configure_file(
  "${PROJECT_SOURCE_DIR}/Common/VTKEdgeUUIDConfigure.h.in"
  "${PROJECT_BINARY_DIR}/VTKEdgeUUIDConfigure.h"
  )

# -----------------------------------------------------------------------------
# Create the library
# -----------------------------------------------------------------------------
add_library(vtkKWECommon ${KIT_SRCS})
target_link_libraries(vtkKWECommon ${KIT_LIBS})

# -----------------------------------------------------------------------------
# Testing
# -----------------------------------------------------------------------------
#if(BUILD_TESTING)
# add_subdirectory(Testing)
#endif(BUILD_TESTING)

# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------
if(NOT VTKEdge_INSTALL_NO_LIBRARIES)
  install_targets(
    ${VTKEdge_INSTALL_LIB_DIR}
    RUNTIME_DIRECTORY ${VTKEdge_INSTALL_BIN_DIR}
    vtkKWECommon
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
vtk_export_kit2("KWECommon" "KWECOMMON" ${CMAKE_CURRENT_BINARY_DIR} "${KIT_SRCS}")
