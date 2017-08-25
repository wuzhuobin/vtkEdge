//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2010 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the BSD License
//   Please see the file Copyright.txt in the root directory of
//   VTKEdge for further information.
//
//   Alternatively, you may see: 
//
//   http://www.vtkedge.org/vtkedge/project/license.html
//
//
//   For custom extensions, consulting services, or training for
//   this or any other Kitware supported open source project, please
//   contact Kitware at sales@kitware.com.
//
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

