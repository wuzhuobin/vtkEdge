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
// .NAME vtkKWECommonSerializationHelper - Concrete serialization helper for
// Common package vtkObjects.
// .SECTION Description
// Concrete class for serialization of vtkObjects in the Common package using
// vtkKWEXMLArchiveWriter and vtkKWEXMLArchiveReader.  Only serialization of
// vtkTransform, vtkIdTypeArray, vtkIntArray, and vtkDoubleArray is currently
// implemented.
// .SECTION See Also
// vtkKWESerializationHelperMap vtkKWESerializationHelper

#ifndef __vtkKWECommonSerializationHelper_h
#define __vtkKWECommonSerializationHelper_h

#include "vtkKWESerializationHelper.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkDataArray;
class vtkKWESerializer;
class vtkTransform;

class VTKEdge_IO_EXPORT vtkKWECommonSerializationHelper : public vtkKWESerializationHelper
{
public:
  static vtkKWECommonSerializationHelper *New();
  vtkTypeRevisionMacro(vtkKWECommonSerializationHelper, vtkKWESerializationHelper);
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
  vtkKWECommonSerializationHelper();
  ~vtkKWECommonSerializationHelper() {}

  // Description:
  // Serialize a vtkTransform
  void SerializeTransform(vtkTransform *transform, vtkKWESerializer *serializer);

  // Description:
  // Serialize vtkIdTypeArray, vtkIntArray, or vtkDoubleArray
  void SerializeDataArray(vtkDataArray *dataArray, vtkKWESerializer *serializer);

private:
  vtkKWECommonSerializationHelper(const vtkKWECommonSerializationHelper&);  // Not implemented.
  void operator=(const vtkKWECommonSerializationHelper&);  // Not implemented.
  //ETX
};

#endif
