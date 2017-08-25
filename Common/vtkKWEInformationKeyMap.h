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
// .NAME vtkKWEInformationKeyMap - Map from string to information key instances
// .SECTION Description
// This class is used to access information keys given a string in the
// form KeyLocation.KeyName. For example: vtkDataObject.DATA_OBJECT
// (note the lack of () at the end).
// You have to manually register the keys in the map before they can
// be accessed.
// Note that all of the key instances are stored using smart pointers.
// Make sure to call RemoveAllKeys() before exit to avoid leak warnings
// from vtkDebugLeaks.

#ifndef __vtkKWEInformationKeyMap_h
#define __vtkKWEInformationKeyMap_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include <vtkstd/string>

//BTX
class vtkInformationKey;
//ETX

class VTKEdge_COMMON_EXPORT vtkKWEInformationKeyMap : public vtkObject
{
public:
  static vtkKWEInformationKeyMap *New();
  vtkTypeRevisionMacro(vtkKWEInformationKeyMap,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Register a key with the map. The key will be KeyLocation::KeyName.
  static void RegisterKey(vtkInformationKey* key);
  
  // Description:
  // Lookup a key instance registered with the map using its location
  // and name.
  static vtkInformationKey* FindKey(const char* name);
  
  // Description:
  // Given a key instance, returns its name used to store it in the
  // map. Implemented as:
  // key->GetLocation() + "." + key->GetName()
  static vtkstd::string GetFullName(vtkInformationKey* key);
  
  // Description:
  // Removes all keys from the map.
  static void RemoveAllKeys();
  
protected:
  vtkKWEInformationKeyMap();
  ~vtkKWEInformationKeyMap();

private:
  vtkKWEInformationKeyMap(const vtkKWEInformationKeyMap&);  // Not implemented.
  void operator=(const vtkKWEInformationKeyMap&);  // Not implemented.
};

#endif
