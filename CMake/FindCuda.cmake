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

#=========================================================================
# Module to locate NVIDIA's CUDA Toolkit and SDK.
#
#   Basic variables
#   CUDA_FOUND        - True if Cuda runtime library/compiler were found.
#   CUDA_CUTIL_FOUND  - True if Cuda SDK was found.
#   CUDA_LIBRARIES    - Libraries to link against to use Cuda.
#   CUDA_CUTIL_LIBRARIES  - Libraries to link agains to use cutil.
#   CUDA_INCLUDE_DIR      - Include dir for Cuda run time.
#   CUDA_CUTIL_INCLUDE_DIR  - Include dir for cutil.

#   Additional variables
#   CUDA_COMPILER     - the cuda nvcc compiler.
#   CUDA_DEVICE_EMULATION - Set this to True to use emulation.

#   Macro to convert a *.cu file to *.c file that can be compiled by the C
#   compiler.
#   CUDA_COMPILE(outfiles inputfiles ... )

FIND_PACKAGE(OpenGL)
SET(CUDA_FOUND FALSE)
SET(CUDA_CUTIL_FOUND FALSE)

## Find Cuda toolkit.
FIND_PROGRAM(CUDA_COMPILER NAMES  "nvcc"
  PATHS "/usr/local/cuda/bin"
  "C:/CUDA/bin"
  DOC "Path to the  Cuda compiler (nvcc)"
)

FIND_LIBRARY(CUDA_RT_LIBRARY cudart
  "/usr/local/cuda/lib"
  "C:/CUDA/lib"
  DOC "Path to Cuda runtime library (cudart)")

FIND_LIBRARY(CUDA_FFT_LIBRARY cufft
  "/usr/local/cuda/lib"
  "C:/CUDA/lib"
  DOC "Path to Cuda FFT library (cufft)")

FIND_PATH(CUDA_INCLUDE_DIR cuda.h
  "/usr/local/cuda/include"
  "C:/CUDA/include"
  DOC "Path to Cuda headers (cuda.h)")

IF (CUDA_COMPILER AND CUDA_RT_LIBRARY AND CUDA_INCLUDE_DIR)
  SET (CUDA_FOUND TRUE)
ELSE (CUDA_COMPILER AND CUDA_RT_LIBRARY AND CUDA_INCLUDE_DIR)
  SET (CUDA_FOUND FALSE)
ENDIF (CUDA_COMPILER AND CUDA_RT_LIBRARY AND CUDA_INCLUDE_DIR)

SET (CUDA_LIBRARIES CACHE INTERNAL "Libraries to link against Cuda.")
IF (CUDA_FOUND)
  SET (CUDA_LIBRARIES
    ${CUDA_RT_LIBRARY}
    ${CUDA_FFT_LIBRARY})
ENDIF (CUDA_FOUND)

SET (CUDA_DEVICE_EMULATION OFF CACHE BOOL "Use device enumlation for Cuda")

MARK_AS_ADVANCED(CUDA_COMPILER CUDA_RT_LIBRARY CUDA_FFT_LIBRARY CUDA_INCLUDE_DIR CUDA_DEVICE_EMULATION)

## Find Cuda SDK.

FIND_LIBRARY(CUDA_CUTIL_LIBRARY 
  NAMES cutil64.lib cutil32.lib cutil
  PATHS "C:/Program Files (x86)/NVIDIA Corporation/NVIDIA CUDA SDK/common/lib/"
  "C:/Program Files/NVIDIA Corporation/NVIDIA CUDA SDK/common/lib"
  DOC "Path to Cuda SDK libraries (cutil)")

FIND_PATH(CUDA_CUTIL_INCLUDE_DIR  cutil.h
  "C:/Program Files (x86)/NVIDIA Corporation/NVIDIA CUDA SDK/common/inc"
  "C:/Program Files/NVIDIA Corporation/NVIDIA CUDA SDK/common/inc"
  DOC "Path to Cuda SDK include directory (cutil.h)")

IF (CUDA_CUTIL_LIBRARY AND CUDA_CUTIL_INCLUDE_DIR)
  SET (CUDA_CUTIL_FOUND TRUE)
ELSE (CUDA_CUTIL_LIBRARY AND CUDA_CUTIL_INCLUDE_DIR)
  SET (CUDA_CUTIL_FOUND FALSE)
ENDIF (CUDA_CUTIL_LIBRARY AND CUDA_CUTIL_INCLUDE_DIR)

MARK_AS_ADVANCED(CUDA_CUTIL_LIBRARY CUDA_CUTIL_INCLUDE_DIR)

SET (CUDA_CUTIL_LIBRARIES CACHE INTERNAL "Libraries to link agains Cuda cutil.")
IF (CUDA_CUTIL_FOUND)
  SET (CUDA_CUTIL_LIBRARIES 
      ${CUDA_LIBRARIES}
      ${CUDA_CUTIL_LIBRARY})
ENDIF (CUDA_CUTIL_FOUND)

MACRO (CUDA_COMPILE outfiles)
  IF (WIN32)
    SET (extra_args "-ccbin \"$(VCInstallDir)bin\" ")
  ENDIF (WIN32)

  IF (CUDA_DEVICE_EMULATION)
    SET (extra_args "${extra_args} --device-emulation")
  ENDIF (CUDA_DEVICE_EMULATION)

  # Get the directories to pass as include directories.
  GET_DIRECTORY_PROPERTY(include_dirs INCLUDE_DIRECTORIES)
  FOREACH (iter ${include_dirs})
    SET (extra_args ${extra_args} -I ${iter})
  ENDFOREACH (iter)


  FOREACH (iter ${ARGN})
    GET_FILENAME_COMPONENT(filename ${iter} NAME)
    SET (outfile ${CMAKE_CURRENT_BINARY_DIR}/${filename}.cxx)
    GET_FILENAME_COMPONENT(infile ${iter} ABSOLUTE)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${outfile}
      COMMAND ${CUDA_COMPILER} ${extra_args} 
        -o ${outfile}
        -cuda
        ${infile}
      DEPENDS ${iter}
      COMMENT "Processing CUDA file")
    SET (${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (iter)
ENDMACRO (CUDA_COMPILE outfiles) 
