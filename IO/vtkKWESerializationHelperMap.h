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
// .NAME vtkKWESerializationHelperMap - Map from class name to helper object.
// .SECTION Description
// This class is used to map from class name to a helper class that has been
// registered to serialize the class.  
//
// Note, helper classes must be manually instantiated, at which time 
// the class types supported by the helper should be registered with the
// vtkKWESerializationHelperMap (by calling RegisterWithHelperMap on the 
// helper).  The "known" helper classes (those for the VTK/VTKEdge kits) 
// should be added to InstantiateDefaultHelpers(), which is called by both
// the vtkKWEXMLArchiveReader and vtkKWEXMLArchiveWriter during their 
// construction.  This class then manages destruction of the helper since the
// map holds a vtkSmartPointer to the helper for each supported class type.
//
// .SECTION See Also
// vtkKWESerializationHelper

#ifndef __vtkKWESerializationHelperMap_h
#define __vtkKWESerializationHelperMap_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWESerializationHelper;
class vtkKWESerializer;
class vtkKWEXMLElement;

class VTKEdge_IO_EXPORT vtkKWESerializationHelperMap : public vtkObject
{
public:
  static vtkKWESerializationHelperMap *New();
  vtkTypeRevisionMacro(vtkKWESerializationHelperMap,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Instantiate all "known" (supporting core VTK and VTKEdge) helpers
  // and then register them with the helper map.
  static void InstantiateDefaultHelpers();

  // Description:
  // Register (add) a class type / helper with the map.
  static void RegisterHelperForClass(const char *classType, 
    vtkKWESerializationHelper* helper);

  // Description:
  // UnRegister (remove) a class type / helper in the map, if the registered
  // helper matches.
  static void UnRegisterHelperForClass(const char *classType, 
    vtkKWESerializationHelper* helper);

  // Description:
  // Serialize the input object using a registered helper.  If successful (map
  // entry exists), returns 1 (0 if not).
  static int Serialize(vtkObject *obj, vtkKWESerializer *serializer);

  // Description:
  // Get the value for the "type" attribute (generally the ClassName but may be
  // name of Superclass) of the specfied object type from a registerd helper
  // for the object Clas
  static const char *GetSerializationType(vtkObject *object);

  // Description:
  // Returns true if the object class type is registered with the map, and thus
  // can be serialized.
  static bool IsSerializable(vtkObject *obj);

  // Description:
  // Return the serialization helper registered for the indicated class (if one
  // has been registerd); otherwise return NULL
  static vtkKWESerializationHelper* GetHelper(const char *classType);

  // Description:
  // Removes all helpers from the map.
  static void RemoveAllHelpers();

protected:
  vtkKWESerializationHelperMap();
  ~vtkKWESerializationHelperMap();

private:
  vtkKWESerializationHelperMap(const vtkKWESerializationHelperMap&);  // Not implemented.
  void operator=(const vtkKWESerializationHelperMap&);  // Not implemented.

  static bool DefaultHelpersInstantiated;
};

#endif
