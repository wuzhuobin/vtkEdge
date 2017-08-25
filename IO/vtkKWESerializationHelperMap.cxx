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
#include "vtkKWESerializationHelperMap.h"

#include "vtkKWECommonSerializationHelper.h"
#include "vtkKWESerializationHelper.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkToolkits.h"
#include <vtkstd/map>
#include <vtkstd/string>

#ifdef VTK_USE_RENDERING
 #include "vtkKWERenderingSerializationHelper.h"
#endif

vtkCxxRevisionMacro(vtkKWESerializationHelperMap, "$Revision: 752 $");
vtkStandardNewMacro(vtkKWESerializationHelperMap);



bool vtkKWESerializationHelperMap::DefaultHelpersInstantiated = false;

namespace
{
typedef vtkstd::map<vtkstd::string, vtkSmartPointer<vtkKWESerializationHelper> > ClassHelperMapType;
struct vtkKWESerializationHelperMapInternals
{
  ClassHelperMapType ClassMap;
};

vtkKWESerializationHelperMapInternals vtkKWESerializationHelperMapClassMap;
}

//-----------------------------------------------------------------------------
vtkKWESerializationHelperMap::vtkKWESerializationHelperMap()
{
}

//-----------------------------------------------------------------------------
vtkKWESerializationHelperMap::~vtkKWESerializationHelperMap()
{
}

void vtkKWESerializationHelperMap::InstantiateDefaultHelpers()
{
  if (!DefaultHelpersInstantiated)
    {
    DefaultHelpersInstantiated = true;
    vtkSmartPointer<vtkKWECommonSerializationHelper> commonHelper = 
      vtkSmartPointer<vtkKWECommonSerializationHelper>::New();
    commonHelper->RegisterWithHelperMap();
#ifdef VTK_USE_RENDERING
    vtkSmartPointer<vtkKWERenderingSerializationHelper> renderingHelper = 
      vtkSmartPointer<vtkKWERenderingSerializationHelper>::New();
    renderingHelper->RegisterWithHelperMap();
#endif
    }
}

//-----------------------------------------------------------------------------
void vtkKWESerializationHelperMap::RegisterHelperForClass(const char *classType,
                                                          vtkKWESerializationHelper* helper)
{
  vtkKWESerializationHelperMapClassMap.ClassMap[classType] = helper;
}

//-----------------------------------------------------------------------------
void vtkKWESerializationHelperMap::UnRegisterHelperForClass(const char *classType,
                                                            vtkKWESerializationHelper* helper)
{
  ClassHelperMapType::iterator iter = 
    vtkKWESerializationHelperMapClassMap.ClassMap.find(classType);
  if (iter != vtkKWESerializationHelperMapClassMap.ClassMap.end() &&
    iter->second == helper)
    {
    vtkKWESerializationHelperMapClassMap.ClassMap.erase(iter);
    }
}

//-----------------------------------------------------------------------------
void vtkKWESerializationHelperMap::RemoveAllHelpers()
{
  vtkKWESerializationHelperMapClassMap.ClassMap.clear();
}


//----------------------------------------------------------------------------- 
bool vtkKWESerializationHelperMap::IsSerializable(vtkObject *obj)
{
  ClassHelperMapType::iterator iter = 
    vtkKWESerializationHelperMapClassMap.ClassMap.find(obj->GetClassName());
  if (iter != vtkKWESerializationHelperMapClassMap.ClassMap.end())
    {
    return true;
    }

  return false;
}

//-----------------------------------------------------------------------------
int vtkKWESerializationHelperMap::Serialize(vtkObject *object,
                                            vtkKWESerializer *serializer)
{
  ClassHelperMapType::iterator iter = 
    vtkKWESerializationHelperMapClassMap.ClassMap.find(object->GetClassName());
  if (iter == vtkKWESerializationHelperMapClassMap.ClassMap.end())
    {
    vtkGenericWarningMacro("Unable to serialize object: " << object->GetClassName());
    return 0;
    }

  iter->second->Serialize(object, serializer);

  return 1;
}

//-----------------------------------------------------------------------------
const char *vtkKWESerializationHelperMap::GetSerializationType(vtkObject *object)
{
  ClassHelperMapType::iterator iter = 
    vtkKWESerializationHelperMapClassMap.ClassMap.find(object->GetClassName());
  if (iter == vtkKWESerializationHelperMapClassMap.ClassMap.end())
    {
    vtkGenericWarningMacro("Unable to get serialization type: " << object->GetClassName());
    return 0;
    }

  return iter->second->GetSerializationType(object);
}

//-----------------------------------------------------------------------------
vtkKWESerializationHelper* vtkKWESerializationHelperMap::GetHelper(const char *classType)
{
  ClassHelperMapType::iterator iter = 
    vtkKWESerializationHelperMapClassMap.ClassMap.find(classType);
  if (iter == vtkKWESerializationHelperMapClassMap.ClassMap.end())
    {
    return 0;
    }

  return iter->second;
}

//-----------------------------------------------------------------------------
void vtkKWESerializationHelperMap::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
