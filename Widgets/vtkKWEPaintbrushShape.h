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
// .NAME vtkKWEPaintbrushShape - An abstract class to represent a paintbrush shape
// .SECTION Description
// The class represents a paintbrush shape. It defines two pure virtual
// methods: \c GetStencil and \c GetShapePolyData. The first returns the shape
// stencil centered at a given point. The second returns the shape polydata
// in world coordinates intersecting with the specified plane. If the plane
// is NULL, it should return the shape polydata in 3D.
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushShape_h
#define __vtkKWEPaintbrushShape_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEPaintbrushEnums.h"
#include "vtkSmartPointer.h" //
#include "vtkPolyData.h"
#include "vtkObject.h"

class vtkImageStencilData;
class vtkImageData;
class vtkKWEPaintbrushData;
class vtkPlane;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushShape : public vtkObject
{
public:

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushShape, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // It allows the paintbrush representation to query the template for a
  // paintbrush outline centered at the specified point and intersecting
  // with the specified plane. If the plane is NULL, a polydata representation
  // of the template in 3D is returned.
  // Returns NULL if the plane does not intersect with the template.
//BTX
  virtual vtkSmartPointer< vtkPolyData > GetShapePolyData( double *center,
                               vtkPlane *plane = NULL) = 0;
//ETX

  // Description:
  // Get the template as a stencil or as a grayscale data.
  virtual void GetStencil(vtkImageStencilData *, double p[3]) = 0;
  virtual void GetGrayscaleData(vtkImageData  *, double p[3]) = 0;
  void GetPaintbrushData(vtkKWEPaintbrushData *, int x, int y, int z);
  void GetPaintbrushData(vtkKWEPaintbrushData *, double p[3]);

  // Description:
  // Set/Get methods to set the metadata of the image on which the template is
  // drawn. These must be set or bad things will happen.
  vtkSetVector3Macro( Spacing, double );
  vtkGetVector3Macro( Spacing, double );
  vtkSetVector3Macro( Origin,  double );
  vtkGetVector3Macro( Origin,  double );

  // Description:
  // Set the datatype of the grayscale data returned. (ie VTK_UNSIGNED_CHAR
  // etc)
  vtkSetMacro(ScalarType,int);
  vtkGetMacro(ScalarType,int);

  // Description:
  // Set the polarity of the shape. Positive by default. Positive shapes
  // are intended to draw. Negative shapes are intended to be used for
  // erasing.
  vtkSetMacro(Polarity, int);
  vtkGetMacro(Polarity, int);
  virtual void SetPolarityToDraw();
  virtual void SetPolarityToErase();

  //BTX
  // Enums that constrain resizing of the brush
  enum PaintbrushResizeEnum
    {
    PaintbrushResizeUnConstrained = 0,
    PaintbrushResizeIsotropic,
    PaintbrushResizeAnisotropic,
    PaintbrushResize_XY,
    PaintbrushResize_YZ,
    PaintbrushResize_XZ
    };
  //ETX

  // Description:
  // Optionally, you may set a constraint on the resizing of the brush. This
  // constraint is imposed during interactive resizing of the brush. See the
  // enum: PaintbrushResizeEnum.
  // Default is UnConstrained.
  vtkSetMacro( ResizeConstraint, int );
  vtkGetMacro( ResizeConstraint, int );

  // Description:
  // INTERNAL - Do not use.
  // Resize the shape. This method is only intended to be called from
  // the vtkKWEPaintbrushWidget class via user-interaction. If you wish to specify
  // an initial size for the brush, use the method SetWidth/SetRadius etc
  // that each concrete subclass provides.
  // 'ResizeType' must be one fo the PaintbrushResizeEnum's specified below.
  // Returns 1 if resize is successful. 0 otherwise.
  virtual int Resize(double factor[3], int ResizeType) = 0;

  // Description:
  // Deep copy.. Synchronizes states etc.
  virtual void DeepCopy(vtkKWEPaintbrushShape *);

  // Description:
  // Provide some kind of contextual annotation about the shape that
  // indicates its size etc..
  virtual void GetAnnotation(char *) = 0;

  // Description:
  // Check if point is inside the shape. Subclasses must implement this.
  virtual int IsInside(double vtkNotUsed(currPos)[3],
                       double vtkNotUsed(testPos)[3]) { return 1; }

  // Description:
  // Abstract method to get the width of the shape.
  virtual double *GetWidth() = 0;
  virtual void GetWidth (double &, double &, double &) = 0;

  // Description:
  // Set the shape representation. Either:
  //   vtkKWEPaintbrushEnums::Binary or
  //   vtkKWEPaintbrushEnums::Grayscale
  vtkSetMacro( Representation, int );
  vtkGetMacro( Representation, int );

  // Description:
  // Set the maximum width of the shape for each axe.
  vtkSetVector3Macro( MaxWidth,  double );
  vtkGetVector3Macro( MaxWidth,  double );

  // Description:
  // The etch produced by the shape can be sent through these clip extents, so
  // as to confine it to a certain axis aligned region. The default clip
  // extents are infinite.
  void SetClipExtent( int e[6] );
  void GetClipExtent( int e[6] );
  int * GetClipExtent();

  // Description:
  // INTERNAL - Do not use.
  // Get the extents of a shape drawn at the current location.
  virtual void GetExtent( int extent[6], double p[3] ) = 0;

protected:
  vtkKWEPaintbrushShape();
  ~vtkKWEPaintbrushShape();

  double         Spacing[3];
  double         Origin[3];
  int            ScalarType;
  int            Polarity;
  int            ResizeConstraint;
  int            Representation;
  double         MaxWidth[3];
  int            ClipExtent[6];

private:
  vtkKWEPaintbrushShape(const vtkKWEPaintbrushShape&);  //Not implemented
  void operator=(const vtkKWEPaintbrushShape&);  //Not implemented
};

#endif
