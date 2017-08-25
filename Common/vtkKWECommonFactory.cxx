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

#include "vtkKWECommonFactory.h"

#include "vtkObjectFactory.h"
#include "vtkToolkits.h"
#include "vtkVersion.h"

#include "vtkDebugLeaks.h"
#include "vtkKWEDummyGPUInfoList.h"
#ifdef VTKEdge_USE_DIRECTX // Windows
# include "vtkKWEDirectXGPUInfoList.h"
#else
# ifdef VTKEdge_USE_CORE_GRAPHICS // Mac
#  include "vtkKWECoreGraphicsGPUInfoList.h"
# endif
#endif

#ifdef VTKEdge_USE_NVCONTROL // Linux and X server extensions queries
# include "vtkKWEXGPUInfoList.h"
#endif

vtkStandardNewMacro(vtkKWECommonFactory);
vtkCxxRevisionMacro(vtkKWECommonFactory, "$Revision: 727 $");
//----------------------------------------------------------------------------
VTK_FACTORY_INTERFACE_IMPLEMENT(vtkKWECommonFactory);

//----------------------------------------------------------------------------
vtkKWECommonFactory::vtkKWECommonFactory()
{
}

// ----------------------------------------------------------------------------
vtkObject *vtkKWECommonFactory::CreateInstance(const char *vtkclassname)
{
  // first check the object factory
  vtkObject *ret=vtkObjectFactory::CreateInstance(vtkclassname);
  if(ret!=0)
    {
    return ret;
    }
  // if the factory failed to create the object,
  // then destroy it now, as vtkDebugLeaks::ConstructClass was called
  // with vtkclassname, and not the real name of the class
#ifdef VTK_DEBUG_LEAKS
  vtkDebugLeaks::DestructClass(vtkclassname);
#endif
  
  if(strcmp(vtkclassname, "vtkKWEGPUInfoList") == 0)
      {
#ifdef VTKEdge_USE_DIRECTX // Windows
      return vtkKWEDirectXGPUInfoList::New();     
#else
# ifdef VTKEdge_USE_CORE_GRAPHICS // Mac
      return vtkKWECoreGraphicsGPUInfoList::New();
# else
#  ifdef VTKEdge_USE_NVCONTROL // X11
      return vtkKWEXGPUInfoList::New();
#  else
      return vtkKWEDummyGPUInfoList::New();
#  endif
# endif
#endif
      }
  return 0;
}

//----------------------------------------------------------------------------
const char* vtkKWECommonFactory::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

//----------------------------------------------------------------------------
const char* vtkKWECommonFactory::GetDescription()
{
  return "VTKEdge Common Support Factory";
}

//----------------------------------------------------------------------------
void vtkKWECommonFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
