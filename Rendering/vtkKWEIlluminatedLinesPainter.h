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
// .NAME vtkKWEIlluminatedLinesPainter - painter that paints lines.
// .SECTION Description
// This painter tries to paint lines efficiently. Request to Render
// any other primitive are ignored and not passed to the delegate painter,
// if any. This painter cannot handle cell colors/normals. If they are
// present the request is passed on to the Delegate painter. If this
// class is able to render the primitive, the render request is not
// propagated to the delegate painter.
//

#ifndef __vtkKWEIlluminatedLinesPainter_h
#define __vtkKWEIlluminatedLinesPainter_h

#include "vtkLinesPainter.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkInformationIntegerKey;
class vtkShaderProgram2;
class vtkShader2;
class vtkPainterPolyDataMapper;
class vtkRenderWindow;
class vtkWindow;

class VTKEdge_RENDERING_EXPORT vtkKWEIlluminatedLinesPainter : public vtkLinesPainter
{
public:
  static vtkKWEIlluminatedLinesPainter* New();
  vtkTypeRevisionMacro(vtkKWEIlluminatedLinesPainter, vtkLinesPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Release any graphics resources that are being consumed by this painter.
  // The parameter window could be used to determine which graphic
  // resources to release.
  // The call is propagated to the delegate painter, if any.
  virtual void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Key used to enable/disable this painter.
  static vtkInformationIntegerKey* ENABLE();

  // Description:
  // Initializes the mapper with a new instance of vtkKWEIlluminatedLinesPainter
  // inserted at the correct location.
  static void UpdateMapper(vtkPainterPolyDataMapper* mapper);

  // Description:
  // Returns if the context supports the required extensions.
  static bool IsSupported(vtkRenderWindow* renWin);
//BTX
protected:
  vtkKWEIlluminatedLinesPainter();
  ~vtkKWEIlluminatedLinesPainter();

  // Description:
  // Called before RenderInternal() if the Information has been changed
  // since the last time this method was called.
  virtual void ProcessInformation(vtkInformation*);

  // Description:
  // Some subclasses may need to do some preprocessing
  // before the actual rendering can be done eg. build effecient
  // representation for the data etc. This should be done here.
  // This method get called after the ProcessInformation()
  // but before RenderInternal().
  virtual void PrepareForRendering(vtkRenderer*, vtkActor*);

  // Description:
  // The actual rendering happens here. This method is called only when
  // SupportedPrimitive is present in typeflags when Render() is invoked.
  virtual int RenderPrimitive(unsigned long flags, vtkDataArray* n,
    vtkUnsignedCharArray* c, vtkDataArray* t, vtkRenderer* ren);

  vtkSetMacro(Enable, int);
  int Enable;
  vtkWeakPointer<vtkWindow> LastRenderWindow;
  vtkShaderProgram2* Program;
  vtkShader2* FragmentShader;

private:
  vtkKWEIlluminatedLinesPainter(const vtkKWEIlluminatedLinesPainter&); // Not implemented.
  void operator=(const vtkKWEIlluminatedLinesPainter&); // Not implemented.
//ETX
};



#endif

