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
#include "vtkKWEDummyGPUInfoList.h"

#include "vtkKWEGPUInfoListArray.h"
#include "vtkObjectFactory.h"
#include <assert.h>

vtkCxxRevisionMacro(vtkKWEDummyGPUInfoList, "$Revision: 549 $");
vtkStandardNewMacro(vtkKWEDummyGPUInfoList);

// ----------------------------------------------------------------------------
// Description:
// Build the list of vtkKWEInfoGPU if not done yet.
// \post probed: IsProbed()
void vtkKWEDummyGPUInfoList::Probe()
{
  if(!this->Probed)
    {
    this->Probed=true;
    this->Array=new vtkKWEGPUInfoListArray;
    this->Array->v.resize(0); // no GPU.
    }
  assert("post: probed" && this->IsProbed());
}

// ----------------------------------------------------------------------------
vtkKWEDummyGPUInfoList::vtkKWEDummyGPUInfoList()
{
}

// ----------------------------------------------------------------------------
vtkKWEDummyGPUInfoList::~vtkKWEDummyGPUInfoList()
{
}

// ----------------------------------------------------------------------------
void vtkKWEDummyGPUInfoList::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
