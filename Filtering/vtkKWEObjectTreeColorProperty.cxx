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
#include "vtkKWEObjectTreeColorProperty.h"

#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEObjectTreeColorProperty, "$Revision: 665 $");
vtkStandardNewMacro(vtkKWEObjectTreeColorProperty);

vtkInformationKeyRestrictedMacro(vtkKWEObjectTreeColorProperty, COLOR, DoubleVector, 3);
vtkInformationKeyMacro(vtkKWEObjectTreeColorProperty, KEY, ObjectBase);


//-----------------------------------------------------------------------------
double *vtkKWEObjectTreeColorProperty::GetColor()
{
  return this->Attributes->Get(COLOR());
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeColorProperty::SetColor(double *color)
{
  double *currentColor = this->GetColor();
  if (!currentColor || 
    currentColor[0] != color[0] ||
    currentColor[1] != color[1] ||
    currentColor[2] != color[2])
    {
    this->Attributes->Set(COLOR(), color, 3);
    this->Modified();
    }
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeColorProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << this->GetClassName() << "\n";
  this->Superclass::PrintSelf(os,indent);

  if (this->Attributes->Has(COLOR()))
    {
    double *color = this->GetColor();
    os << indent << "Color: (" << color[0] << ", " 
      << color[1] << ", " << color[2] << ")\n";
    }
  else
    {
    os << indent << "Color: (not set)\n";
    }
}
