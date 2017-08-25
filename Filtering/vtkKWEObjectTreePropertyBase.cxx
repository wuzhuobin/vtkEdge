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
#include "vtkKWEObjectTreePropertyBase.h"

#include "vtkKWEObjectTreeNodeBase.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleKey.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationIdTypeKey.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkInformationIterator.h"
#include "vtkInformationStringKey.h"
#include "vtkObjectFactory.h"
#include "vtkKWESerializer.h"

#include <vtkstd/set>
#include <vtkstd/string>

vtkCxxRevisionMacro(vtkKWEObjectTreePropertyBase, "$Revision: 1774 $");

vtkInformationKeyMacro(vtkKWEObjectTreePropertyBase, IS_INHERITABLE, Integer);

class vtkKWEObjectTreePropertyBaseReferencingNodes : public vtkstd::set< vtkKWEObjectTreeNodeBase* >
{
};

//-----------------------------------------------------------------------------
vtkKWEObjectTreePropertyBase::vtkKWEObjectTreePropertyBase()
{
  this->Attributes = vtkInformation::New();
  this->ReferencingNodes = new vtkKWEObjectTreePropertyBaseReferencingNodes;
  this->SetIsInheritable(true); // by default properties are inheritable
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreePropertyBase::~vtkKWEObjectTreePropertyBase()
{
  this->Attributes->Delete();
  if (this->ReferencingNodes->size() > 0)
    {
    vtkErrorMacro("We seem to think that someone still is refercing us.... so should be destructing!");
    }
  delete this->ReferencingNodes;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::Modified()
{
  this->Superclass::Modified();

  vtkKWEObjectTreePropertyBaseReferencingNodes::const_iterator iterator;
  for (iterator = this->ReferencingNodes->begin();
    iterator != this->ReferencingNodes->end(); iterator++)
    {
    (*iterator)->UpdateTreeModifiedTime( this->GetMTime() );
    }
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreePropertyBase::GetIsInheritable()
{
  if (this->Attributes->Has(IS_INHERITABLE()))
    {
    return this->Attributes->Get( IS_INHERITABLE() ) ? true : false;
    }
  return true;  // if not set, return true
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::SetIsInheritable(bool isInheritable)
{
  // if already set AND same as "new" value, do nothing
  if (this->Attributes->Has(IS_INHERITABLE()) && this->GetIsInheritable() == isInheritable)
    {
    return;
    }
  // otherwise need to set and indicate that we've been modified
  this->Attributes->Set(IS_INHERITABLE(), isInheritable ? 1 : 0);
  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::UnsetIsInheritable()
{
  this->Attributes->Remove(IS_INHERITABLE());
  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::AddReferencingNode(vtkKWEObjectTreeNodeBase *node)
{
  this->ReferencingNodes->insert(node);
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::RemoveReferencingNode(vtkKWEObjectTreeNodeBase *node)
{
  this->ReferencingNodes->erase(node);
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::Serialize(vtkKWESerializer* ser)
{
  ser->Serialize("Attributes", this->Attributes);
  if (ser->IsWriting())
    {
    vtkstd::vector< vtkSmartPointer<vtkObject> > myVector =
      vtkKWESerializer::ToBase<vtkKWEObjectTreePropertyBaseReferencingNodes>( *this->ReferencingNodes );
    ser->Serialize("ReferencingNodes", myVector, true);
    }
  else
    {
    // The ReferencingNodes are "weak" pointers;  still upon deserializaing the method
    // expects a vector of SmartPointers.  When we call FromBase we fill our
    // non-referencing counted container, giving the desired "weak" pointers (and no leak)
    vtkstd::vector< vtkSmartPointer<vtkObject> > myVector;
    ser->Serialize("ReferencingNodes", myVector, true);
    vtkKWESerializer::FromBase<vtkKWEObjectTreeNodeBase>(myVector, *this->ReferencingNodes);
    }
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreePropertyBase::GetNumberOfAttributes()
{
  vtkSmartPointer<vtkInformationIterator> attributeIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  attributeIterator->SetInformation( this->Attributes );

  int numberOfAttributes = 0;
  for (attributeIterator->InitTraversal(); !attributeIterator->IsDoneWithTraversal();
    attributeIterator->GoToNextItem())
    {
    numberOfAttributes++;
    }
  return numberOfAttributes;
}

//-----------------------------------------------------------------------------
template <typename KeyType>
bool AreScalarKeysEqual(vtkInformation* thisInfo, vtkInformation* testInfo,
  KeyType* key)
{
  // if thisInfo doesn't have the key or the key values don't match, return false
  if (!thisInfo->Has(key) || testInfo->Get(key) != thisInfo->Get(key))
    {
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool AreVectorKeysEqual(vtkInformation* thisInfo, vtkInformation* testInfo,
  KeyType* key)
{
  if (!thisInfo->Has(key) ||
    thisInfo->Length(key) != testInfo->Length(key))
    {
    return false;
    }
  ValueType *thisValues = thisInfo->Get(key);
  ValueType *testValues = testInfo->Get(key);
  int length = testInfo->Length(key);
  for (int i = 0; i < length; i++)
    {
    if (thisValues[i] != testValues[i])
      {
      return false;
      }
    }
  return true;
}

// ---------------------------------------------------------------------------
bool vtkKWEObjectTreePropertyBase::IsEqualTo(vtkKWEObjectTreePropertyBase *testProperty,
                                             bool canBeSuperset/*=false*/)
{
  if (!testProperty)
    {
    return false;  // can't be EqualTo if it doesn't exist!
    }

  if (strcmp(this->GetClassName(), testProperty->GetClassName()))
    {
    return false; // must be the same type of Property to be equal!
    }

  int myNumAttributes = this->GetNumberOfAttributes();
  int testNumAttributes = testProperty->GetNumberOfAttributes();
  if (myNumAttributes < testNumAttributes ||
    (myNumAttributes > testNumAttributes && !canBeSuperset))
    {
    // if the number of attributes doesn't match, then we can't be equal...
    // unless "this" Property has more and it can be a superset of the property
    // we are comparing ourselves to
    return false;
    }

  // test to see if the attributes match; at this point we know we either have
  // the same number of attributes or "this" has more but ok if it is a
  // superset.  Thus just test every attribute in the testProperty to see if
  // there is a matching attribute in "this" Property... if so, then they match
  vtkSmartPointer<vtkInformationIterator> attributeIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  attributeIterator->SetInformation( testProperty->Attributes );
  for (attributeIterator->InitTraversal(); !attributeIterator->IsDoneWithTraversal();
    attributeIterator->GoToNextItem())
    {
    vtkInformationKey *key = attributeIterator->GetCurrentKey();
    if (key->IsA("vtkInformationIntegerKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationIntegerKey>( this->Attributes,
        testProperty->Attributes, static_cast<vtkInformationIntegerKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationDoubleKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationDoubleKey>( this->Attributes,
        testProperty->Attributes, static_cast<vtkInformationDoubleKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationIdTypeKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationIdTypeKey>( this->Attributes,
        testProperty->Attributes, static_cast<vtkInformationIdTypeKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationStringKey"))
      {
      vtkInformationStringKey *stringKey = static_cast<vtkInformationStringKey*>(key);
      if (!this->Attributes->Has(stringKey) ||
        strcmp(this->Attributes->Get(stringKey), testProperty->Attributes->Get(stringKey)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationIntegerVectorKey"))
      {
      if (!AreVectorKeysEqual<vtkInformationIntegerVectorKey, int>(
        this->Attributes, testProperty->Attributes,
        static_cast<vtkInformationIntegerVectorKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationDoubleVectorKey"))
      {
      if (!AreVectorKeysEqual<vtkInformationDoubleVectorKey, double>(
        this->Attributes, testProperty->Attributes,
        static_cast<vtkInformationDoubleVectorKey*>(key)))
        {
        return false;
        }
      }
    }

  return true;
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreePropertyBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Attributes:\n";
  this->Attributes->PrintSelf(os, indent.GetNextIndent());
}
