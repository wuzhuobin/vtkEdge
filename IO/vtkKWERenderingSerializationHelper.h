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
// .NAME vtkKWERenderingSerializationHelper - Concrete serialization helper for
// Rendering package.
// .SECTION Description
// Concrete class for serialization of vtkObjects in the Rendering package using
// vtkKWEXMLArchiveWriter and vtkKWEXMLArchiveReader. Only serialization of
// vtkCamera (and its subclasses) is currently implemented.
// .SECTION See Also
// vtkKWESerializationHelperMap vtkKWESerializationHelper

#ifndef __vtkKWERenderingSerializationHelper_h
#define __vtkKWERenderingSerializationHelper_h

#include "vtkKWESerializationHelper.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkCamera;
class vtkKWESerializer;

class VTKEdge_IO_EXPORT vtkKWERenderingSerializationHelper : public vtkKWESerializationHelper
{
public:
  static vtkKWERenderingSerializationHelper *New();
  vtkTypeRevisionMacro(vtkKWERenderingSerializationHelper, vtkKWESerializationHelper);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This member registers ALL the classes supported by this helper with the
  // vtkKWESerializationHelperMap, which manages all the helpers.  Should be
  // called after construction unless you only want to enable support for
  // a subset (in which case it must be done "manually" by calling
  // vtkKWESerializationHelperMap::RegisterHelperForClass()
  virtual void RegisterWithHelperMap();

  // Description:
  // Unregister this helper (remove each class type/helper pair) with the
  // vtkKWESerializationHelperMap
  virtual void UnRegisterWithHelperMap();

  // Description:
  // Get the value for the "type" attribute of the specfied object type
  virtual const char *GetSerializationType(vtkObject *object);

  // Description:
  // Serialize the input object.  Returns 1 if successful.
  virtual int Serialize(vtkObject *object, vtkKWESerializer *serializer);

  //BTX
protected:
  vtkKWERenderingSerializationHelper();
  ~vtkKWERenderingSerializationHelper() {}

  // Description:
  // Serialize a vtkCamera
  void SerializeCamera(vtkCamera *camera, vtkKWESerializer *serializer);

private:
  vtkKWERenderingSerializationHelper(const vtkKWERenderingSerializationHelper&);  // Not implemented.
  void operator=(const vtkKWERenderingSerializationHelper&);  // Not implemented.
  //ETX
};

#endif
