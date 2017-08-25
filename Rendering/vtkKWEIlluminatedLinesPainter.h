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

