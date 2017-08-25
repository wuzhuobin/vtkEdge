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
#include "vtkKWEObjectTreeUserProperty.h"

#include "vtkInformation.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEObjectTreeUserProperty, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEObjectTreeUserProperty);

vtkInformationKeyMacro(vtkKWEObjectTreeUserProperty, KEY, ObjectBase);

//-----------------------------------------------------------------------------
vtkInformation *vtkKWEObjectTreeUserProperty::GetAttributesPointer()
{
  return this->Attributes;
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeUserProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
