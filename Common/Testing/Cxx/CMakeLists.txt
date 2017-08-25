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
set(KIT Common)

set(TESTBASE ${PROJECT_NAME}${KIT})

# -----------------------------------------------------------------------------
# First, list all the tests that either use no data, or only data from the
# VTKEdge data directory (which always exists since it is part of the
# VTKEdge repository). These will go into one test executable.
# -----------------------------------------------------------------------------
set(MyTests
  )

create_test_sourcelist(Tests
  ${TESTBASE}CxxTests.cxx
  ${MyTests}
  EXTRA_INCLUDE vtkTestDriver.h
  )

add_executable(${TESTBASE}CxxTests ${Tests})
#install_targets(${VTKEdge_INSTALL_BIN_DIR} ${TESTBASE}CxxTests)

target_link_libraries(${TESTBASE}CxxTests vtkKWECommon)
set(TestsToRun ${Tests})
remove(TestsToRun ${TESTBASE}CxxTests.cxx)

# Add all the executables
foreach(test ${TestsToRun})
  get_filename_component(TName ${test} NAME_WE)
  add_test(${TName} ${CXX_TEST_PATH}/${TESTBASE}CxxTests ${TName}
    -D ${PROJECT_SOURCE_DIR}/Data
    -T ${PROJECT_BINARY_DIR}/Testing/Temporary
    -V ${PROJECT_SOURCE_DIR}/Data/Baseline/${KIT}/${TName}.png
    )
endforeach(test)
