//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2008 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the GNU General Public License 
//   version 3 as published by the Free Software Foundation and appearing in 
//   the file LICENSE.txt included in the top level directory of this source
//   code distribution. Alternatively you may (at your option) use any later 
//   version of the GNU General Public License if such license has been 
//   publicly approved by Kitware, Inc. (or its successors, if any).
//
//   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
//   PURPOSE. See LICENSE.txt for additional details.
//
//   VTKEdge is available under alternative license terms. Please visit
//   vtkedge.org or contact us at kitware@kitware.com for further information.
//
//=============================================================================

#include "vtkObjectFactory.h"

#include "vtkKWEVolumeRenderingFactory.h"
#include "vtkToolkits.h"
#include "vtkDebugLeaks.h"
#include "vtkGraphicsFactory.h"

// if using some sort of opengl, then include these files
#if defined(VTK_USE_OGLR) || defined(_WIN32) || defined(VTK_USE_COCOA) || defined(VTK_USE_CARBON)
#include "vtkKWEOpenGLGPUVolumeRayCastMapper.h"
#endif

#include "vtkCriticalSection.h"

#include "stdlib.h"

vtkCxxRevisionMacro(vtkKWEVolumeRenderingFactory, "$Revision: 196 $");
vtkStandardNewMacro(vtkKWEVolumeRenderingFactory);


vtkObject* vtkKWEVolumeRenderingFactory::CreateInstance(const char* vtkclassname )
{
  // first check the object factory
  vtkObject *ret = vtkObjectFactory::CreateInstance(vtkclassname);
  if (ret)
    {
    return ret;
    }
  // if the factory failed to create the object,
  // then destroy it now, as vtkDebugLeaks::ConstructClass was called
  // with vtkclassname, and not the real name of the class
#ifdef VTK_DEBUG_LEAKS
  vtkDebugLeaks::DestructClass(vtkclassname);
#endif
  const char *rl = vtkGraphicsFactory::GetRenderLibrary();
  

#if defined(VTK_USE_OGLR) || defined(_WIN32) || defined(VTK_USE_COCOA) || defined(VTK_USE_CARBON)
  if (!strcmp("OpenGL",rl) || !strcmp("Win32OpenGL",rl) || !strcmp("CarbonOpenGL",rl) || !strcmp("CocoaOpenGL",rl))
    {
    // GPU Ray Cast Mapper
    if(strcmp(vtkclassname, "vtkKWEGPUVolumeRayCastMapper") == 0)
      {
#if defined(VTK_USE_MANGLED_MESA)
      if (vtkGraphicsFactory::GetUseMesaClasses())
        {
        vtkGenericWarningMacro("No support for mesa in vtkKWEGPUVolumeRayCastMapper");
        return 0;
        }
#endif
      return vtkKWEOpenGLGPUVolumeRayCastMapper::New();
      }
    }
#endif

  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEVolumeRenderingFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
