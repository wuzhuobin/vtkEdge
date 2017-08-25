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
