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
// .NAME vtkKWEObjectTreeColorProperty - simple (temporary) Property example
// .SECTION Description
// vtkKWEObjectTreeColorProperty  is a simple concreate example of a ObjectTree
// Property. containg one value to be saved in the Attribute list (COLOR()).
// Here for example only, with plans to remove it as soon as a better/"real"
// class is created and added to VTKEdge.
//
// .SECTION See Also
// vtkKWEObjectTreePropertyBase vtkKWEObjectTreeUserProperty

#ifndef __vtkKWEObjectTreeColorProperty_h
#define __vtkKWEObjectTreeColorProperty_h

#include "vtkKWEObjectTreePropertyBase.h"

class vtkInformationDoubleVectorKey;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreeColorProperty : public vtkKWEObjectTreePropertyBase
{
public:
  static vtkKWEObjectTreeColorProperty* New();
  vtkTypeRevisionMacro(vtkKWEObjectTreeColorProperty, vtkKWEObjectTreePropertyBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkInformationDoubleVectorKey* COLOR();

  // Description:
  // Set/Get the color of this node.
  double *GetColor();
  void SetColor(double *color);

  // Description:
  // Get the key used for keeping track of this property.
  // referencing nodes.
  static vtkInformationObjectBaseKey* KEY();
  virtual vtkInformationObjectBaseKey* GetKey()
    {
    return KEY();
    }

protected:
  vtkKWEObjectTreeColorProperty() {};
  ~vtkKWEObjectTreeColorProperty() {};


private:
  vtkKWEObjectTreeColorProperty(const vtkKWEObjectTreeColorProperty&); // Not implemented.
  void operator=(const vtkKWEObjectTreeColorProperty&); // Not implemented.
};

#endif
