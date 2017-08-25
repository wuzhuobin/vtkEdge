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
#include "vtkKWEPaintbrushShapeEllipsoid.h"

#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkImageStencilData.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkParametricFunctionSource.h"
#include "vtkParametricEllipsoid.h"
#include "vtkImageEllipsoidSource.h"
#include "vtkPlane.h"
#include "vtkCellArray.h"
#include "vtkKWEPaintbrushUtilities.h"

#ifndef min
#define min(x,y) ((x<y) ? (x) : (y))
#endif

#define sign(x) ((x<0) ? (-1) : (1))

vtkCxxRevisionMacro(vtkKWEPaintbrushShapeEllipsoid, "$Revision: 3282 $");
vtkStandardNewMacro(vtkKWEPaintbrushShapeEllipsoid);

//----------------------------------------------------------------------
vtkKWEPaintbrushShapeEllipsoid::vtkKWEPaintbrushShapeEllipsoid()
{
  this->Width[0]         = 0.0;
  this->Width[1]         = 0.0;
  this->Width[2]         = 0.0;
  this->Resolution       = 64;
  this->TransitionRegion = 0.2;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushShapeEllipsoid::~vtkKWEPaintbrushShapeEllipsoid()
{
}

//----------------------------------------------------------------------
vtkSmartPointer< vtkPolyData >
vtkKWEPaintbrushShapeEllipsoid::GetShapePolyData(
              double *center, vtkPlane *plane)
{

  if (plane == NULL)
    {
    // No Orientation specified. Return the whole Polydata. This is what will
    // be rendered on the volume widget

    vtkParametricFunctionSource * parametricFunctionSource =
      vtkParametricFunctionSource::New();
    vtkParametricEllipsoid * ellipsoidFunction
                            = vtkParametricEllipsoid::New();
    parametricFunctionSource->SetParametricFunction(ellipsoidFunction);
    parametricFunctionSource->SetScalarModeToNone();
    parametricFunctionSource->GenerateTextureCoordinatesOff();
    parametricFunctionSource->SetUResolution( this->Resolution );
    parametricFunctionSource->Update();

    vtkSmartPointer< vtkPolyData > pd = parametricFunctionSource->GetOutput();

    ellipsoidFunction->Delete();
    parametricFunctionSource->Delete();

    return pd;
    }

  else

    {
    double normal[3], origin[3], r[3];
    r[0] = this->Width[0]/2.0;
    r[1] = this->Width[1]/2.0;
    r[2] = this->Width[2]/2.0;
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

      vtkPoints    *points      = vtkPoints::New();
      vtkCellArray *lines       = vtkCellArray::New();
      vtkIdType    *lineIndices = new vtkIdType[this->Resolution + 1];

      r[1] = r[1] * sqrt(1-
          (origin[0]-center[0])*(origin[0]-center[0])/(r[0]*r[0]));
      r[2] = r[2] * sqrt(1-
          (origin[0]-center[0])*(origin[0]-center[0])/(r[0]*r[0]));


      for (int i = 0; i< this->Resolution; i++)
        {
        const double angle = 2.0*vtkMath::Pi()*
          static_cast<double>(i)/static_cast<double>(this->Resolution);
        points->InsertPoint(static_cast<vtkIdType>(i), origin[0],
                             center[1] + r[1] * cos(angle),
                             center[2] + r[2] * sin(angle));
        lineIndices[i] = static_cast<vtkIdType>(i);
        }

      lineIndices[this->Resolution] = 0;
      lines->InsertNextCell(this->Resolution+1,lineIndices);
      delete [] lineIndices;
      templateOutline->SetPoints(points);
      templateOutline->SetLines(lines);
      points->Delete();
      lines->Delete();

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

      vtkPoints    *points      = vtkPoints::New();
      vtkCellArray *lines       = vtkCellArray::New();
      vtkIdType    *lineIndices = new vtkIdType[this->Resolution + 1];

      r[0] = r[0] * sqrt(1-
          (origin[1]-center[1])*(origin[1]-center[1])/(r[1]*r[1]));
      r[2] = r[2] * sqrt(1-
          (origin[1]-center[1])*(origin[1]-center[1])/(r[1]*r[1]));


      for (int i = 0; i< this->Resolution; i++)
        {
        const double angle = 2.0*vtkMath::Pi()
          *i/static_cast<double>(this->Resolution);
        points->InsertPoint(static_cast<vtkIdType>(i),
                            center[0] + r[0] * cos(angle),
                            origin[1],
                            center[2] + r[2] * sin(angle));
        lineIndices[i] = static_cast<vtkIdType>(i);
        }

      lineIndices[this->Resolution] = 0;
      lines->InsertNextCell(this->Resolution+1,lineIndices);
      delete [] lineIndices;
      templateOutline->SetPoints(points);
      templateOutline->SetLines(lines);
      points->Delete();
      lines->Delete();

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

      vtkPoints    *points      = vtkPoints::New();
      vtkCellArray *lines       = vtkCellArray::New();
      vtkIdType    *lineIndices = new vtkIdType[this->Resolution + 1];

      r[0] = r[0] * sqrt(1-
          (origin[1]-center[1])*(origin[1]-center[1])/(r[1]*r[1]));
      r[2] = r[2] * sqrt(1-
          (origin[1]-center[1])*(origin[1]-center[1])/(r[1]*r[1]));


      for (int i = 0; i< this->Resolution; i++)
        {
        const double angle = 2.0*vtkMath::Pi()*
          i/static_cast<double>(this->Resolution);
        points->InsertPoint(static_cast<vtkIdType>(i),
                            center[0] + r[0] * cos(angle),
                            center[1] + r[1] * sin(angle),
                            origin[2]);
        lineIndices[i] = static_cast<vtkIdType>(i);
        }

      lineIndices[this->Resolution] = 0;
      lines->InsertNextCell(this->Resolution+1,lineIndices);
      delete [] lineIndices;
      templateOutline->SetPoints(points);
      templateOutline->SetLines(lines);
      points->Delete();
      lines->Delete();

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
void vtkKWEPaintbrushShapeEllipsoid::GetStencil(
    vtkImageStencilData *stencilData, double p[3])
{
  //int extent[6];
  //this->GetExtent( extent, p );

  // A. Get an elliptical stencil

  // A.1. Compute extents that the stencil will have

  double ellipsoidBounds[6];
  int ellipsoidExtent[6];
  for (int i = 0; i < 3; i++)
    {
    ellipsoidBounds[2*i] = p[i] - this->Width[i]/2.0;
    ellipsoidBounds[2*i+1] = p[i] + this->Width[i]/2.0;
    ellipsoidExtent[2*i] = static_cast< int >((ellipsoidBounds[2*i]-
          this->Origin[i])/this->Spacing[i]);
    ellipsoidExtent[2*i+1] = static_cast< int >((ellipsoidBounds[2*i+1]-
          this->Origin[i])/this->Spacing[i] + 0.5);

    // Clip the extents with the clip extent.
    if (ellipsoidExtent[2*i] < this->ClipExtent[2*i])
      {
      ellipsoidExtent[2*i] = this->ClipExtent[2*i];
      }
    if (ellipsoidExtent[2*i+1] > this->ClipExtent[2*i+1])
      {
      ellipsoidExtent[2*i+1] = this->ClipExtent[2*i+1];
      }
    }

  // A.2 Allocate the stencil

  stencilData->SetExtent(ellipsoidExtent);
  stencilData->SetSpacing(this->Spacing);
  stencilData->SetOrigin(this->Origin);
  stencilData->AllocateExtents();

  // A.3 Fill the stencil with an ellipsoid

  int xIdx, yIdx, zIdx, ends[2];
  double startPt[3], inc[3];

  for (int i = 0; i < 3; i++)
    {
    startPt[i] = 2.0 * (((double)ellipsoidExtent[2*i])*this->Spacing[i] +
        this->Origin[i] - p[i])/this->Width[i];
    inc[i] = 2.0 * this->Spacing[i] / this->Width[i];
    }

  double yInc = inc[1], xInc = inc[0], zInc = inc[2], zDist = startPt[2], xDist, yDist;
  double zDistSq, zyDistSq, distSq;

  // A.4 Rasterize

  for (zIdx = ellipsoidExtent[4]; zIdx <= ellipsoidExtent[5]; zIdx++, zDist += zInc)
    {
    zDistSq = zDist * zDist;
    yDist = startPt[1];
    for (yIdx = ellipsoidExtent[2]; yIdx <= ellipsoidExtent[3]; yIdx++, yDist += yInc)
      {
      zyDistSq = zDistSq + (yDist * yDist);

      xDist = startPt[0];
      ends[0] = ends[1] = -1;

      for (xIdx = ellipsoidExtent[0]; xIdx <= ellipsoidExtent[1]; xIdx++, xDist += xInc)
        {
        distSq = zyDistSq + (xDist * xDist);

        if (ends[0] == -1 && distSq <= 1.0)
          {
          // found the start
          ends[0] = xIdx;
          continue;
          }
        if (ends[0] != -1 && distSq > 1.0)
          {
          // found the end, jump to to the next scan line. There can be only one
          // segment per scan line in an ellipse.
          ends[1] = xIdx;
          stencilData->InsertNextExtent(ends[0], ends[1]-1, yIdx, zIdx);
          break;
          }
        }

      if (ends[1] == -1 && ends[0] != -1)
        {
        // scan line never completed.
        stencilData->InsertNextExtent(ends[0], ellipsoidExtent[1], yIdx, zIdx);
        }
      }
    }
}

//----------------------------------------------------------------------
template < class T >
int vtkKWEPaintbrushShapeEllipsoidFillBuffer( vtkKWEPaintbrushShapeEllipsoid * self,
                   vtkImageData *imageData, T, int extent[6], double p[3] )
{
  // Polarity of the shape
  const bool state = (self->GetPolarity() == vtkKWEPaintbrushEnums::Draw);

  // Get the radius along each axis = halfwidth ^ 2
  const double r1square = 0.25 * self->GetWidth()[0]*self->GetWidth()[0];
  const double r2square = 0.25 * self->GetWidth()[1]*self->GetWidth()[1];
  const double r3square = 0.25 * self->GetWidth()[2]*self->GetWidth()[2];

  // get metadata
  double spacing[3], origin[3], value;
  self->GetSpacing(spacing);
  self->GetOrigin(origin);

  // The grayscale values in the brush will extend to maxRadiusFactor times
  // the brush width along any direction.
  const double transitionRegion = self->GetTransitionRegion();
  const double maxRadiusFactor = 1.0 + transitionRegion;
  const double maxRadiusFactorSq = maxRadiusFactor * maxRadiusFactor;
  const double polarity = (state ? 1.0 : -1.0);

  for (int k= extent[4]; k<=extent[5]; k++)
    {
    for (int j= extent[2]; j<=extent[3]; j++)
      {
      T * np = static_cast< T* >(imageData->GetScalarPointer(extent[0],j,k));
      for (int i= extent[0]; i<=extent[1]; ++i, ++np)
        {
        double px = i * spacing[0] + origin[0] - p[0];
        double py = j * spacing[1] + origin[1] - p[1];
        double pz = k * spacing[2] + origin[2] - p[2];

        const double distanceSq = px*px/r1square +
                                  py*py/r2square +
                                  pz*pz/r3square;

        if ( distanceSq > maxRadiusFactorSq )
          {
          // Outside the ellipse
          *np = static_cast< T >(0.0);
          continue;
          }

        // Normalized distance of the point from the surface of the ellipse.
        // This is 1.0 at the surface, 0.0 at the center, 2.0 at twice the
        // distance from the surface...
        const double distance = sqrt(distanceSq);

        if (distance <= (1.0 - transitionRegion))
          {
          value = state ? 255.0 : 0.0;
          }
        else
          {
          value = 127.5 * (1.0 + polarity * (1-distance)/transitionRegion);
          }

        //  Positive brush profile              Negarive brush profile
        //
        //
        // 255   -           /\                       ----        ----
        //                  /  \                          \      /
        // 127.5 -         /    \                          \    /
        //                /      \                          \  /
        // 0     _   ____/        \____                      \/
        //
        //               |    |    |                      |   |   |
        //             -2r    0    2r                   -2r   0   2r
        //
        // value = state ? (255.0 - 127.5 * distance) : 127.5 * distance;


        // Don't use 0. So we shrink the range within the shape by 1 on
        // each end. 1 - 254. The reason is 0 is used to indicate an
        // outside value (outside the brush). See vtkKWEPaintbrushGrayscaleData
        //
        // clamp value [1 - 254]
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
void vtkKWEPaintbrushShapeEllipsoid::GetGrayscaleData(
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
    vtkTemplateMacro( vtkKWEPaintbrushShapeEllipsoidFillBuffer(
                this, imageData, static_cast< VTK_TT >(0), extent, p ));
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeEllipsoid::SetWidth( double newWidthX, double newWidthY, double newWidthZ )
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
void vtkKWEPaintbrushShapeEllipsoid::SetWidth( double newWidth[3] )
{
  this->SetWidth( newWidth[0], newWidth[1], newWidth[2] );
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushShapeEllipsoid::Resize(double d[3], int ResizeType)
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
      if (d[i] > 0.0 || this->Width[i] > minSize)
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
void vtkKWEPaintbrushShapeEllipsoid::DeepCopy(vtkKWEPaintbrushShape *s)
{
  if (s == this)
    {
    return;
    }

  vtkKWEPaintbrushShapeEllipsoid *sb = vtkKWEPaintbrushShapeEllipsoid::SafeDownCast(s);
  if (sb)
    {
    for (unsigned int i=0; i<3; i++)
      {
      this->Width[i] = sb->Width[i];
      }
    this->Resolution = sb->Resolution;
    }

  this->Superclass::DeepCopy(s);
  this->Modified();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeEllipsoid::GetAnnotation(char *s)
{
  sprintf(s, "Diameter: (%0.3g,%0.3g,%0.3g)",
      this->Width[0], this->Width[1], this->Width[2]);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeEllipsoid::GetExtent(int extent[6],
                                            double p[3])
{
  if (this->Representation == vtkKWEPaintbrushEnums::Grayscale)
    {
    for (int i=0; i< 3; i++)
      {
      // transition region based extension of width.
      extent[2*i]   = static_cast<int>((p[i] - this->Width[i]*
            (0.5+this->TransitionRegion/2.0) - this->Origin[i])/
                this->Spacing[i] + 0.5);
      extent[2*i+1] = static_cast<int>((p[i] + this->Width[i]*
            (0.5+this->TransitionRegion/2.0) - this->Origin[i])/
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
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShapeEllipsoid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Width: (" << this->Width[0] << ", "
                             << this->Width[1] << ", "
                             << this->Width[2] << ")\n";
  os << indent << "Resolution: " << this->Resolution << std::endl;
}

