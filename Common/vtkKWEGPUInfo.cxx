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

#include "vtkKWEGPUInfo.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEGPUInfo, "$Revision: 549 $");
vtkStandardNewMacro(vtkKWEGPUInfo);

// ----------------------------------------------------------------------------
vtkKWEGPUInfo::vtkKWEGPUInfo()
{
  this->DedicatedVideoMemory=0;
  this->DedicatedSystemMemory=0;
  this->SharedSystemMemory=0;
}

// ----------------------------------------------------------------------------
vtkKWEGPUInfo::~vtkKWEGPUInfo()
{
}

// ----------------------------------------------------------------------------
void vtkKWEGPUInfo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dedicated Video Memory in bytes: " << this->DedicatedVideoMemory
     << endl;
  os << indent << "Dedicated System Memory in bytes: " << this->DedicatedSystemMemory
     << endl;
  os << indent << "Shared System Memory in bytes: " << this->SharedSystemMemory
     << endl;
}
