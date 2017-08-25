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

#ifndef __vtkSMKWESurfaceLICRepresentationProxy_h
#define __vtkSMKWESurfaceLICRepresentationProxy_h

#include "vtkSMSurfaceRepresentationProxy.h"

// .NAME vtkSMKWESurfaceLICRepresentationProxy
// .SECTION Description
//
class vtkSMKWESurfaceLICRepresentationProxy : public vtkSMSurfaceRepresentationProxy
{
public:
  static vtkSMKWESurfaceLICRepresentationProxy* New();
  vtkTypeRevisionMacro(vtkSMKWESurfaceLICRepresentationProxy, vtkSMSurfaceRepresentationProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  void  SelectInputVectors(int, int, int, int attributeMode, const char* name);

  // Description:
  // Set to 0 to disable using of LIC when interacting.
  void SetUseLICForLOD(int);

//BTX
protected:
  vtkSMKWESurfaceLICRepresentationProxy();
  ~vtkSMKWESurfaceLICRepresentationProxy();

  // Description:
  // This method is called at the beginning of CreateVTKObjects().
  // This gives the subclasses an opportunity to set the servers flags
  // on the subproxies.
  // If this method returns false, CreateVTKObjects() is aborted.
  virtual bool BeginCreateVTKObjects();

  // Description:
  // This method is called after CreateVTKObjects(). 
  // This gives subclasses an opportunity to do some post-creation
  // initialization.
  virtual bool EndCreateVTKObjects();
private:
  vtkSMKWESurfaceLICRepresentationProxy(const vtkSMKWESurfaceLICRepresentationProxy&); // Not implemented
  void operator=(const vtkSMKWESurfaceLICRepresentationProxy&); // Not implemented
//ETX
};

#endif

