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
#include "vtkKWEInformationKeyMap.h"

#include <vtkInformationKey.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include <vtkstd/map>
#include <vtkstd/string>

vtkCxxRevisionMacro(vtkKWEInformationKeyMap, "$Revision: 1774 $");
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
