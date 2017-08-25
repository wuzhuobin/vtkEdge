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
// .NAME vtkKWEDummyGPUInfoList - Do thing during Probe()
// .SECTION Description
// vtkKWEDummyGPUInfoList implements Probe() by just setting the count of
// GPUs to be zero. Useful when an OS specific implementation is not available.
// .SECTION See Also
// vtkKWEGPUInfo vtkKWEGPUInfoList

#ifndef __vtkKWEDummyGPUInfoList_h
#define __vtkKWEDummyGPUInfoList_h

#include "vtkKWEGPUInfoList.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_COMMON_EXPORT vtkKWEDummyGPUInfoList : public vtkKWEGPUInfoList
{
public:
  static vtkKWEDummyGPUInfoList* New();
  vtkTypeRevisionMacro(vtkKWEDummyGPUInfoList, vtkKWEGPUInfoList);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Build the list of vtkKWEInfoGPU if not done yet.
  // \post probed: IsProbed()
  virtual void Probe();
  
protected:
  // Description:
  // Default constructor.
  vtkKWEDummyGPUInfoList();
  virtual ~vtkKWEDummyGPUInfoList();
  
private:
  vtkKWEDummyGPUInfoList(const vtkKWEDummyGPUInfoList&); // Not implemented.
  void operator=(const vtkKWEDummyGPUInfoList&); // Not implemented.
};

#endif
