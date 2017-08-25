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
// .NAME vtkKWEDirectXGPUInfoList - Get GPUs VRAM information using DirectX.
// .SECTION Description
// vtkKWEDirectXGPUInfoList implements Probe() method of vtkKWEGPUInfoList
// through the DirectX API. As recommended by Microsoft, the WMI interface is
// used for Windows XP and the DXGI interface is used for Windows Vista and
// later. (see documentation of VideoMemory sample of the DirectX SDK)
// ref: http://msdn.microsoft.com/en-us/library/cc308070(VS.85).aspx
// .SECTION See Also
// vtkKWEGPUInfo vtkKWEGPUInfoList

#ifndef __vtkKWEDirectXGPUInfoList_h
#define __vtkKWEDirectXGPUInfoList_h

#include "vtkKWEGPUInfoList.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include <d3d9.h> // DirectX, HMONITOR

class VTKEdge_COMMON_EXPORT vtkKWEDirectXGPUInfoList : public vtkKWEGPUInfoList
{
public:
  static vtkKWEDirectXGPUInfoList* New();
  vtkTypeRevisionMacro(vtkKWEDirectXGPUInfoList, vtkKWEGPUInfoList);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Build the list of vtkKWEInfoGPU if not done yet.
  // \post probed: IsProbed()
  virtual void Probe();
  
protected:
  // Description:
  // Default constructor.
  vtkKWEDirectXGPUInfoList();
  virtual ~vtkKWEDirectXGPUInfoList();
  
  // Description:
  // Probe the GPUs with the DXGI api (Windows Vista or later).
  // It returns true if it succeeded (DXGI API is supported and probing
  // succeeded)
  // \pre m_exists: m!=0
  // \pre info_exist: info!=0
  bool ProbeInfoWithDXGI(HMONITOR m,
                         vtkKWEGPUInfo *info);

  // Description:
  // Probe the GPUs with the WMI api (Windows XP or later).
  // \pre m_exists: m!=0
  // \pre info_exist: info!=0
  void ProbeInfoWithWMI(HMONITOR m,
                        vtkKWEGPUInfo *info);
  
  // Description:
  // Used by ProbeInfoWithWMI().
  // \pre pre hm_exists: hm!=0
  // \pre strDeviceID_exists: strDeviceID!=0
  // \pre cchDeviceID_is_positive: cchDeviceID>0
  bool GetDeviceIDFromHMonitor(HMONITOR hm,
                               WCHAR *strDeviceID,
                               int cchDeviceID);
  
private:
  vtkKWEDirectXGPUInfoList(const vtkKWEDirectXGPUInfoList&); // Not implemented.
  void operator=(const vtkKWEDirectXGPUInfoList&); // Not implemented.
};

#endif
