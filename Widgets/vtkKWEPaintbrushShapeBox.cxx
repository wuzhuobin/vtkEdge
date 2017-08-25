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
#include "vtkKWEPaintbrushShapeBox.h"

#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkImageStencilData.h"
#include "vtkImageData.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkMath.h"

#define sign(x) ((x<0) ? (-1) : (1))

vtkCxxRevisionMacro(vtkKWEPaintbrushShapeBox, "$Revision: 3282 $");
vtkStandardNewMacro(vtkKWEPaintbrushShapeBox);

//----------------------------------------------------------------------
vtkKWEPaintbrushShapeBox::vtkKWEPaintbrushShapeBox()
{
  this->Width[0] = 2.0;
  this->Width[1] = 2.0;
  this->Width[2] = 2.0;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushShapeBox::~vtkKWEPaintbrushShapeBox()
{
}

//----------------------------------------------------------------------
vtkSmartPointer< vtkPolyData >
vtkKWEPaintbrushShapeBox::GetShapePolyData(
            double *center, vtkPlane *plane)
{
  if (plane == NULL)
    {
    // No Orientation specified. Return the whole Polydata. This is what will
    // be rendered on the volume widget

    vtkCubeSource * templateOutline = vtkCubeSource::New();
    templateOutline->SetCenter( center );
    templateOutline->SetXLength(this->Width[0]);
    templateOutline->SetYLength(this->Width[1]);
    templateOutline->SetZLength(this->Width[2]);
    templateOutline->Update();
    vtkSmartPointer< vtkPolyData > pd = templateOutline->GetOutput();
    templateOutline->Delete();
    return pd;

    }
  else
    {
    double normal[3], origin[3];
    plane->GetNormal(normal);
    plane->GetOrigin(origin);

    // Fast handlers for axis aligned planes.
    const double tolerance = 0.01;
    if ((fabs(normal[0])-1.0) < tolerance && fabs(normal[1]) < tolerance &&
         fabs(normal[2]) < tolerance)
      {

      if (fabs(origin[0] - center[0]) > this->Width[0]/2.0)
        {
        return NULL;
        }

      vtkSmartPointer< vtkPolyData > templateOutline
                  = vtkSmartPointer< vtkPolyData >::New();
      templateOutline->Allocate(1, 1);

      vtkPoints * points = vtkPoints::New();
      points->InsertNextPoint( center[0],
                               center[1] - this->Width[1]/2.0,
                               center[2] - this->Width[2]/2.0);
      points->InsertNextPoint( center[0],
                               center[1] - this->Width[1]/2.0,
                               center[2] + this->Width[2]/2.0);
      points->InsertNextPoint( center[0],
                               center[1] + this->Width[1]/2.0,
                               center[2] + this->Width[2]/2.0);
      points->InsertNextPoint( center[0],
                               center[1] + this->Width[1]/2.0,
                               center[2] - this->Width[2]/2.0);
      templateOutline->SetPoints(points);
      vtkIdType ptIds[4];
      ptIds[0] = 0; ptIds[1] = 1; ptIds[2] = 2; ptIds[3] = 3;
      templateOutline->InsertNextCell(VTK_QUAD, 4, ptIds);
      points->Delete();

      return templateOutline;
      }
    else if (fabs(normal[0]) < tolerance && (fabs(normal[1])-1.0) < tolerance &&
        fabs(normal[2]) < tolerance)
      {

      if (fabs(origin[1] - center[1]) > this->Width[1]/2.0)
        {
        return NULL;
        }

      vtkSmartPointer< vtkPolyData > templateOutline
                  = vtkSmartPointer< vtkPolyData >::New();
      templateOutline->Allocate(1, 1);

      vtkPoints * points = vtkPoints::New();
      points->InsertNextPoint( center[0] - this->Width[0]/2.0,
                               center[1],
                               center[2] - this->Width[2]/2.0);
      points->InsertNextPoint( center[0] - this->Width[0]/2.0,
                               center[1],
                               center[2] + this->Width[2]/2.0);
      points->InsertNextPoint( center[0] + this->Width[0]/2.0,
                               center[1],
                               center[2] + this->Width[2]/2.0);
      points->InsertNextPoint( center[0] + this->Width[0]/2.0,
                               center[1],
                               center[2] - this->Width[2]/2.0);
      templateOutline->SetPoints(points);
      vtkIdType ptIds[4];
      ptIds[0] = 0; ptIds[1] = 1; ptIds[2] = 2; ptIds[3] = 3;
      templateOutline->InsertNextCell(VTK_QUAD, 4, ptIds);
      points->Delete();

      return templateOutline;
      }
    else if (fabs(normal[0]) < tolerance && fabs(normal[1]) < tolerance &&
             (fabs(normal[2])-1.0) < tolerance)
      {

      if (fabs(origin[2] - center[2]) > this->Width[2]/2.0)
        {
        return NULL;
        }

      vtkSmartPointer< vtkPolyData > templateOutline
                  = vtkSmartPointer< vtkPolyData >::New();
      templateOutline->Allocate(1, 1);

      vtkPoints * points = vtkPoints::New();
      points->InsertNextPoint( center[0] - this->Width[0]/2.0,
                               center[1] - this->Width[1]/2.0,
                               center[2]);
      points->InsertNextPoint( center[0] - this->Width[0]/2.0,
                               center[1] + this->Width[1]/2.0,
                               center[2]);
      points->InsertNextPoint( center[0] + this->Width[0]/2.0,
                               center[1] + this->Width[1]/2.0,
                               center[2]);
      points->InsertNextPoint( center[0] + this->Width[0]/2.0,
                               center[1] - this->Width[1]/2.0,
                               center[2]);
      templateOutline->SetPoints(points);
      vtkIdType ptIds[4];
      ptIds[0] = 0; ptIds[1] = 1; ptIds[2] = 2; ptIds[3] = 3;
      templateOutline->InsertNextCell(VTK_QUAD, 4, ptIds);
      points->Delete();

      return templateOutline;
      }

    else
      {
      // TODO intersect cube with arbitrarily oriented plane and return polydata
      vtkErrorMacro( << "Not yet supported" );
      return NULL;
      }
    }

}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::GetStencil(
    vtkImageStencilData *stencilData, double p[3])
{
  int extent[6];
  this->GetExtent( extent, p );

  stencilData->SetExtent(extent);
  stencilData->SetSpacing(this->Spacing);
  stencilData->SetOrigin(this->Origin);
  stencilData->AllocateExtents();

  for (int idz=extent[4]; idz<=extent[5]; idz++)
    {
    for (int idy = extent[2]; idy <= extent[3]; idy++)
      {
      stencilData->InsertNextExtent( extent[0], extent[1], idy, idz );
      }
    }
}

//----------------------------------------------------------------------
// This really returns the distance map from an ellipsoid of similar size
// as the cuboid.. Who the hell will use a box shaped rectangular image
// data anyway.. Its got singularities.. noone doing image processing
// for sure.. !!
template < class T >
int vtkKWEPaintbrushShapeBoxFillBuffer( vtkKWEPaintbrushShapeBox * self,
                   vtkImageData *imageData, T, int extent[6], double p[3] )
{
  // Polarity of the shape
  bool state = false;
  if (self->GetPolarity() == vtkKWEPaintbrushEnums::Draw)
    {
    state = true;
    }

  const double r1square = 0.25 * self->GetWidth()[0]*self->GetWidth()[0];
  const double r2square = 0.25 * self->GetWidth()[1]*self->GetWidth()[1];
  const double r3square = 0.25 * self->GetWidth()[2]*self->GetWidth()[2];

  double value;

  for (int k= extent[4]; k<=extent[5]; k++)
    {
    for (int j= extent[2]; j<=extent[3]; j++)
      {
      for (int i= extent[0]; i<=extent[1]; i++)
        {
        T * np = static_cast< T* >(imageData->GetScalarPointer(i,j,k));

        double px = (i *
            self->GetSpacing()[0] + self->GetOrigin()[0]) - p[0];
        double py = (j *
            self->GetSpacing()[1] + self->GetOrigin()[1]) - p[1];
        double pz = (k *
            self->GetSpacing()[2] + self->GetOrigin()[2]) - p[2];

        if ( (px*px/r1square + py*py/r2square + pz*pz/r3square) > 2.0 )
          {
          // Outside the ellipse
          *np = static_cast< T >(0.0);
          continue;
          }

        // Normalized distance of the point from the surface of the ellipse.
        // This is 1.0 at the surface, 0.0 at the center, 2.0 at twice the
        // distance from the surface...
        double distance = sqrt(px*px/r1square + py*py/r2square + pz*pz/r3square);

        if (state)
          {
          value = 255.0 - 127.5 * distance;
          }
        else
          {
          value = 127.5 * distance;
          }

        // clamp value
        if( value < 1.0 )
          {
          value = 1.0;
          }
        else if( value > 254.0 )
          {
          value = 254.0;
          }

        *np = static_cast< T >(value);
        }
      }
    }

  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::GetGrayscaleData(
          vtkImageData *imageData, double p[3])
{
  // Compute the extents of the an image centered about p.
  int extent[6];
  this->GetExtent( extent, p );

  imageData->SetSpacing(this->Spacing);
  imageData->SetOrigin(this->Origin);
  imageData->SetExtent(extent);

  imageData->SetScalarType(this->GetScalarType());
  imageData->AllocateScalars();

  switch (imageData->GetScalarType())
    {
    vtkTemplateMacro( vtkKWEPaintbrushShapeBoxFillBuffer(
                this, imageData, static_cast< VTK_TT >(0), extent, p ));
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::SetWidth( double newWidthX, double newWidthY, double newWidthZ )
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Width to (" << newWidthX << "," << newWidthY << "," << newWidthZ << ")");
  if ((this->Width[0] != newWidthX)||(this->Width[1] != newWidthY)||(this->Width[2] != newWidthZ))
    {
    this->Width[0] = (this->MaxWidth[0] < 0. || this->MaxWidth[0] > newWidthX)? newWidthX :this->MaxWidth[0];
    this->Width[1] = (this->MaxWidth[1] < 0. || this->MaxWidth[1] > newWidthY)? newWidthY :this->MaxWidth[1];
    this->Width[2] = (this->MaxWidth[2] < 0. || this->MaxWidth[2] > newWidthZ)? newWidthZ :this->MaxWidth[2];
    this->Modified();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::SetWidth( double newWidth[3] )
{
  this->SetWidth( newWidth[0], newWidth[1], newWidth[2] );
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushShapeBox::Resize(double d[3], int ResizeType)
{
  // If the user specified a constraint on the resize type, use that,
  // otherwise default to whatever the widget told us in the functions'
  // argument.
  const int resizeType = (this->ResizeConstraint ==
      PaintbrushResizeUnConstrained) ? ResizeType : this->ResizeConstraint;

  // Define a minimum size that the shape will take. The shape will not
  // get smaller than this.
  const double minSize = 0.5;

  double newWidth[3] = { this->Width[0], this->Width[1], this->Width[2] };

  if (resizeType == vtkKWEPaintbrushShape::PaintbrushResizeAnisotropic)
    {
    // non-isotropic resize. This will resize each axis according to the
    // factor specified along each axis.

    for (unsigned int i=0; i<3; i++)
      {
      if (d[i] > 0.0 || this->Width[i] > 0.5)
        {
        newWidth[i] *= (1+d[i]/10.0);
        }
      }
    }

  else
    {

    // Not an AnIsotropic resize.. This will resize each axis by the same
    // factor. This factor will be the norm of the factor vector specified
    // as the functions' argument

    // Calculate the sign.. (grow or shrink)

    unsigned int idx = 0;
    double max = fabs(d[0]);
    int signVal;
    for (unsigned int i=1; i<3; i++)
      {
      if (fabs(d[i]) > max)
        {
        idx = i;
        max = fabs(d[i]);
        }
      }
    signVal = sign(d[idx]);

    // The new size is ....
    const double norm = vtkMath::Norm(d);
    for (unsigned int i=0; i<3; i++)
      {
      newWidth[i] *= (1+(norm * signVal)/10.0);
      }

    }

  // Handle special cases.
  switch (resizeType)
    {
    case PaintbrushResize_XY:
      newWidth[2] = this->Width[2];
      break;

    case PaintbrushResize_YZ:
      newWidth[0] = this->Width[0];
      break;

    case PaintbrushResize_XZ:
      newWidth[1] = this->Width[1];
      break;
    }

  // Make sure we aren't smaller than the minimum
  if (newWidth[0] < minSize ||
      newWidth[1] < minSize ||
      newWidth[2] < minSize)
    {
    return 0;
    }

  // Now change our size to the new size.
  this->SetWidth( newWidth );
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::DeepCopy(vtkKWEPaintbrushShape *s)
{
  if (s == this)
    {
    return;
    }

  vtkKWEPaintbrushShapeBox *sb = vtkKWEPaintbrushShapeBox::SafeDownCast(s);
  if (sb)
    {
    for (unsigned int i=0; i<3; i++)
      {
      this->Width[i] = sb->Width[i];
      }
    }

  this->Superclass::DeepCopy(s);
  this->Modified();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::GetAnnotation(char *s)
{
  sprintf(s, "(%0.3g,%0.3g,%0.3g)",
      this->Width[0], this->Width[1], this->Width[2]);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushShapeBox::IsInside(double currPos[3], double worldPos[3])
{
  for (unsigned int i=0; i<3; i++)
    {
    if (fabs(worldPos[i] - currPos[i]) > this->Width[i]/2.0)
      {
      return 0;
      }
    }
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::GetExtent( int extent[6], double p[3] )
{
  if (this->Representation == vtkKWEPaintbrushEnums::Grayscale)
    {
    for (int i=0; i< 3; i++)
      {
      // transition region based extension of width.
      extent[2*i]   = static_cast<int>((p[i] - this->Width[i]*
            (0.5+1.0/2.0) - this->Origin[i])/
                this->Spacing[i] + 0.5);
      extent[2*i+1] = static_cast<int>((p[i] + this->Width[i]*
            (0.5+1.0/2.0) - this->Origin[i])/
                this->Spacing[i] - 0.4999999);
      extent[2*i] = extent[2*i] < 0 ? 0 : extent[2*i];
      if (extent[2*i] > extent[2*i+1])
        {
        extent[2*i+1] = extent[2*i];
        }
      }
    }

  else
    {
    for (int i=0; i< 3; i++)
      {
      extent[2*i]   = static_cast<int>((p[i] - this->Width[i]/2.0 - this->Origin[i])
                                             /this->Spacing[i] + 0.5);
      extent[2*i+1] = static_cast<int>((p[i] + this->Width[i]/2.0 - this->Origin[i])
                                      /this->Spacing[i] - 0.49999999);
      extent[2*i]   = extent[2*i] < 0 ? 0 : extent[2*i];
      if (extent[2*i] > extent[2*i+1])
        {
        extent[2*i+1] = extent[2*i];
        }
      }
    }

  // Clip the extents with the ClipExtent
  for (int i=0; i< 3; i++)
    {
    if (extent[2*i] < this->ClipExtent[2*i])
      {
      extent[2*i] = this->ClipExtent[2*i];
      }
    if (extent[2*i+1] > this->ClipExtent[2*i+1])
      {
      extent[2*i+1] = this->ClipExtent[2*i+1];
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeBox::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Width: (" << this->Width[0] << ", "
                             << this->Width[1] << ", "
                             << this->Width[2] << ")\n";
}

