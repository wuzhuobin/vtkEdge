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
// .NAME vtkKWEWireframePainter
// .SECTION Description

#ifndef __vtkKWEWireframePainter_h
#define __vtkKWEWireframePainter_h

#include "vtkOpenGLRepresentationPainter.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_RENDERING_EXPORT vtkKWEWireframePainter : public vtkOpenGLRepresentationPainter
{
public:
  static vtkKWEWireframePainter* New();
  vtkTypeRevisionMacro(vtkKWEWireframePainter, vtkOpenGLRepresentationPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release. In this case, releases the display lists.
  virtual void ReleaseGraphicsResources(vtkWindow *);

//BTX
protected:
  vtkKWEWireframePainter();
  ~vtkKWEWireframePainter();

  // Description:
  // Some subclasses may need to do some preprocessing
  // before the actual rendering can be done eg. build effecient
  // representation for the data etc. This should be done here.
  // This method get called after the ProcessInformation()
  // but before RenderInternal().
  virtual void PrepareForRendering(vtkRenderer*, vtkActor*);

  // Description:
  // Changes the polygon mode according to the representation.
  void RenderInternal(vtkRenderer* renderer, vtkActor* actor,
                      unsigned long typeflags, bool forceCompileOnly);

  // Description:
  // Returns true when rendering is possible.
  bool CanRender(vtkRenderer*, vtkActor*);

private:
  vtkKWEWireframePainter(const vtkKWEWireframePainter&); // Not implemented.
  void operator=(const vtkKWEWireframePainter&); // Not implemented.

  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif


