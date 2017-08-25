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
// .NAME vtkKWESurfaceLICPainter - painter that performs LIC on the surface of
// arbitrary geometry.
// .SECTION Description
// vtkKWESurfaceLICPainter painter performs LIC on the surface of arbitrary
// geometry. Point vectors are used as the vector field for generating the LIC.
// The implementation is based on "Image Space Based Visualization on Unstread
// Flow on Surfaces" by Laramee, Jobard and Hauser appered in proceedings of 
// IEEE Visualization '03, pages 131-138.

#ifndef __vtkKWESurfaceLICPainter_h
#define __vtkKWESurfaceLICPainter_h

#include "vtkPolyDataPainter.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkRenderWindow;
class VTKEdge_RENDERING_EXPORT vtkKWESurfaceLICPainter : public vtkPolyDataPainter
{
public:
  static vtkKWESurfaceLICPainter* New();
  vtkTypeRevisionMacro(vtkKWESurfaceLICPainter, vtkPolyDataPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release. In this case, releases the display lists.
  virtual void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Get the output data object from this painter. 
  // Overridden to pass the input points (or cells) vectors as the tcoords to
  // the deletage painters. This is required by the internal GLSL shader
  // programs used for generating LIC.
  virtual vtkDataObject* GetOutput();

  // Description:
  // Enable/Disable this painter.
  vtkSetMacro(Enable, int);
  vtkGetMacro(Enable, int);
  vtkBooleanMacro(Enable, int);

  // Description:
  // Set the vectors to used for applying LIC. By default point vectors are
  // used. Arguments are same as those passed to
  // vtkAlgorithm::SetInputArrayToProcess except the first 3 arguments i.e. idx,
  // port, connection.
  void SetInputArrayToProcess(int fieldAssociation, const char *name);
  void SetInputArrayToProcess(int fieldAssociation, int fieldAttributeType);

  // Description:
  // Get/Set the number of integration steps in each direction.
  vtkSetMacro(NumberOfSteps, int);
  vtkGetMacro(NumberOfSteps, int);

  // Description:
  // Get/Set the step size (in pixels).
  vtkSetMacro(StepSize, double);
  vtkGetMacro(StepSize, double);

  // Description:
  // Control the contribution of the LIC in the final output image.
  // 0.0 produces same result as disabling LIC alltogether, while 1.0 implies
  // show LIC result alone.
  vtkSetClampMacro(LICIntensity, double, 0.0, 1.0);
  vtkGetMacro(LICIntensity, double);

  // Description:
  // Returns true is the rendering context supports extensions needed by this
  // painter.
  static bool IsSupported(vtkRenderWindow*);
//BTX
protected:
  vtkKWESurfaceLICPainter();
  ~vtkKWESurfaceLICPainter();

  // Description:
  // Take part in garbage collection.
  virtual void ReportReferences(vtkGarbageCollector *collector);

  // Description:
  // Some subclasses may need to do some preprocessing
  // before the actual rendering can be done eg. build effecient
  // representation for the data etc. This should be done here.
  // This method get called after the ProcessInformation()
  // but before RenderInternal().
  virtual void PrepareForRendering(vtkRenderer*, vtkActor*);

  // Description:
  // Performs the actual rendering. Subclasses may override this method.
  // default implementation merely call a Render on the DelegatePainter,
  // if any. When RenderInternal() is called, it is assured that the 
  // DelegatePainter is in sync with this painter i.e. UpdateDelegatePainter()
  // has been called.
  virtual void RenderInternal(vtkRenderer* renderer, vtkActor* actor, 
                              unsigned long typeflags, bool forceCompileOnly);

  // Description:
  // Prepares output data. Returns true if vectors are available.
  bool PrepareOutput();

  // Description:
  // Returns true when rendering LIC is possible.
  bool CanRenderLIC(vtkRenderer*, vtkActor*);

  // Unit is a pixel length.
  double StepSize;
  int NumberOfSteps;

  double LICIntensity;
  int Enable;
private:
  vtkKWESurfaceLICPainter(const vtkKWESurfaceLICPainter&); // Not implemented.
  void operator=(const vtkKWESurfaceLICPainter&); // Not implemented.

  vtkDataObject* Output;
  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif


