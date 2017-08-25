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
// .NAME vtkKWEXGPUInfoList - Get GPUs VRAM information using X server
// extensions.
// .SECTION Description
// vtkKWEXGPUInfoList implements Probe() method of vtkKWEGPUInfoList
// through some X server extensions API. NV-CONTROL for Nvidia.
// ATIFGLEXTENSION for ATI is not supported yet.
// There is no support for other vendors.
// .SECTION See Also
// vtkKWEGPUInfo vtkKWEGPUInfoList

#ifndef __vtkKWEXGPUInfoList_h
#define __vtkKWEXGPUInfoList_h

#include "vtkKWEGPUInfoList.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_COMMON_EXPORT vtkKWEXGPUInfoList : public vtkKWEGPUInfoList
{
public:
  static vtkKWEXGPUInfoList* New();
  vtkTypeRevisionMacro(vtkKWEXGPUInfoList, vtkKWEGPUInfoList);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Build the list of vtkKWEInfoGPU if not done yet.
  // \post probed: IsProbed()
  virtual void Probe();
  
protected:
  // Description:
  // Default constructor.
  vtkKWEXGPUInfoList();
  virtual ~vtkKWEXGPUInfoList();
 
private:
  vtkKWEXGPUInfoList(const vtkKWEXGPUInfoList&); // Not implemented.
  void operator=(const vtkKWEXGPUInfoList&); // Not implemented.
};

#endif
