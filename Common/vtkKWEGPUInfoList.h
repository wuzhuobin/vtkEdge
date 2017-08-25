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
// .NAME vtkKWEGPUInfoList - Stores the list of GPUs VRAM information.
// .SECTION Description
// vtkKWEGPUInfoList stores a list of vtkKWEGPUInfo. An host can have
// several GPUs. It creates and sets the list by probing the host with system
// calls. This an abstract class. Concrete classes are OS specific.
// .SECTION See Also
// vtkKWEGPUInfo vtkKWEDirectXGPUInfoList vtkKWECoreGraphicsGPUInfoList

#ifndef __vtkKWEGPUInfoList_h
#define __vtkKWEGPUInfoList_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWEGPUInfoListArray; // STL Pimpl
class vtkKWEGPUInfo;

class VTKEdge_COMMON_EXPORT vtkKWEGPUInfoList : public vtkObject
{
public:
  static vtkKWEGPUInfoList *New();
  vtkTypeRevisionMacro(vtkKWEGPUInfoList, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Build the list of vtkKWEInfoGPU if not done yet.
  // Default implementation created an empty list. Useful if there is no
  // implementation available for a given architecture yet.
  // \post probed: IsProbed()
  virtual void Probe()=0;
  
  // Description:
  // Tells if the operating system has been probed. Initial value is false.
  virtual bool IsProbed();
  
  // Description:
  // Return the number of GPUs.
  // \pre probed: IsProbed()
  virtual int GetNumberOfGPUs();
  
  // Description:
  // Return information about GPU i.
  // \pre probed: IsProbed()
  // \pre valid_index: i>=0 && i<GetNumberOfGPUs()
  // \post result_exists: result!=0
  virtual vtkKWEGPUInfo *GetGPUInfo(int i);
  
protected:
  // Description:
  // Default constructor. Set Probed to false. Set Array to NULL.
  vtkKWEGPUInfoList();
  virtual ~vtkKWEGPUInfoList();
  
  bool Probed;
  vtkKWEGPUInfoListArray *Array;
  
private:
  vtkKWEGPUInfoList(const vtkKWEGPUInfoList&); // Not implemented.
  void operator=(const vtkKWEGPUInfoList&); // Not implemented.
};

#endif
