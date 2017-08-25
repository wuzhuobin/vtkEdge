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
