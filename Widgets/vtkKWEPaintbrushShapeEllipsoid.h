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
// .NAME vtkKWEPaintbrushShapeEllipsoid - A box shaped paintbrush template
// .SECTION Usage
// 
// .SECTION Description
// 
// .SECTION See Also

#ifndef __vtkKWEPaintbrushShapeEllipsoid_h
#define __vtkKWEPaintbrushShapeEllipsoid_h

#include "vtkKWEPaintbrushShape.h"

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushShapeEllipsoid
                              : public vtkKWEPaintbrushShape
{
public:
  
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushShapeEllipsoid *New();
  
  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushShapeEllipsoid, vtkKWEPaintbrushShape);
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
  // Get the shape as a binary stencil
  virtual void GetStencil(vtkImageStencilData *, double p[3]);

  // Description:
  // Get the shape as a grayscale image data
  virtual void GetGrayscaleData(vtkImageData  *, double p[3]); 
  
  //Description:
  //Set new shape width
  virtual void SetWidth( double newWidthX, double newWidthY, double newWidthZ ); 
  virtual void SetWidth( double newWidth[3]);
  vtkGetVector3Macro( Width, double );

  // Description:
  // Set/Get the resolution of the ellipse. The resolution is the number of
  // points on the polydata used to represent the ellipse.
  vtkSetMacro( Resolution, int );
  vtkGetMacro( Resolution, int );

  // Description:
  // See superclass documentation
  virtual int Resize(double d[3], int ResizeType);

  // Description:
  // Deep copy.. Synchronizes states etc.
  virtual void DeepCopy(vtkKWEPaintbrushShape *);
 
  // Description:
  // Retrieve text annotation. This returns the size of the box
  virtual void GetAnnotation(char *s);

  // Description:
  // Transition region for the grayscale brush. Default 20% of the brush 
  // radius on either side of the brush.
  vtkSetClampMacro( TransitionRegion, double, 0.0, 1.0 );
  vtkGetMacro( TransitionRegion, double );

  // Description:
  // INTERNAL - Do not use.
  // Get the extents of a shape drawn at the current location.
  virtual void GetExtent( int extent[6], double p[3] );
  
protected:
  vtkKWEPaintbrushShapeEllipsoid();
  ~vtkKWEPaintbrushShapeEllipsoid();

  double         Width[3];
  int            Resolution;
  double         TransitionRegion;
  
private:
  vtkKWEPaintbrushShapeEllipsoid(const vtkKWEPaintbrushShapeEllipsoid&);  //Not implemented
  void operator=(const vtkKWEPaintbrushShapeEllipsoid&);  //Not implemented
};

#endif
