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
// .NAME vtkKWECoreGraphicsGPUInfoList - Get GPUs VRAM information using CoreGraphics.
// .SECTION Description
// vtkKWECoreGraphicsGPUInfoList implements Probe() method of vtkKWEGPUInfoList
// through the CoreGraphics API (Apple Mac OS X) and the IOKit API.
// The CoreGraphics framework is part of the ApplicationServices umbrella
// framework. IOKit is a framework on its own.
// ref: http://developer.apple.com/qa/qa2004/qa1168.html
// .SECTION See Also
// vtkKWEGPUInfo vtkKWEGPUInfoList

#ifndef __vtkKWECoreGraphicsGPUInfoList_h
#define __vtkKWECoreGraphicsGPUInfoList_h

#include "vtkKWEGPUInfoList.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_RENDERING_EXPORT vtkKWECoreGraphicsGPUInfoList : public vtkKWEGPUInfoList
{
public:
  static vtkKWECoreGraphicsGPUInfoList* New();
  vtkTypeRevisionMacro(vtkKWECoreGraphicsGPUInfoList, vtkKWEGPUInfoList);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Build the list of vtkKWEInfoGPU if not done yet.
  // \post probed: IsProbed()
  virtual void Probe();
  
protected:
  // Description:
  // Default constructor.
  vtkKWECoreGraphicsGPUInfoList();
  virtual ~vtkKWECoreGraphicsGPUInfoList();
  
private:
  vtkKWECoreGraphicsGPUInfoList(const vtkKWECoreGraphicsGPUInfoList&); // Not implemented.
  void operator=(const vtkKWECoreGraphicsGPUInfoList&); // Not implemented.
};

#endif
