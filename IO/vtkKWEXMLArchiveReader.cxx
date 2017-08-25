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
#include "vtkKWEXMLArchiveReader.h"

#include "vtkInformation.h"
#include "vtkInformationDoubleKey.h"
#include "vtkInformationIdTypeKey.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkInformationStringKey.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationObjectBaseVectorKey.h"
#include "vtkInformationKeyVectorKey.h"
#include "vtkInstantiator.h"
#include "vtkKWEInformationKeyMap.h"
#include "vtkKWESerializableObject.h"
#include "vtkKWESerializationHelperMap.h"
#include "vtkKWEXMLElement.h"
#include "vtkKWEXMLParser.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include <vtkstd/list>
#include <vtkstd/map>
#include <vtksys/SystemTools.hxx>
#include <vtksys/ios/sstream>

vtkCxxRevisionMacro(vtkKWEXMLArchiveReader, "$Revision: 833 $");
vtkStandardNewMacro(vtkKWEXMLArchiveReader);

vtkCxxSetObjectMacro(vtkKWEXMLArchiveReader, RootElement, vtkKWEXMLElement);

struct vtkKWEXMLArchiveReaderInternals
{
  vtkstd::map<int, vtkKWEXMLElement*> IdToElement;
  
  vtkKWEXMLElement* FindElement(int id)
  {
    vtkstd::map<int, vtkKWEXMLElement*>::iterator iter =
      this->IdToElement.find(id);
    if (iter != this->IdToElement.end())
      {
      return iter->second;
      }
    return 0;
  }
  
  vtkstd::map<int, vtkObject*> IdToObject;
  
  vtkObject* FindObject(int id)
  {
    vtkstd::map<int, vtkObject*>::iterator iter = this->IdToObject.find(id);
    if (iter != this->IdToObject.end())
      {
      return iter->second;
      }
    return 0;
  }
  
  vtkKWEXMLElement* Top()
    {
    return this->Stack.front();
    }
    
  void Push(vtkKWEXMLElement* elem)
    {
    this->Stack.push_front(elem);
    }
    
  void Pop()
    {
    this->Stack.pop_front();
    }
    
  vtkstd::list<vtkKWEXMLElement*> Stack;

  // Stack to hold all the serializable objects; to support a weakPtr
  // to an object that is read before the object being referred to is created.
  // The ObjectStack holds an extra reference to all objects to prevent leaks
  // while supoorting this situtation (see comment in ReadObject).
  vtkstd::list<vtkSmartPointer<vtkObject> > ObjectStack;
};



//----------------------------------------------------------------------------
vtkKWEXMLArchiveReader::vtkKWEXMLArchiveReader()
{
  this->Internal = new vtkKWEXMLArchiveReaderInternals;
  vtkKWESerializationHelperMap::InstantiateDefaultHelpers();
  this->RootElement = 0;
}

//----------------------------------------------------------------------------
vtkKWEXMLArchiveReader::~vtkKWEXMLArchiveReader()
{
  this->SetRootElement(0);
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(istream& str, const char*, 
                                       vtkstd::vector<vtkSmartPointer<vtkObject> >& objs)
{
  delete this->Internal;
  this->Internal = new vtkKWEXMLArchiveReaderInternals;
  objs.clear();
  if (this->ParseStream(str))
    {
    this->Serialize(objs);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(vtkKWEXMLElement *rootElement,
                                       const char*, 
                                       vtkstd::vector<vtkSmartPointer<vtkObject> >& objs)
{
  delete this->Internal;
  this->Internal = new vtkKWEXMLArchiveReaderInternals;
  objs.clear();

  this->SetRootElement(rootElement);
  this->Serialize(objs);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(vtkstd::vector<vtkSmartPointer<vtkObject> >& objs)
{
  unsigned int nnested = this->RootElement->GetNumberOfNestedElements();
  for (unsigned int i=0; i<nnested; i++)
    {
    vtkKWEXMLElement* elem = this->RootElement->GetNestedElement(i);
    int id;
    if (elem->GetScalarAttribute("id", &id))
      {
      this->Internal->IdToElement[id] = elem;
      }
    }
    
  unsigned int version;
  if (this->RootElement->GetScalarAttribute("version", &version))
    {
    this->SetArchiveVersion(version);
    }
  
  this->Internal->Push(this->RootElement);
  this->Internal->ObjectStack.clear(); // just to be sure
  this->Serialize("RootObjects", objs);
  this->Internal->Pop();
  
  delete this->Internal;
  this->Internal = 0;
}

//----------------------------------------------------------------------------
vtkObject* vtkKWEXMLArchiveReader::ReadObject(int id, bool weakPtr)
{
  vtkObject* obj = this->Internal->FindObject(id);
  if (obj)
    {
    if (!weakPtr)
      {
      obj->Register(0);
      }
    return obj;
    }
    
  vtkKWEXMLElement* elem = this->Internal->FindElement(id);
  if (!elem)
    {
    vtkErrorMacro("Could not find of id " << id)
    return 0;
    }
  const char* className = elem->GetAttribute("type");
  obj = vtkObject::SafeDownCast(vtkInstantiator::CreateInstance(className));
  if (!obj)
    {
    vtkErrorMacro("Could not create object of type " << className)
    return 0;
    }
  this->Internal->ObjectStack.push_back( obj );
  if (weakPtr)
    {
    // pointer to this object was weak, but the object hadn't been created
    // yet;  The ObjectStack will hold the only reference until non-weak
    // references are made to the object (and registered at the top of this
    // fn).  The extra reference held by the ObjectStack gets removed when
    // we are done reading (delete the this->Internal structure).
    obj->UnRegister(0);
    }
  this->Internal->IdToObject[id] = obj;
  this->Internal->Push(elem);
  vtkKWESerializableObject *serializableObject = 
    vtkKWESerializableObject::SafeDownCast(obj);
  if (serializableObject)
    {
    serializableObject->Serialize(this);
    }
  else if (vtkInformation::SafeDownCast(obj))
    {
    this->Serialize(elem, vtkInformation::SafeDownCast(obj));
    }
  else
    {
    vtkKWESerializationHelperMap::Serialize(obj, this);
    }

  this->Internal->Pop();
  return obj;
}

//----------------------------------------------------------------------------
int vtkKWEXMLArchiveReader::ParseStream(istream& str)
{
  vtkSmartPointer<vtkKWEXMLParser> parser = 
    vtkSmartPointer<vtkKWEXMLParser>::New();
  parser->SetStream(&str);
  int result = parser->Parse();
  this->SetRootElement(parser->GetRootElement());
  return result;
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

namespace
{
vtkKWEXMLElement* BaseSerialize(vtkKWEXMLArchiveReaderInternals* internal,
  const char* name)
{
  vtkKWEXMLElement* root = internal->Top();
  if (!root)
    {
    vtkGenericWarningMacro("Serialize cannot be called outside serialization");
    return 0;
    }
  
  return root->FindNestedElementByName(name);
}
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, 
                                       vtkstd::vector<vtkSmartPointer<vtkObject> >& objs,
                                       bool vtkNotUsed(weakPtr)/*=false*/)
{
  objs.clear();
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }

  unsigned int nnested = elem->GetNumberOfNestedElements();
  for(unsigned int i=0; i<nnested; i++)
    {
    vtkKWEXMLElement* objElem = elem->GetNestedElement(i);
    int id;
    if (objElem->GetScalarAttribute("to_id", &id))
      {
      vtkObject* obj = this->ReadObject(id, false);
      if (obj)
        {
        objs.push_back(obj);
        // ReadObject incremented the ReferenceCount (weakPtr = false), or
        // created the object (ReferenceCount = 1); we then stuff it in the 
        // vector , which is where we were trying to get it but has the
        // side effect of further incrementing the ReferenceCount.  To prevent
        // a leak we need to decerment the ReferenceCount.
        obj->UnRegister(0);
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, 
                                       vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >& map)
{
  map.clear();
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }

  this->Internal->Push(elem);
  unsigned int nnested = elem->GetNumberOfNestedElements();
  for(unsigned int i=0; i<nnested; i++)
    {
    vtkKWEXMLElement* vecElem = elem->GetNestedElement(i);
    // Take out Key_ and convert to int
    int key = atoi(vecElem->GetName() + 4);
    this->Serialize(vecElem->GetName(), map[key]);
    }
  this->Internal->Pop();
}

namespace
{
template <typename KeyType, typename ValueType>
void SerializeScalarKey(vtkInformation* info, KeyType* key, vtkKWEXMLElement* elem)
{
  ValueType val;
  if (elem->GetScalarAttribute("value", &val))
    {
    info->Set(key, val);
    }
}
    
//----------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void SerializeVectorKey(vtkInformation* info, KeyType* key, vtkKWEXMLElement* elem)
{
  int length;
  if (!elem->GetScalarAttribute("length", &length))
    {
    return;
    }
  if (length > 0)
    {
    ValueType* vals = new ValueType[length];
    if (elem->GetVectorAttribute("values", length, vals))
      {
      info->Set(key, vals, length);
      }
    delete[] vals;
    }
}

//----------------------------------------------------------------------------
void SerializeKeyVectorKey(vtkInformation* info,
          vtkInformationKeyVectorKey* key, vtkKWEXMLElement* elem)
{
  int length;
  if (!elem->GetScalarAttribute("length", &length))
    {
    return;
    }
  if (length <= 0)
    {
    return;
    }

  const char* values = elem->GetAttribute("values");
  if (!values)
    {
    return;
    }

  vtksys_ios::istringstream valueStr(values);
  for (int i = 0; i < length; ++i)
    {
    vtkstd::string keyName;
    valueStr >> keyName;

    vtkInformationKey* tmpKey =
      vtkKWEInformationKeyMap::FindKey(keyName.c_str());
    if (tmpKey)
      {
      info->Append(key, tmpKey); 
      }
    else
      {
      vtkGenericWarningMacro("Lookup failed for vtkInformationKey: "
                            << keyName.c_str());
      }
    }

}

}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, vtkInformation* info)
{
  this->Serialize(BaseSerialize(this->Internal, name), info);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(vtkKWEXMLElement* elem, vtkInformation* info)
{
  if (!elem)
    {
    return;
    }

  info->Clear();
  unsigned int nnested = elem->GetNumberOfNestedElements();
  for(unsigned int i=0; i<nnested; i++)
    {
    vtkKWEXMLElement* keyElem = elem->GetNestedElement(i);
    vtkInformationKey* key = vtkKWEInformationKeyMap::FindKey(keyElem->GetName());
    if (!key)
      {
      vtkErrorMacro("Lookup failed for vtkInformationKey: " << keyElem->GetName());
      continue;
      }

    if (key->IsA("vtkInformationIntegerKey"))
      {
      SerializeScalarKey<vtkInformationIntegerKey, int>(info, 
        static_cast<vtkInformationIntegerKey*>(key), keyElem);
      }
    else if (key->IsA("vtkInformationDoubleKey"))
      {
      SerializeScalarKey<vtkInformationDoubleKey, double>(info, 
        static_cast<vtkInformationDoubleKey*>(key), keyElem);      
      }
    else if (key->IsA("vtkInformationIdTypeKey"))
      {
      SerializeScalarKey<vtkInformationIdTypeKey, vtkIdType>(info, 
        static_cast<vtkInformationIdTypeKey*>(key), keyElem);      
      }
    else if (key->IsA("vtkInformationStringKey"))
      {
      const char* val = keyElem->GetAttribute("value");
      if (val)
        {
        info->Set(static_cast<vtkInformationStringKey*>(key), val);
        }
      }
    else if (key->IsA("vtkInformationDoubleVectorKey"))
      {
      SerializeVectorKey<vtkInformationDoubleVectorKey, double>(info, 
        static_cast<vtkInformationDoubleVectorKey*>(key), keyElem);      
      }
    else if (key->IsA("vtkInformationIntegerVectorKey"))
      {
      SerializeVectorKey<vtkInformationIntegerVectorKey, int>(info, 
        static_cast<vtkInformationIntegerVectorKey*>(key), keyElem);      
      }
    else if (key->IsA("vtkInformationKeyVectorKey"))
      {
      SerializeKeyVectorKey(info, 
        static_cast<vtkInformationKeyVectorKey*>(key), keyElem);      
      }
    else if (key->IsA("vtkInformationObjectBaseKey"))
      {
      int id;
      if (keyElem->GetScalarAttribute("to_id", &id))
        {
        vtkObject *obj = this->ReadObject(id, false);
        if(obj)
          {
          info->Set(static_cast<vtkInformationObjectBaseKey*>(key), obj);

          // ReadObject incremented the ReferenceCount (weakPtr = false), or
          // created the object (ReferenceCount = 1); we then stuff it in the 
          // information object, which is where we were trying to get it but 
          // has the side effect of further incrementing the ReferenceCount.  
          // To prevent a leak we need to decrment the ReferenceCount.
          obj->UnRegister(0);
          }
        }
      }
    else if (key->IsA("vtkInformationObjectBaseVectorKey"))
      {
      vtkInformationObjectBaseVectorKey* vecKey =
        static_cast<vtkInformationObjectBaseVectorKey*>(key);
      unsigned int length;
      if (keyElem->GetScalarAttribute("length", &length))
        {
        unsigned long* ids = new unsigned long[length];
        if (keyElem->GetVectorAttribute("ids", length, ids))
          {
          vecKey->Resize(info, length);
          for (unsigned int k = 0; k < length; ++k)
            {
            vtkObject *obj = this->ReadObject(ids[k], false);
            if(obj)
              {
              vecKey->Set(info, obj, k);

              // ReadObject incremented the ReferenceCount (weakPtr = false), or
              // created the object (ReferenceCount = 1); we then stuff it in the 
              // information object, which is where we were trying to get it but 
              // has the side effect of further incrementing the ReferenceCount.  
              // To prevent a leak we need to decrment the ReferenceCount.
              obj->UnRegister(0);
              }

            }
          }

        delete[] ids;
        }
      }
    }

}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, vtkObject*& obj,
                                       bool weakPtr/*=false*/)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }

  obj = 0;
  int id;
  if (elem->GetScalarAttribute("to_id", &id))
    {
    obj = this->ReadObject(id, weakPtr);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, int& val)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
    
  elem->GetScalarAttribute("value", &val);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, 
                                       int*& val, 
                                       unsigned int& length)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
  if (val)
    {
    delete[] val;
    val = 0;
    }
  length = 0;
  elem->GetScalarAttribute("length", &length);
  if (length > 0)
    {
    val = new int[length];
    elem->GetVectorAttribute("values", length, val);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, unsigned long& val)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
    
  elem->GetScalarAttribute("value", &val);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name,
                                       unsigned long*& val, 
                                       unsigned int& length)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
  if (val)
    {
    delete[] val;
    val = 0;
    }
  length = 0;
  elem->GetScalarAttribute("length", &length);
  if (length > 0)
    {
    val = new unsigned long[length];
    elem->GetVectorAttribute("values", length, val);
    }
}

//----------------------------------------------------------------------------
#if defined(VTK_USE_64BIT_IDS)
void vtkKWEXMLArchiveReader::Serialize(const char* name, vtkIdType& val)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
    
  elem->GetScalarAttribute("value", &val);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, vtkIdType*& val,
                                       unsigned int& length)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
  if (val)
    {
    delete[] val;
    val = 0;
    }
  length = 0;
  elem->GetScalarAttribute("length", &length);
  if (length > 0)
    {
    val = new vtkIdType[length];
    elem->GetVectorAttribute("values", length, val);
    }
}
#endif // if defined(VTK_USE_64BIT_IDS)

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, double& val)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
    
  elem->GetScalarAttribute("value", &val);
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name,
                                       double*& val,
                                       unsigned int& length)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
  if (val)
    {
    delete[] val;
    val = 0;
    }
  length = 0;
  elem->GetScalarAttribute("length", &length);
  if (length > 0)
    {
    val = new double[length];
    elem->GetVectorAttribute("values", length, val);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, char*& str)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }

  str = 0;
  const char* val = elem->GetAttribute("value");
  if (val)
    {
    str = vtksys::SystemTools::DuplicateString(val);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLArchiveReader::Serialize(const char* name, vtkstd::string& str)
{
  vtkKWEXMLElement* elem = BaseSerialize(this->Internal, name);
  if (!elem)
    {
    return;
    }
  
  const char* val = elem->GetAttribute("value");
  if (val)
    {
    str = val;
    }
  else
    {
    str = vtkstd::string();
    }

}
