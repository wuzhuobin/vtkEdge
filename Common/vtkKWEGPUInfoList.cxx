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
#include "vtkKWEGPUInfoList.h"

#include "vtkKWEGPUInfoListArray.h"
#include <assert.h>
#include "vtkKWEGPUInfo.h"
#include "vtkKWECommonFactory.h"

vtkCxxRevisionMacro(vtkKWEGPUInfoList, "$Revision: 808 $");

// ----------------------------------------------------------------------------
// Needed when we don't use the vtkStandardNewMacro.
vtkInstantiatorNewMacro(vtkKWEGPUInfoList);

// ----------------------------------------------------------------------------
vtkKWEGPUInfoList *vtkKWEGPUInfoList::New()
{
  vtkObject *ret=vtkKWECommonFactory::CreateInstance("vtkKWEGPUInfoList");
  return static_cast<vtkKWEGPUInfoList *>(ret);
}

// ----------------------------------------------------------------------------
// Description:
// Tells if the operating system has been probed. Initial value is false.
bool vtkKWEGPUInfoList::IsProbed()
{
  return this->Probed;
}
  
// ----------------------------------------------------------------------------
// Description:
// Return the number of GPUs.
// \pre probed: IsProbed()
int vtkKWEGPUInfoList::GetNumberOfGPUs()
{
  assert("pre: probed" && this->IsProbed());
  return static_cast<int>(this->Array->v.size());
}
  
// ----------------------------------------------------------------------------
// Description:
// Return information about GPU i.
// \pre probed: IsProbed()
// \pre valid_index: i>=0 && i<GetNumberOfGPUs()
// \post result_exists: result!=0
vtkKWEGPUInfo *vtkKWEGPUInfoList::GetGPUInfo(int i)
{
  assert("pre: probed" && this->IsProbed());
  assert("pre: valid_index" && i>=0 && i<this->GetNumberOfGPUs());
  
  vtkKWEGPUInfo *result=this->Array->v[static_cast<size_t>(i)];
  assert("post: result_exists" && result!=0);
  return result;
}

// ----------------------------------------------------------------------------
// Description:
// Default constructor. Set Probed to false. Set Array to NULL.
vtkKWEGPUInfoList::vtkKWEGPUInfoList()
{
  this->Probed=false;
  this->Array=0;
}
    
// ----------------------------------------------------------------------------
vtkKWEGPUInfoList::~vtkKWEGPUInfoList()
{
  if(this->Array!=0)
    {
    size_t c=this->Array->v.size();
    size_t i=0;
    while(i<c)
      {
      this->Array->v[i]->Delete();
      ++i;
      }
    delete this->Array;
    }
}

// ----------------------------------------------------------------------------
void vtkKWEGPUInfoList::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "IsProbed: " << this->Probed << endl;
  if(this->Probed)
    {
    int c=this->GetNumberOfGPUs();
    os << indent << "Number of GPUs: " << c << endl;
    int i=0;
    while(i<c)
      {
      os << indent << " GPU " << i;
      this->GetGPUInfo(i)->PrintSelf(os,indent);
      ++i;
      }
    }
}
