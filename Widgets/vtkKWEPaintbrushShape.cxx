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
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushShape, "$Revision: 3282 $");

//----------------------------------------------------------------------
vtkKWEPaintbrushShape::vtkKWEPaintbrushShape()
{
  this->Spacing[0]       = this->Spacing[1] = this->Spacing[2] = 1.0;
  this->Origin[0]        = this->Origin[1]  = this->Origin[2]  = 0.0;
  this->ScalarType       = VTK_UNSIGNED_CHAR;
  this->Polarity         = vtkKWEPaintbrushEnums::Draw;
  this->ResizeConstraint = vtkKWEPaintbrushShape::PaintbrushResizeUnConstrained;
  this->Representation   = vtkKWEPaintbrushEnums::Binary;
  this->MaxWidth[0]      = this->MaxWidth[1] = this->MaxWidth[2] = -1.0;
  this->ClipExtent[0]    = VTK_INT_MIN;
  this->ClipExtent[2]    = VTK_INT_MIN;
  this->ClipExtent[4]    = VTK_INT_MIN;
  this->ClipExtent[1]    = VTK_INT_MAX;
  this->ClipExtent[3]    = VTK_INT_MAX;
  this->ClipExtent[5]    = VTK_INT_MAX;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushShape::~vtkKWEPaintbrushShape()
{
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::GetPaintbrushData(vtkKWEPaintbrushData *d,
                                           int x,
                                           int y,
                                           int z)
{
  double worldPos[3];
  worldPos[0] = x * this->Spacing[0] + this->Origin[0];
  worldPos[1] = y * this->Spacing[1] + this->Origin[1];
  worldPos[2] = z * this->Spacing[2] + this->Origin[2];
  this->GetPaintbrushData(d, worldPos);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::GetPaintbrushData(vtkKWEPaintbrushData *d,
                                           double p[3])
{
  if (vtkKWEPaintbrushStencilData *s = vtkKWEPaintbrushStencilData::SafeDownCast(d))
    {
    this->GetStencil( s->GetImageStencilData(), p);
    }
  else if (vtkKWEPaintbrushGrayscaleData *t =
           vtkKWEPaintbrushGrayscaleData::SafeDownCast(d))
    {
    this->GetGrayscaleData( t->GetImageData(), p);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::DeepCopy(vtkKWEPaintbrushShape *s)
{
  if (s == this || !s)
    {
    return;
    }

  for (unsigned int i=0; i<3; i++)
    {
    this->Spacing[i] = s->Spacing[i];
    this->Origin[i]  = s->Origin[i];
    this->ScalarType = s->ScalarType;
    this->Polarity   = s->Polarity;
    }
  this->Modified();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::SetClipExtent(int e[6])
{
  for (int i = 0; i < 6; i++)
    {
    this->ClipExtent[i] = e[i];
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::GetClipExtent(int e[6])
{
  for (int i = 0; i < 6; i++)
    {
    e[i] = this->ClipExtent[i];
    }
}

//----------------------------------------------------------------------
int *vtkKWEPaintbrushShape::GetClipExtent()
{
  return this->ClipExtent;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::SetPolarityToDraw()
{
  this->SetPolarity(vtkKWEPaintbrushEnums::Draw);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::SetPolarityToErase()
{
  this->SetPolarity(vtkKWEPaintbrushEnums::Erase);
}

