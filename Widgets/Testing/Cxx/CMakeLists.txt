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
  PaintbrushTest1b.cxx
  PaintbrushTest1c.cxx
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
set(labelmap_type "USHORT")
if (VTKEdge_PAINTBRUSH_LABEL_TYPE STREQUAL "VTK_UNSIGNED_CHAR")
  set(labelmap_type "UCHAR")
endif (VTKEdge_PAINTBRUSH_LABEL_TYPE STREQUAL "VTK_UNSIGNED_CHAR")

add_test(PaintbrushTest1
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest1
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest1InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/EditedLabelMap.mha
  )
add_test(PaintbrushTest1b
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest1b
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest1bInputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/EditedLabelMap.mha
  )
add_test(PaintbrushTest1c
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest1c
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest1cInputEventLog.txt
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
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest3InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha
  )

add_test(PaintbrushTest4
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest4
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest4InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha
  )

add_test(PaintbrushTest5
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest5
  ${WIDGETS_DATA_ROOT}/Data/IBSRData.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest4InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha
  )

add_test(PaintbrushTest6
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest6
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-WhiteMatter-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest6InputEventLog.txt
  0
  )

add_test(PaintbrushTest7
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest7
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest7InputEventLog.txt
  0
  ${CMAKE_CURRENT_BINARY_DIR}/Temporary/EditedLabelMap.mha
  )

add_test(PaintbrushTest8
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushTest8
  ${WIDGETS_DATA_ROOT}/Data/IBSRDataCropped.mha
  ${WIDGETS_DATA_ROOT}/Data/IBSRLabelMap-4Labels-${labelmap_type}-Cropped.mha
  ${WIDGETS_DATA_ROOT}/Data/PaintbrushTest8InputEventLog.txt
  0
  )

add_test( PaintbrushInstantiationMemLeaksTest
  ${CXX_TEST_PATH}/${TESTBASE}CxxTests
  PaintbrushInstantiatonMemLeaksTest )
