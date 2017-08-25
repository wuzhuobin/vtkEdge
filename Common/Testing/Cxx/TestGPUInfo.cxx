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

#include "vtkTestUtilities.h"
#include "vtkKWEGPUInfoList.h"
#include "vtkKWEGPUInfo.h"

int TestGPUInfo(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  vtkKWEGPUInfoList *l=vtkKWEGPUInfoList::New();
  l->Probe();
  int c=l->GetNumberOfGPUs();
  
  cout << "There is " << c << " GPU(s)."<< endl;
  int i=0;
  while(i<c)
    {
    cout << " GPU " << i << ": " << endl;
    vtkKWEGPUInfo *info=l->GetGPUInfo(i);
    vtkIdType value;
    value=info->GetDedicatedVideoMemory();
    cout << "  dedicated VRAM=" << value/(1024*1024) << " MB" << endl;
    value=info->GetDedicatedSystemMemory();
    cout << "  dedicated RAM=" << value/(1024*1024) << " MB" << endl;
    value=info->GetSharedSystemMemory();
    cout << "  shared RAM=" << value/(1024*1024) << " MB" << endl;
    ++i;
    }
  l->Delete();
  
  return 0; // 0==never fails.
}
