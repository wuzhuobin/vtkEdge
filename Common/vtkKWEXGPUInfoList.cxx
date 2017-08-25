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
#include "vtkKWEXGPUInfoList.h"

#include "vtkKWEGPUInfoListArray.h"
#include "vtkObjectFactory.h"
#include <assert.h>

#include <X11/Xlib.h> // Display structure, XOpenDisplay(), XScreenCount()
#ifdef VTKEdge_USE_NVCONTROL
#include "NVCtrlLib.h" // needs NVCtrl.h (NV_CTRL_VIDEO_RAM, XNVCTRLQueryAttribute() )
#endif

vtkCxxRevisionMacro(vtkKWEXGPUInfoList, "$Revision: 727 $");
vtkStandardNewMacro(vtkKWEXGPUInfoList);

// ----------------------------------------------------------------------------
// Description:
// Build the list of vtkKWEInfoGPU if not done yet.
// \post probed: IsProbed()
void vtkKWEXGPUInfoList::Probe()
{
  if(!this->Probed)
    {
    this->Probed=true;
    this->Array=new vtkKWEGPUInfoListArray;
    bool found=false;
    
#ifdef VTKEdge_USE_NVCONTROL
    // see sample code in nvidia-settings-1.0/samples/nv-control-info.c
    Display *dpy=XOpenDisplay(NULL); // we use the environment variable DISPLAY
    if(dpy!=NULL)
      {
      int eventBase;
      int errorBase;
      if(XNVCTRLQueryExtension(dpy,&eventBase,&errorBase)==True)
        {
        int screenCount=XScreenCount(dpy);
        int nvScreenCount=0;
        int i=0;
        while(i<screenCount)
          {
          if(XNVCTRLIsNvScreen(dpy,i))
            {
            ++nvScreenCount;
            }
          ++i;
          }
        found=nvScreenCount>0;
        if(found)
          {
          this->Array->v.resize(nvScreenCount);
          int j=0;
          i=0;
          while(i<screenCount)
            {
            if(XNVCTRLIsNvScreen(dpy,i))
              {
              int ramSize;
              Bool status=XNVCTRLQueryAttribute(dpy,i,0,
                                                NV_CTRL_VIDEO_RAM,&ramSize);
              if(!status)
                {
                ramSize=0;
                }
              vtkKWEGPUInfo *info=vtkKWEGPUInfo::New();
              info->SetDedicatedVideoMemory(ramSize*1024); // ramSize is in KB
              this->Array->v[j]=info;
              ++j;
              }
            ++i;
            }
          }
        }
      XCloseDisplay(dpy);
      }
#endif // #ifdef VTKEdge_USE_NVCONTROL
    if(!found)
      {
      this->Array->v.resize(0); // no GPU.
      }
    }
  assert("post: probed" && this->IsProbed());
}


// ----------------------------------------------------------------------------
vtkKWEXGPUInfoList::vtkKWEXGPUInfoList()
{
}

// ----------------------------------------------------------------------------
vtkKWEXGPUInfoList::~vtkKWEXGPUInfoList()
{
}

// ----------------------------------------------------------------------------
void vtkKWEXGPUInfoList::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
