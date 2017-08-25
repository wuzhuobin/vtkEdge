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
#include "vtkKWEObjectTreeColorProperty.h"

#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEObjectTreeColorProperty, "$Revision: 1774 $");
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
