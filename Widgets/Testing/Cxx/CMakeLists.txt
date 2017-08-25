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
set(KIT Widgets)

set(TESTBASE ${PROJECT_NAME}${KIT})

set(WIDGETS_DATA_ROOT ${VTKEdge_SOURCE_DIR}/Data)
set(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

set(MyTests     
  PaintbrushTest1.cxx
  PaintbrushTest2.cxx
  PaintbrushTest3.cxx
  PaintbrushTest4.cxx
  PaintbrushTest5.cxx
  PaintbrushTest6.cxx
  PaintbrushTest7.cxx
  PaintbrushTest8.cxx
  PaintbrushInstantiatonMemLeaksTest.cxx
  )

create_test_sourcelist(Tests 
  ${TESTBASE}CxxTests.cxx 
  ${MyTests}
  EXTRA_INCLUDE vtkTestDriver.h
  )

add_executable(${TESTBASE}CxxTests ${Tests})
#install_targets(${VTKEdge_INSTALL_BIN_DIR} ${TESTBASE}CxxTests)

target_link_libraries(${TESTBASE}CxxTests 
  vtkRendering vtkIO vtkKWEWidgets vtkFiltering vtkImaging )

#ADD regression testing
add_test(PaintbrushTest1 
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest1
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest1InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/EditedLabelMap.mha 
  )
add_test(PaintbrushTest2
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest2
  ${WIDGETS_DATA_ROOT}/Data/HeadMR.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest2InputEventLog.txt
  0
  )
add_test(PaintbrushTest3
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest3
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest3InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha 
  )

add_test(PaintbrushTest4
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest4
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest4InputEventLog.txt
  0 
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha 
  )

add_test(PaintbrushTest5
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest5
  ${WIDGETS_DATA_ROOT}/Data/IBSRData.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest4InputEventLog.txt
  0 
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha 
  )

add_test(PaintbrushTest6
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest6
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-WhiteMatter-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest6InputEventLog.txt
  0 
  )

add_test(PaintbrushTest7
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest7
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest7InputEventLog.txt
  0 
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha 
  )

add_test(PaintbrushTest8
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushTest8
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-USHORT-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest8InputEventLog.txt
  0 
  )

add_test( PaintbrushInstantiationMemLeaksTest
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests 
  PaintbrushInstantiatonMemLeaksTest )

