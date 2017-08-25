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
#include "vtkKWEInformationKeyMap.h"

#include <vtkInformationKey.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include <vtkstd/map>
#include <vtkstd/string>

vtkCxxRevisionMacro(vtkKWEInformationKeyMap, "$Revision: 660 $");
vtkStandardNewMacro(vtkKWEInformationKeyMap);

namespace
{
typedef vtkstd::map<vtkstd::string, vtkSmartPointer<vtkInformationKey> >KeyMapType;
struct vtkKWEInformationKeyMapInternals
{
  KeyMapType Keys;
};

vtkKWEInformationKeyMapInternals vtkKWEInformationKeyMapKeys;
}

//----------------------------------------------------------------------------
vtkKWEInformationKeyMap::vtkKWEInformationKeyMap()
{
}

//----------------------------------------------------------------------------
vtkKWEInformationKeyMap::~vtkKWEInformationKeyMap()
{
}

//----------------------------------------------------------------------------
void vtkKWEInformationKeyMap::RegisterKey(vtkInformationKey* key)
{
  vtkstd::string name = vtkstd::string(key->GetLocation()) + "." + key->GetName();
  vtkKWEInformationKeyMapKeys.Keys[name] = key;
}
  
//----------------------------------------------------------------------------
vtkInformationKey* vtkKWEInformationKeyMap::FindKey(const char* name)
{
  KeyMapType::iterator iter = vtkKWEInformationKeyMapKeys.Keys.find(name);
  if (iter != vtkKWEInformationKeyMapKeys.Keys.end())
    {
    return iter->second;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkstd::string vtkKWEInformationKeyMap::GetFullName(vtkInformationKey* key)
{
  return vtkstd::string(key->GetLocation()) + "." + key->GetName();    
}


//----------------------------------------------------------------------------
void vtkKWEInformationKeyMap::RemoveAllKeys()
{
  vtkKWEInformationKeyMapKeys.Keys.clear();
}

//----------------------------------------------------------------------------
void vtkKWEInformationKeyMap::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
