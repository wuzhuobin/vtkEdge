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
#include "vtkKWECommonSerializationHelper.h"

#include "vtkDoubleArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkIntArray.h"
#include "vtkKWESerializationHelperMap.h"
#include "vtkKWESerializer.h"
#include "vtkKWEXMLElement.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"

vtkCxxRevisionMacro(vtkKWECommonSerializationHelper, "$Revision: 757 $");
vtkStandardNewMacro(vtkKWECommonSerializationHelper);

//-----------------------------------------------------------------------------
vtkKWECommonSerializationHelper::vtkKWECommonSerializationHelper()
{
}

//-----------------------------------------------------------------------------
int vtkKWECommonSerializationHelper::Serialize(vtkObject *object, 
                                               vtkKWESerializer *serializer)
{
  if (vtkTransform::SafeDownCast(object))
    {
    this->SerializeTransform(vtkTransform::SafeDownCast(object), serializer);
    return 1;
    }
  // make sure object is a type of vtkDataArray that is supported
  else if (vtkDataArray::SafeDownCast(object) && 
    this->GetSerializationType(object))
    {
    this->SerializeDataArray(vtkDataArray::SafeDownCast(object), serializer);
    return 1;
    }

  return 0;
}

//-----------------------------------------------------------------------------
void vtkKWECommonSerializationHelper::RegisterWithHelperMap() 
{
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkTransform", this);
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkIdTypeArray", this);
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkDoubleArray", this);
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkIntArray", this);
}

//-----------------------------------------------------------------------------
void vtkKWECommonSerializationHelper::UnRegisterWithHelperMap() 
{
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkTransform", this);
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkIdTypeArray", this);
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkDoubleArray", this);
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkIntArray", this);
}

//-----------------------------------------------------------------------------
void vtkKWECommonSerializationHelper::SerializeTransform(vtkTransform *transform,
                                                         vtkKWESerializer *serializer)
{
  if (serializer->IsWriting())
    {
    unsigned int length = 16;
    double *elementsPtr = *transform->GetMatrix()->Element;
    // just the matrix for now... plan to add better support for serializing transforms
    serializer->Serialize("Matrix", elementsPtr, length);
    }
  else 
    {
    unsigned int length = 0;
    double *elements = 0;
    serializer->Serialize("Matrix", elements, length);
    if (length > 0)
      {
      transform->Concatenate(elements);
      delete [] elements;
      }
    }
}

//-----------------------------------------------------------------------------
void vtkKWECommonSerializationHelper::SerializeDataArray(vtkDataArray *dataArray,
                                                         vtkKWESerializer *serializer)
{
  if (serializer->IsWriting())
    {
    if (dataArray->HasInformation())
      {
      serializer->Serialize("ArrayInformation", dataArray->GetInformation());
      }

    int numberOfComponents = dataArray->GetNumberOfComponents();
    serializer->Serialize("NumberOfComponents", numberOfComponents);

    // Upon reading we are going to be limited to array of length vtkIdType,
    // which if not using 64-bit ID's means at most ~2^31 (~2 billion) values...
    // which is a LOT when considering this will be written as ASCII (this 
    // serialization is NOT intended for storing LARGE data).  Therefore, for the
    // time being, we write out at most what can be represented by an int.
    unsigned long numberOfValuesLong = dataArray->GetDataSize();
    unsigned int numberOfValues = static_cast<unsigned int>(
      static_cast<int>(numberOfValuesLong) );
    if (numberOfValues < numberOfValuesLong)
      {
      vtkErrorMacro("Unable to Serialize the entire array; too large!");
      }

    // now handle the type specfic
    if (vtkIdTypeArray::SafeDownCast(dataArray))
      {
      vtkIdType *arrayPtr = vtkIdTypeArray::SafeDownCast(dataArray)->GetPointer(0);
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      }
    else if (vtkDoubleArray::SafeDownCast(dataArray))
      {
      double *arrayPtr = vtkDoubleArray::SafeDownCast(dataArray)->GetPointer(0);
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      }
    else if (vtkIntArray::SafeDownCast(dataArray))
      {
      int *arrayPtr = vtkIntArray::SafeDownCast(dataArray)->GetPointer(0);
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      }
    }
  else 
    {
    // handle vtkInformation, if present
    vtkSmartPointer<vtkInformation> infoObject = 
      vtkSmartPointer<vtkInformation>::New();
    serializer->Serialize("ArrayInformation", infoObject);
    if (infoObject->GetNumberOfKeys() > 0)
      {
      dataArray->GetInformation()->Copy(infoObject, 1);
      }

    // how many components
    int numberOfComponents;
    serializer->Serialize("NumberOfComponents", numberOfComponents);
    dataArray->SetNumberOfComponents(numberOfComponents);

    // now
    unsigned int numberOfValues = 0;
    if (vtkIdTypeArray::SafeDownCast(dataArray))
      {
      vtkIdType *arrayPtr = 0;
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      if (numberOfValues > 0)
        {
        // the dataArray assumes reposibility for the array allocated
        // in Serialize(), indicated by the final arguement of "0"
        vtkIdTypeArray::SafeDownCast(dataArray)->SetArray(
          arrayPtr, static_cast<vtkIdType>(numberOfValues), 0, 
          vtkIdTypeArray::VTK_DATA_ARRAY_DELETE);
        }
      }
    else if (vtkDoubleArray::SafeDownCast(dataArray))
      {
      double *arrayPtr = 0;
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      if (numberOfValues > 0)
        {
        // the dataArray assumes reposibility for the array allocated
        // in Serialize(), indicated by the final arguement of "0"
        vtkDoubleArray::SafeDownCast(dataArray)->SetArray(
          arrayPtr, static_cast<vtkIdType>(numberOfValues), 0,
          vtkDoubleArray::VTK_DATA_ARRAY_DELETE);
        }
      }
    else if (vtkIntArray::SafeDownCast(dataArray))
      {
      int *arrayPtr = 0;
      serializer->Serialize("Array", arrayPtr, numberOfValues);
      if (numberOfValues > 0)
        {
        // the dataArray assumes reposibility for the array allocated
        // in Serialize(), indicated by the final arguement of "0"
        vtkIntArray::SafeDownCast(dataArray)->SetArray(
          arrayPtr, static_cast<vtkIdType>(numberOfValues), 0,
          vtkIntArray::VTK_DATA_ARRAY_DELETE);
        }
      }
    }
}

//-----------------------------------------------------------------------------
const char *vtkKWECommonSerializationHelper::GetSerializationType(vtkObject *object)
{
  if (vtkTransform::SafeDownCast(object))
    {
    return "vtkTransform";
    }  
  if (vtkIdTypeArray::SafeDownCast(object))
    {
    return "vtkIdTypeArray";
    }
  if (vtkDoubleArray::SafeDownCast(object))
    {
    return "vtkDoubleArray";
    }
  if (vtkIntArray::SafeDownCast(object))
    {
    return "vtkIntArray";
    }
  return 0;
}

//-----------------------------------------------------------------------------
void vtkKWECommonSerializationHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Supported ClassTypes:\n";
  os << indent.GetNextIndent() << "vtkTransform\n";
  os << indent.GetNextIndent() << "vtkDoubleArray\n";
  os << indent.GetNextIndent() << "vtkIntArray\n";
  os << indent.GetNextIndent() << "vtkIdTypeArray\n";
}
