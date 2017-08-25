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
# Name this kit - this will be used to form test executable names later
# Then we'll add in tests with various data or kit dependencies to the
# test executables
# -----------------------------------------------------------------------------
set(KIT Imaging)

set(TESTBASE ${PROJECT_NAME}${KIT})

# -----------------------------------------------------------------------------
# First, list all the tests that either use no data, or only data from the
# VTKEdge data directory (which always exists since it is part of the
# VTKEdge repository). These will go into one test executable.
# -----------------------------------------------------------------------------
set(ImagingTests
  TestKWEImageFDK.cxx
  )
if(CUDA_FOUND AND CUDA_CUTIL_FOUND)
  set(CUDAImagingTests
    TestKWEImageFFT.cxx
    )
endif(CUDA_FOUND AND CUDA_CUTIL_FOUND)

# add tests that require data from VTKData/Data
if(VTK_DATA_ROOT)

  set(ImagingTestsWithVTKData
    )

  if(CUDA_FOUND AND CUDA_CUTIL_FOUND)
    set(CUDAImagingTestsWithVTKData  
      )
  endif(CUDA_FOUND AND CUDA_CUTIL_FOUND)
endif(VTK_DATA_ROOT)

create_test_sourcelist(Tests 
  ${TESTBASE}CxxTests.cxx 
  ${ImagingTests} 
  ${CUDAImagingTests} 
  ${ImagingTestsWithVTKData} 
  ${CUDAImagingTestsWithVTKData}
  EXTRA_INCLUDE vtkTestDriver.h
  )

add_executable(${TESTBASE}CxxTests ${Tests})
#install_targets(${VTKEdge_INSTALL_BIN_DIR} ${TESTBASE}CxxTests)

target_link_libraries(${TESTBASE}CxxTests vtkKWEImaging)
set(TestsToRun ${Tests})
remove(TestsToRun ${TESTBASE}CxxTests.cxx ${ImagingTestsWithVTKData})

# Add all the executables
foreach(test ${TestsToRun})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${TESTBASE}CxxTests ${TName}
    -D ${PROJECT_SOURCE_DIR}/Data
    -T ${PROJECT_BINARY_DIR}/Testing/Temporary
    -V ${PROJECT_SOURCE_DIR}/Data/Baseline/${KIT}/${TName}.png
    )
endforeach(test)

if(VTK_DATA_ROOT)
  foreach(test ${MyTestsWithVTKData})
    get_filename_component(TName ${test} NAME_WE)
    add_test(${TName} ${CXX_TEST_PATH}/${TESTBASE}CxxTests ${TName}
      -D ${VTK_DATA_ROOT}
      -T ${PROJECT_BINARY_DIR}/Testing/Temporary
      -V ${PROJECT_SOURCE_DIR}/Data/Baseline/${KIT}/${TName}.png
      )
  endforeach(test)
endif(VTK_DATA_ROOT)