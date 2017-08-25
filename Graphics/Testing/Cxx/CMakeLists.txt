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
# Name this kit - this will be used to form test executable name later
# Then we'll add in tests with various data or kit dependencies later.
# -----------------------------------------------------------------------------
set(KIT Graphics)

set(TESTBASE ${PROJECT_NAME}${KIT})

# -----------------------------------------------------------------------------
# First, list all the tests that have no dependencies and can always be run.
# Then we'll add in tests with various data or kit dependencies later.
# -----------------------------------------------------------------------------
set(MyTests
  TestDensifyPolyData.cxx
  TestTessellatedBoxSource.cxx
  )

create_test_sourcelist(Tests
  ${TESTBASE}CxxTests.cxx
  ${MyTests}
  EXTRA_INCLUDE vtkTestDriver.h
  )

add_executable(${TESTBASE}CxxTests ${Tests})
#install_targets(${VTKEdge_INSTALL_BIN_DIR} ${TESTBASE}CxxTests)

target_link_libraries(${TESTBASE}CxxTests vtkRendering vtkIO vtkKWEGraphics)
set(TestsToRun ${Tests})
remove(TestsToRun ${TESTBASE}CxxTests.cxx)

# Add all the executables
foreach(test ${TestsToRun})
  get_filename_component(TName ${test} NAME_WE)
  if(VTK_DATA_ROOT)
    add_test(${TName} ${CXX_TEST_PATH}/${TESTBASE}CxxTests ${TName}
      -D ${VTK_DATA_ROOT}
      -T ${PROJECT_BINARY_DIR}/Testing/Temporary
      -V ${PROJECT_SOURCE_DIR}/Data/Baseline/${KIT}/${TName}.png)
  else(VTK_DATA_ROOT)
    add_test(${TName} ${CXX_TEST_PATH}/${TESTBASE}CxxTests ${TName})
  endif(VTK_DATA_ROOT)
endforeach(test)
