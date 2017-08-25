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
// .NAME vtkKWESerializationHelper - Superclass for serialization helpers
// .SECTION Description
// Abstract class for serialization of vtkObjects with the
// vtkKWEXMLArchiveWriter and vtkKWEXMLArchiveReader.  Subclasses must
// implement RegisterWithHelperMap (which registers each class type supported
// by the helper with the vtkKWESerializationHelperMap), GetSerializationType
// (returns the "type" to set as the attribute for the element), and Serialize
// (which does the actual serialization) and UnRegisterWithHelperMap().
// .SECTION See Also
// vtkKWESerializationHelperMap

#ifndef __vtkKWESerializationHelper_h
#define __vtkKWESerializationHelper_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWESerializer;

class VTKEdge_IO_EXPORT vtkKWESerializationHelper : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkKWESerializationHelper,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This member registers ALL the classes supported by this helper with the
  // vtkKWESerializationHelperMap, which manages all the helpers.  Should be
  // called after construction unless you only want to enable support for
  // a subset (in which case it must be done "manually" by calling
  // vtkKWESerializationHelperMap::RegisterHelperForClass()
  virtual void RegisterWithHelperMap() = 0;

  // Description:
  // Unregister the helper (remove each class type/helper pair) with the
  // vtkKWESerializationHelperMap
  virtual void UnRegisterWithHelperMap() = 0;

  // Description:
  // Get the value for the "type" attribute of the specfied object type
  virtual const char *GetSerializationType(vtkObject *object) = 0;

  // Description:
  // Serialize the input object.  Returns 1 if successful.
  virtual int Serialize(vtkObject *object, vtkKWESerializer *serializer) = 0;

protected:
  vtkKWESerializationHelper() {}
  ~vtkKWESerializationHelper() {}

private:
  vtkKWESerializationHelper(const vtkKWESerializationHelper&);  // Not implemented.
  void operator=(const vtkKWESerializationHelper&);  // Not implemented.
};

#endif
