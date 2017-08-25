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
// .NAME vtkSMKWEIlluminatedLinesRepresentationProxy - representation proxy for
// adding support for vtkKWEIlluminatedLinesPainter in ParaView
// .SECTION Description
// vtkSMKWEIlluminatedLinesRepresentationProxy is a representation proxy that
// adds support for vtkKWEIlluminatedLinesPainter in ParaView.

#ifndef __vtkSMKWEIlluminatedLinesRepresentationProxy_h
#define __vtkSMKWEIlluminatedLinesRepresentationProxy_h

#include "vtkSMSurfaceRepresentationProxy.h"

class vtkSMKWEIlluminatedLinesRepresentationProxy : public vtkSMSurfaceRepresentationProxy
{
public:
  static vtkSMKWEIlluminatedLinesRepresentationProxy* New();
  vtkTypeRevisionMacro(vtkSMKWEIlluminatedLinesRepresentationProxy, vtkSMSurfaceRepresentationProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

//BTX
protected:
  vtkSMKWEIlluminatedLinesRepresentationProxy();
  ~vtkSMKWEIlluminatedLinesRepresentationProxy();

  // Description:
  // Internal method to update actual diffuse/specular/ambient coefficients used
  // based on the representation.
  virtual void UpdateShadingParameters();

  // Description:
  // This method is called after CreateVTKObjects(). 
  // This gives subclasses an opportunity to do some post-creation
  // initialization.
  virtual bool EndCreateVTKObjects();
private:
  vtkSMKWEIlluminatedLinesRepresentationProxy(const vtkSMKWEIlluminatedLinesRepresentationProxy&); // Not implemented
  void operator=(const vtkSMKWEIlluminatedLinesRepresentationProxy&); // Not implemented
//ETX
};

#endif

