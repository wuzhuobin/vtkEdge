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
# Just add all the subdirectories
# -----------------------------------------------------------------------------
set(kits
  Filtering
  Hybrid
  Rendering
  )
foreach(kit ${kits})
  if(VTKEdge_BUILD_${kit}_KIT)
    add_subdirectory(${kit})
  endif(VTKEdge_BUILD_${kit}_KIT)
endforeach(kit)

if (VTKEdge_BUILD_Widgets_KIT)
  subdirs( GUI )
endif(VTKEdge_BUILD_Widgets_KIT)

