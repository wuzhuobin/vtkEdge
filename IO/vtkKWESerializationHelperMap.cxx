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

vtkCxxRevisionMacro(vtkKWESerializationHelperMap, "$Revision: 1774 $");
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
