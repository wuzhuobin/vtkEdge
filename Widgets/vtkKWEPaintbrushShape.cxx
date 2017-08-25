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
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushShape, "$Revision: 590 $");

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
void vtkKWEPaintbrushShape::SetPolarityToDraw()
{
  this->SetPolarity(vtkKWEPaintbrushEnums::Draw);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushShape::SetPolarityToErase()
{
  this->SetPolarity(vtkKWEPaintbrushEnums::Erase);
}

