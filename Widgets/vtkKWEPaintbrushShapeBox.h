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
// .NAME vtkKWEPaintbrushShapeBox - A box shaped paintbrush template
// .SECTION Usage
//
// .SECTION Description
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushShapeBox_h
#define __vtkKWEPaintbrushShapeBox_h

#include "vtkKWEPaintbrushShape.h"

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushShapeBox
                              : public vtkKWEPaintbrushShape
{
public:

  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushShapeBox *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushShapeBox, vtkKWEPaintbrushShape);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // It allows the paintbrush representation to query the template for a
  // paintbrush outline centered at the specified point and intersecting
  // with the specified plane. If the plane is NULL, a polydata representation
  // of the template in 3D is returned. (a cube in this case).
  // Returns NULL if the plane does not intersect with the cube.
//BTX
  virtual vtkSmartPointer< vtkPolyData > GetShapePolyData( double *center,
                           vtkPlane *plane = NULL);
//ETX

  // Description:
  // Get the template as a stencil
  virtual void GetStencil(vtkImageStencilData *, double p[3]);
  virtual void GetGrayscaleData(vtkImageData  *, double p[3]);

  //Description:
  //Set new shape width
  virtual void SetWidth( double newWidthX, double newWidthY, double newWidthZ );
  virtual void SetWidth( double newWidth[3]);
  vtkGetVector3Macro( Width, double );

  // Description:
  // See superclass documentation
  virtual int Resize(double d[3], int ResizeType);

  // Description:
  // Deep copy.. Synchronizes states etc.
  virtual void DeepCopy(vtkKWEPaintbrushShape *);

  // Description:
  // Provide some kind of contextual annotation about the shape that
  // indicates its size etc..
  virtual void GetAnnotation(char *);

  // Description:
  // Check if point is inside the shape
  virtual int IsInside(double currentPos[3], double wolrdPos[3]);

  // Description:
  // INTERNAL - Do not use.
  // Get the extents of a shape drawn at the current location.
  virtual void GetExtent( int extent[6], double p[3] );

protected:
  vtkKWEPaintbrushShapeBox();
  ~vtkKWEPaintbrushShapeBox();

  double         Width[3];

private:
  vtkKWEPaintbrushShapeBox(const vtkKWEPaintbrushShapeBox&);  //Not implemented
  void operator=(const vtkKWEPaintbrushShapeBox&);  //Not implemented
};

#endif
