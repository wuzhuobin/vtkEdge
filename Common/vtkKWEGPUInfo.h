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
// .NAME vtkKWEGPUInfo - Stores GPU VRAM information.
// .SECTION Description
// vtkKWEGPUInfo stores information about GPU Video RAM. An host can have
// several GPUs. The values are set by vtkKWEGPUInfoList.
// .SECTION See Also
// vtkKWEGPUInfoList vtkKWEDirectXGPUInfoList vtkKWECoreGraphicsGPUInfoList

#ifndef __vtkKWEGPUInfo_h
#define __vtkKWEGPUInfo_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_COMMON_EXPORT vtkKWEGPUInfo : public vtkObject
{
public:
  static vtkKWEGPUInfo* New();
  vtkTypeRevisionMacro(vtkKWEGPUInfo, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Set/Get dedicated video memory in bytes. Initial value is 0.
  // Usually the fastest one. If it is not null, it should be take into
  // account first and DedicatedSystemMemory or SharedSystemMemory should be
  // ignored.
  vtkSetMacro(DedicatedVideoMemory,vtkIdType);
  vtkGetMacro(DedicatedVideoMemory,vtkIdType);
  
  // Description:
  // Set/Get dedicated system memory in bytes. Initial value is 0.
  // This is slow memory. If it is not null, this value should be taken into
  // account only if there is no DedicatedVideoMemory and SharedSystemMemory
  // should be ignored.
  vtkSetMacro(DedicatedSystemMemory,vtkIdType);
  vtkGetMacro(DedicatedSystemMemory,vtkIdType);
  
  // Description:
  // Set/Get shared system memory in bytes. Initial value is 0.
  // Slowest memory. This value should be taken into account only if there is
  // neither DedicatedVideoMemory nor DedicatedSystemMemory.
  vtkSetMacro(SharedSystemMemory,vtkIdType);
  vtkGetMacro(SharedSystemMemory,vtkIdType);
  
protected:
  vtkKWEGPUInfo();
  ~vtkKWEGPUInfo();

  vtkIdType DedicatedVideoMemory;
  vtkIdType DedicatedSystemMemory;
  vtkIdType SharedSystemMemory;
  
private:
  vtkKWEGPUInfo(const vtkKWEGPUInfo&); // Not implemented.
  void operator=(const vtkKWEGPUInfo&); // Not implemented.
};

#endif
