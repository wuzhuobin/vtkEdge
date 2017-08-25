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

# -----------------------------------------------------------------------------
# Set of basic source files
# -----------------------------------------------------------------------------
set(KIT_SRCS
  vtkKWECommonFactory.cxx
  vtkKWEDataArrayStreamer.cxx
  vtkKWEDummyGPUInfoList.cxx
  vtkKWEEventForwarderCommand.cxx
  vtkKWEFunctionToGLSL.cxx
  vtkKWEUUID.cxx
  vtkKWEGPUInfo.cxx
  vtkKWEGPUInfoList.cxx
  vtkKWEGPUInfoListArray.cxx
  vtkKWEInformationKeyMap.cxx
  )

set_source_files_properties(
  vtkKWEGPUInfoList.cxx
  ABSTRACT
  )

# Get GPU VRAM info
# on Windows
#message("DirectX_FOUND=${DirectX_FOUND}")
if(DirectX_FOUND)
 set(KIT_SRCS ${KIT_SRCS} vtkKWEDirectXGPUInfoList)
endif(DirectX_FOUND)

# on Mac
if(ApplicationServices_FOUND)
 set(KIT_SRCS ${KIT_SRCS} vtkKWECoreGraphicsGPUInfoList.mm)
endif(ApplicationServices_FOUND)

# on Linux, for Nvidia
if(VTK_USE_X)
 set(KIT_SRCS ${KIT_SRCS} vtkKWEXGPUInfoList)
endif(VTK_USE_X)

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

if(DirectX_FOUND)
 # d3d9.lib for symbol Direct3DCreate9
 # symbol CLSID_WbemLocator IID_IWbemLocator: wbemuuid.lib 
 # wbemuuid.lib is *NOT A FILE* (you cannot find it), it is probably an
 # embedded library added by the linker itself.
 set(KIT_LIBS ${KIT_LIBS} ${DirectX_LIBRARY} wbemuuid)
 include_directories(${DirectX_INCLUDE_DIR})
endif(DirectX_FOUND)

if(VTKEdge_USE_CORE_GRAPHICS)
 set(KIT_LIBS ${KIT_LIBS} ${ApplicationServices_LIBRARY} ${IOKit_LIBRARY})
 include_directories(${ApplicationServices_INCLUDE_DIR} ${IOKit_INCLUDE_DIR})
endif(VTKEdge_USE_CORE_GRAPHICS)

if(VTKEdge_USE_NVCONTROL)
 # NVCONTROL depends on X11. We solve that by making KIT_LIBS depending on
 # vtkRendering, as vtkRendering depends on X11 on Unix.
 set(KIT_LIBS ${KIT_LIBS} ${NVCtrlLib_LIBRARY} vtkRendering)
 include_directories(${NVCtrlLib_INCLUDE_DIR})
endif(VTKEdge_USE_NVCONTROL)

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
