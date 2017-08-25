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
// .NAME vtkKWEObjectTreeUserProperty - A user editable Property for ObjectTree nodes
// .SECTION Description
// Whereas other ObjectTreeProperties can only be edited via the provided
// Set/Get methods, the vtkKWEObjectTreeUserProperty allows an application
// developer to define specific attributes/keys within the app (without
// subclassing vtkKWEObjectTreePropertyBase) which can then be serialized
// as other vtkKWEObjectTreePropertyBase) are serialized.  Note that it
// is the responsibility of the user of this class to call Modified()
// when/if changes are made (values added/removed/changed) if the "TreeTime"
// of the nodes referring to the property is to be preoperly updated.
// See TestObjectTree.cxx for example usage.
// .SECTION See Also
// vtkKWEObjectTreePropertyBase

#ifndef __vtkKWEObjectTreeUserProperty_h
#define __vtkKWEObjectTreeUserProperty_h

#include "vtkKWEObjectTreePropertyBase.h"

class vtkInformationDoubleVectorKey;
class vtkInformationObjectBaseKey;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreeUserProperty : public vtkKWEObjectTreePropertyBase
{
public:
  static vtkKWEObjectTreeUserProperty* New();
  vtkTypeRevisionMacro(vtkKWEObjectTreeUserProperty, vtkKWEObjectTreePropertyBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Give access to the internal vtkInformation object so information can be
  // added as user wants.  NOTE: If changes are made, Modified() should/must
  // be called so we can update the TreeModifiedTime of all our
  // referencing nodes.
  vtkInformation *GetAttributesPointer();

  // Description:
  // Get the key used for keeping track of this property.
  static vtkInformationObjectBaseKey* KEY();
  virtual vtkInformationObjectBaseKey* GetKey()
    {
    return KEY();
    }

protected:
  vtkKWEObjectTreeUserProperty() {};
  ~vtkKWEObjectTreeUserProperty() {};

private:
  vtkKWEObjectTreeUserProperty(const vtkKWEObjectTreeUserProperty&); // Not implemented.
  void operator=(const vtkKWEObjectTreeUserProperty&); // Not implemented.
};

#endif
