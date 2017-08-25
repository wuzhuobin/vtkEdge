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
