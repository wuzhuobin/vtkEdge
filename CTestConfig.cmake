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

set(CTEST_PROJECT_NAME "VTKEdge")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "kitware.com")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=VTKEdge")
set(CTEST_DROP_SITE_CDASH TRUE)
