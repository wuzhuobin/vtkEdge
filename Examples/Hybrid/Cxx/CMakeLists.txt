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

set(exe ${VTKEdge_EXAMPLE_EXECUTABLE_PREFIX}GPUArrayCalculatorDemo)
add_executable(${exe} GPUArrayCalculatorDemo.cxx)
target_link_libraries(${exe} vtkKWEHybrid)
install_targets(${VTKEdge_INSTALL_BIN_DIR} ${exe})
