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
#include "vtkKWEObjectTreeNodeBase.h"

#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationIterator.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationStringKey.h"
#include "vtkKWEUUID.h"
#include "vtkKWEObjectTreePropertyBase.h"
#include "vtkKWESerializer.h"
#include "vtkKWEObjectTreeNodeIterator.h"
#include "vtkObjectFactory.h"

#include <vtkstd/vector>

vtkCxxRevisionMacro(vtkKWEObjectTreeNodeBase, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEObjectTreeNodeBase);

vtkInformationKeyMacro(vtkKWEObjectTreeNodeBase, NAME, String);
vtkInformationKeyMacro(vtkKWEObjectTreeNodeBase, UUID, String);
vtkInformationKeyMacro(vtkKWEObjectTreeNodeBase, STATE, Integer);
vtkInformationKeyMacro(vtkKWEObjectTreeNodeBase, INHERIT_PROPERTIES, Integer);

// PIMPL
class vtkKWEObjectTreeNodeBaseChildren : public vtkstd::vector< vtkSmartPointer<vtkKWEObjectTreeNodeBase> >
{
};

//-----------------------------------------------------------------------------
vtkKWEObjectTreeNodeBase::vtkKWEObjectTreeNodeBase()
{
  this->Parent = 0;
  this->NodeObject = 0;
  this->Attributes = vtkInformation::New();
  this->Properties = vtkInformation::New();
  this->TreeModifiedTime = 0;

  this->Children = new vtkKWEObjectTreeNodeBaseChildren;
  this->SetStateToActive();
  this->InheritPropertiesOn();
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreeNodeBase::~vtkKWEObjectTreeNodeBase()
{
  // visit children and tell them they no longer have a parent ("someone" else,
  // may be holding on to the child as a tree on it's own, but we need to tell
  // it it no longer has a parent)
  vtkKWEObjectTreeNodeBaseChildren::const_iterator childIterator;
  for (childIterator = this->Children->begin();
    childIterator != this->Children->end(); childIterator++)
    {
    (*childIterator)->SetParent(0);
    }

  delete this->Children;

  this->SetParent(0);
  this->SetNodeObject(0);
  this->Attributes->Delete();
  this->RemoveAllPropertiesInternal();
  this->Properties->Delete();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SetNodeObject(vtkObject *object)
{
  if (this->NodeObject == object)
    {
    return;
    }

  if ( vtkKWEObjectTreeNodeBase::SafeDownCast(object) )
    {
    vtkErrorMacro("Setting the object to be an ObjectTreeNode is NOT allowed.  Use a vtkKWEObjectTreeReferenceNode to achieve a similar result!");
    return;
    }

  if (this->NodeObject)
    {
    this->NodeObject->UnRegister( this );
    }

  this->NodeObject = object;
  if (this->NodeObject)
    {
    this->NodeObject->Register( this );
    }
  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SetParent(vtkKWEObjectTreeNodeBase *parent)
{
  if (this->Parent == parent)
    {
    return;
    }

  // we shouldn't be calling this if we already have a parent, but just in case
  if (this->Parent && parent)
    {
    vtkErrorMacro("Can't set parent; already has parent");
    return;
    }
  this->Parent = parent;
  this->Modified();
}

//-----------------------------------------------------------------------------
unsigned int vtkKWEObjectTreeNodeBase::GetNumberOfChildren()
{
  return static_cast<unsigned int>(this->Children->size());
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreeNodeBase *vtkKWEObjectTreeNodeBase::GetChild(unsigned int index)
{
  if (index >= this->Children->size())
    {
    vtkErrorMacro("Invalid child index!");
    return 0;
    }

  return this->Children->at(index);
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::AddChild(vtkKWEObjectTreeNodeBase *childNode)
{
  // test for parent usefule in preventing cycles as well as guaranteeing the
  // child is unique (if childNode is already a child of "this", then it
  // will have a parent, and we don't add childNode if it alrady has a parent)
  if (childNode->GetParent())
    {
    vtkErrorMacro("Unable to add child because it already has a parent!");
    return -1;
    }

  if (childNode->IsDescendant(this))
    {
    vtkErrorMacro("Unable to add child because a cycle would be created!");
    return -1;
    }

  this->Children->push_back(childNode);
  childNode->SetParent(this);

  this->Modified();
  return static_cast<int>(this->Children->size() - 1);
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::InsertChild(unsigned int index,
                                          vtkKWEObjectTreeNodeBase *childNode)
{
  // test for parent usefule in preventing cycles as well as guaranteeing the
  // child is unique (if childNode is already a child of "this", then it
  // will have a parent, and we don't add childNode if it alrady has a parent)
  if (childNode->GetParent())
    {
    vtkErrorMacro("Unable to add child because it already has a parent!");
    return 0;
    }

  if (childNode->IsDescendant(this))
    {
    vtkErrorMacro("Unable to add child because a cycle would be created!");
    return 0;
    }

  if (static_cast<size_t>(index) > this->Children->size())
    {
    vtkErrorMacro(
      "Unable to add child because the index is out of range: " << index );
    return 0;
    }

  this->Children->insert( this->Children->begin() + index, childNode);
  childNode->SetParent(this);

  this->Modified();
  return 1;
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::RemoveChild(vtkKWEObjectTreeNodeBase *childNode)
{
  int index = 0;
  vtkKWEObjectTreeNodeBaseChildren::iterator childIterator;
  for (childIterator = this->Children->begin();
    childIterator != this->Children->end(); childIterator++, index++)
    {
    if (*childIterator == childNode)
      {
      childNode->SetParent(0);
      this->Children->erase(childIterator);
      this->Modified();
      return index;
      }
    }

  return -1;
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::RemoveChild(unsigned int index)
{
  if (static_cast<size_t>(index) >= this->Children->size())
    {
    vtkErrorMacro(
      "Unable to remove child because the index is out of range: " << index );
    return 0;
    }

  this->Children->erase( this->Children->begin() + index );
  this->Modified();
  return 1;
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::GetNumberOfProperties()
{
  return this->GetNumberOfInformationEntries( this->Properties );
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::GetNumberOfInformationEntries(vtkInformation *infoObject)
{
  vtkSmartPointer<vtkInformationIterator> infoIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  infoIterator->SetInformation( infoObject );

  int numberOfInfoValues = 0;
  for (infoIterator->InitTraversal(); !infoIterator->IsDoneWithTraversal();
    infoIterator->GoToNextItem())
    {
    numberOfInfoValues++;
    }
  return numberOfInfoValues;
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::AddProperty(vtkKWEObjectTreePropertyBase *nodeProperty)
{
  if ( this->Properties->Has(nodeProperty->GetKey()) )
    {
    return 0;
    }

  this->Properties->Set(nodeProperty->GetKey(), nodeProperty);
  nodeProperty->AddReferencingNode(this);
  this->Modified();
  return 1;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::GetAllProperties(vtkInformation *allProperties)
{
  if (!allProperties)
    {
    vtkErrorMacro("Must pass in valid vtkInformation object");
    return;
    }

  // make sure empty to start
  allProperties->Clear();

  // copy the "local" (non-inherited) Properties
  allProperties->Copy(this->Properties);

  if (this->CanInheritProperties() && this->Parent)
    {
    this->Parent->AddInheritedProperties( allProperties );
    }
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::AddInheritedProperties(vtkInformation *allProperties)
{
  vtkInformationObjectBaseKey *key;
  vtkKWEObjectTreePropertyBase *tmpProperty;

  // iterate through all our "local" properties; add to allProperties if not
  // already in allProperties AND inheritable
  vtkSmartPointer<vtkInformationIterator> propertyIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  propertyIterator->SetInformation( this->Properties );
  for (propertyIterator->InitTraversal(); !propertyIterator->IsDoneWithTraversal();
    propertyIterator->GoToNextItem())
    {
    key = vtkInformationObjectBaseKey::SafeDownCast( propertyIterator->GetCurrentKey() );
    if (allProperties->Has(key))
      {
      // already has the Property, so won't inherit it
      continue;
      }
    tmpProperty = vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(key) );
    if (tmpProperty->IsInheritable())
      {
      allProperties->Set(key, tmpProperty);
      }
    }

  // if we can inherit properties, see what else is out tere
  if (this->CanInheritProperties() && this->Parent)
    {
    this->Parent->AddInheritedProperties( allProperties );
    }
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreePropertyBase* vtkKWEObjectTreeNodeBase::GetProperty(
  vtkInformationObjectBaseKey *propertyKey, bool &inheritedProperty,
  bool includeInheritance/*=false*/)
{
  vtkKWEObjectTreePropertyBase *requestedProperty =
    vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(propertyKey) );
  if (requestedProperty)
    {
    inheritedProperty = false;
    return requestedProperty;
    }
  else if (includeInheritance && this->CanInheritProperties() && this->Parent)
    {
    requestedProperty = this->Parent->GetProperty(propertyKey, inheritedProperty, true);
    inheritedProperty = true;
    // some ancestor did have the property, but is it an inheritable property
    if (requestedProperty && !requestedProperty->IsInheritable())
      {
      requestedProperty = 0;
      }
    }
  return requestedProperty;
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::RemoveProperty(vtkKWEObjectTreePropertyBase *nodeProperty)
{
  return this->RemoveProperty( nodeProperty->GetKey() );
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::RemoveProperty(vtkInformationObjectBaseKey *propertyKey)
{
  if (this->Properties->Has(propertyKey))
    {
    vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(propertyKey) )->
      RemoveReferencingNode(this);
    this->Properties->Remove(propertyKey);
    this->Modified();
    return 1;
    }
  return 0;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::RemoveAllProperties()
{
  if (this->RemoveAllPropertiesInternal())
    {
    this->Modified();
    }
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeBase::RemoveAllPropertiesInternal()
{
  vtkSmartPointer<vtkInformationIterator> propertyIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  propertyIterator->SetInformation( this->Properties );

  bool hadProperties = false;
  vtkInformationObjectBaseKey *key;
  for (propertyIterator->InitTraversal(); !propertyIterator->IsDoneWithTraversal();
    propertyIterator->GoToNextItem())
    {
    hadProperties = true;
    key = vtkInformationObjectBaseKey::SafeDownCast(
      propertyIterator->GetCurrentKey() );
    vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(key) )->
      RemoveReferencingNode(this);
    }

  if (hadProperties)
    {
    this->Properties->Clear();
    }

  return hadProperties;
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeBase::IsDescendant(vtkKWEObjectTreeNodeBase *testNode)
{
  if (this == testNode)
    {
    return true;
    }

  vtkKWEObjectTreeNodeBaseChildren::const_iterator childIterator;
  for (childIterator = this->Children->begin();
    childIterator != this->Children->end(); childIterator++)
    {
    if ((*childIterator)->IsDescendant(testNode))
      {
      return true;
      }
    }
  return false;
}

//-----------------------------------------------------------------------------
// Create a UUID for this node (if it doesn't already exist).  Returns 1
// on success (or already exists)
int vtkKWEObjectTreeNodeBase::CreateUUID()
{
  if (this->Attributes->Has(UUID()))
    {
    return 0;
    }

  int retValue = 1;
  unsigned char uuid[16];
  if (vtkKWEUUID::GenerateUUID(uuid) == -1)
    {
    vtkKWEUUID::ConstructUUID(uuid);
    retValue = 2;
    }

  vtkstd::string uuidString;
  vtkKWEUUID::ConvertBinaryUUIDToString(uuid, uuidString);
  this->SetUUID(uuidString.c_str());

  return retValue;
}

//-----------------------------------------------------------------------------
// Returns the UUID for this node.  The UUID is not created until requested.
const char *vtkKWEObjectTreeNodeBase::GetUUID()
{
  return this->Attributes->Get(UUID());
}

//-----------------------------------------------------------------------------
// Sets the UUID for this node.
void vtkKWEObjectTreeNodeBase::SetUUID(const char *uuid)
{
  this->Attributes->Set(UUID(), uuid);
}

//-----------------------------------------------------------------------------
// Returns the UUID for this node.  The UUID is not created until requested.
void vtkKWEObjectTreeNodeBase::ClearUUID()
{
  return this->Attributes->Remove(UUID());
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SetName(const char *name)
{
  this->Attributes->Set(NAME(), name);
}

//-----------------------------------------------------------------------------
const char *vtkKWEObjectTreeNodeBase::GetName()
{
  return this->Attributes->Get(NAME());
}


//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SetState(int nodeState)
{
  this->Attributes->Set(STATE(), nodeState);
}

//-----------------------------------------------------------------------------
int vtkKWEObjectTreeNodeBase::GetState()
{
  return this->Attributes->Get(STATE());
}

// ---------------------------------------------------------------------------
// Description:
// Return the state as a descriptive character string.
const char *vtkKWEObjectTreeNodeBase::GetStateAsString(void)
{
  switch (this->GetState())
    {
    case ACTIVE_STATE:
      return "Active";
    case INACTIVE_STATE:
      return "Inactive";
    }

  return "Unrecognized";
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeBase::GetInheritProperties()
{
  return this->Attributes->Get(INHERIT_PROPERTIES()) ? true : false;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SetInheritProperties(bool inheritState)
{
  this->Attributes->Set(INHERIT_PROPERTIES(), inheritState ? 1 : 0);
}

//-----------------------------------------------------------------------------
unsigned long vtkKWEObjectTreeNodeBase::GetMTime()
{
  unsigned long propMTime;
  unsigned long mTime = this->Superclass::GetMTime();

  if (this->Attributes->GetMTime() > mTime)
    {
    mTime = this->Attributes->GetMTime();
    }

  // consider the MTime of our Properties as well;
  vtkSmartPointer<vtkInformationIterator> propertyIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  propertyIterator->SetInformation( this->Properties );

  vtkInformationObjectBaseKey *key;
  for (propertyIterator->InitTraversal(); !propertyIterator->IsDoneWithTraversal();
    propertyIterator->GoToNextItem())
    {
    key = vtkInformationObjectBaseKey::SafeDownCast(
      propertyIterator->GetCurrentKey() );
    propMTime =
      vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(key) )->GetMTime();
    if ( propMTime > mTime )
      {
      mTime = propMTime;
      }
    }

  return mTime;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::Modified()
{
  this->Superclass::Modified();
  this->UpdateTreeModifiedTime( this->GetMTime() );
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::UpdateTreeModifiedTime(unsigned long treeTime)
{
  if (this->TreeModifiedTime == treeTime)
    {
    return;
    }

  this->TreeModifiedTime = treeTime;
  if (this->Parent)
    {
    this->Parent->UpdateTreeModifiedTime(treeTime);
    }
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeBase::IsEqualTo(vtkKWEObjectTreeNodeBase *testNode,
                                         bool checkDescendants,
                                         bool canBeSuperset/*=false*/,
                                         bool considerInheritedProperties/*=false*/)
{
  if (!testNode)
    {
    return false;  // can't be EqualTo if it doesn't exist!
    }

  // If canBeSuperset == false, must be the EXACT same type of Node to be equal;
  // otherwise we can be a subclass of the testNode type
  if ((!canBeSuperset && strcmp(this->GetClassName(), testNode->GetClassName())) ||
    (canBeSuperset && !this->IsA(testNode->GetClassName())))
    {
    return false;
    }

  // Do our Attributes match?
  if (testNode->GetState() != this->GetState()) // we always have a state!
    {
    return false;
    }
  // can be a match if the testNode does NOT have a name be we do... IF canBeSuperset == true
  if ((testNode->GetName() && this->GetName() && strcmp(testNode->GetName(), this->GetName())) ||
    (!testNode->GetName() && this->GetName() && !canBeSuperset) ||
    (testNode->GetName() && !this->GetName()) )
    {
    return false;
    }
  // can be a match if the testNode does NOT have a UUID be we do... IF canBeSuperset == true
  if ((testNode->GetUUID() && this->GetUUID() && strcmp(testNode->GetUUID(), this->GetUUID())) ||
    (!testNode->GetUUID() && this->GetUUID() && !canBeSuperset) ||
    (testNode->GetUUID() && !this->GetUUID()) )
    {
    return false;
    }

  // compare Objects... is only one or the other set (if so, return false,
  // unless canBeSuperset).
  if ((!testNode->NodeObject && this->NodeObject && !canBeSuperset) ||
    (testNode->NodeObject && !this->NodeObject) ||
    // if both objects exist, then type must match exactly if can't be superset OR
    // this->NodeObject only has to be an IsA (subclass of) if it can be a superset
    ((testNode->NodeObject && this->NodeObject) &&
    ((!canBeSuperset && strcmp(testNode->NodeObject->GetClassName(), this->NodeObject->GetClassName())) ||
    ((canBeSuperset && !this->NodeObject->IsA(testNode->NodeObject->GetClassName()))))))
    {
    return false;
    }

  int myNumProperties, testNumProperties;
  vtkSmartPointer<vtkInformation> myProperties, testNodeProperties;
  if (considerInheritedProperties)
    {
    // if we inherited properties are considered for the purpose of this
    // comparison then build a complete set of properties for this node
    // (include inherited properties)
    myProperties = vtkSmartPointer<vtkInformation>::New();
    this->GetAllProperties(myProperties);
    myNumProperties = this->GetNumberOfInformationEntries(myProperties);

    testNodeProperties = vtkSmartPointer<vtkInformation>::New();
    testNode->GetAllProperties(testNodeProperties);
    testNumProperties = testNode->GetNumberOfInformationEntries(myProperties);
    }
  else
    {
    myNumProperties = this->GetNumberOfProperties();
    myProperties = this->Properties;
    testNumProperties = testNode->GetNumberOfProperties();
    testNodeProperties = testNode->Properties;
    }

  if (myNumProperties < testNumProperties ||
    (myNumProperties > testNumProperties && !canBeSuperset))
    {
    // if the number of properties doesn't match, then we can't be equal...
    // unless "this" has more and it can be a superset of the testNode we are
    // comparing ourselves to
    return false;
    }

  // To be "equal", we must have that same set of Properties as the testNode...
  // and the Properties need to be equal
  vtkSmartPointer<vtkInformationIterator> propertyIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  propertyIterator->SetInformation( testNodeProperties );

  vtkInformationObjectBaseKey *key;
  for (propertyIterator->InitTraversal(); !propertyIterator->IsDoneWithTraversal();
    propertyIterator->GoToNextItem())
    {
    key = vtkInformationObjectBaseKey::SafeDownCast(
      propertyIterator->GetCurrentKey() );

    vtkKWEObjectTreePropertyBase *thisProperty =
      vtkKWEObjectTreePropertyBase::SafeDownCast(myProperties->Get(key));
    // if the Property doesn't exist in "this" then return false (not equal)...
    // (we know it exists in testNode since that is  what we're iterating over)
    if (!thisProperty)
      {
      return false;
      }
    vtkKWEObjectTreePropertyBase *testProperty =
      vtkKWEObjectTreePropertyBase::SafeDownCast(testNodeProperties->Get(key));
    // if IsEqualTo returns false, then "obviously" not equal
    if (!thisProperty->IsEqualTo( testProperty, canBeSuperset ))
      {
      return false;
      }
    }

  // finally, if we're checking descendants... do so
  if (checkDescendants)
    {
    // must have the same number of children
    if (this->GetNumberOfChildren() != testNode->GetNumberOfChildren())
      {
      return false;
      }

    // and they must be in the same order
    for (unsigned int i = 0; i < this->GetNumberOfChildren(); i++)
      {
      if (!this->GetChild(i)->IsEqualTo( testNode->GetChild(i), true, canBeSuperset ))
        {
        return false;
        }
      }
    }

  return true;

}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::SerializeObject(vtkKWESerializer* ser)
{
  if (ser->IsWriting())
    {
    if (this->NodeObject)
      {
      ser->Serialize("NodeObject", this->NodeObject);
      }
    }
  if (!ser->IsWriting())
    {
    if (this->NodeObject)
      {
      this->NodeObject->UnRegister( this );
      this->NodeObject = 0;
      }
    // the object is registered during serialization
    ser->Serialize("NodeObject", this->NodeObject);
    }
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::Serialize(vtkKWESerializer* ser)
{
  this->SerializeObject( ser );

  ser->Serialize("Attributes", this->Attributes);
  ser->Serialize("Properties", this->Properties);
  vtkObject *parent = this->Parent;
  ser->Serialize("Parent", parent, true); // true indicates it is a weak ptr

  if (ser->IsWriting())
    {
    vtkstd::vector< vtkSmartPointer<vtkObject> > myVector =
      vtkKWESerializer::ToBase<vtkstd::vector<vtkSmartPointer<vtkKWEObjectTreeNodeBase> > >( *this->Children );
    ser->Serialize("Children", myVector);
    }
  else
    {
    this->Parent = vtkKWEObjectTreeNodeBase::SafeDownCast(parent);
    vtkstd::vector< vtkSmartPointer<vtkObject> > myVector;
    ser->Serialize("Children", myVector);
    vtkKWESerializer::FromBase<vtkKWEObjectTreeNodeBase>(myVector, *this->Children);
    }
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::AddChildren(vtkKWEObjectTreeNodeIterator *iterator)
{
  vtkstd::vector<vtkKWEObjectTreeNodeBase*> children;
  children.insert(children.begin(), this->Children->begin(), this->Children->end());
  iterator->AddChildren(children);
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeNodeBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if (this->NodeObject)
    {
    os << indent << "NodeObject: \n";
    this->NodeObject->PrintSelf(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "NodeObject: (none)\n";
    }

  if (this->GetName())
    {
    os << indent << "Name: " << this->GetName() << "\n";
    }
  else
    {
    os << indent << "Name: (none)\n";
    }
  os << indent << "STATE: " << this->GetStateAsString() << "\n";

  if (this->GetUUID())
    {
    os << indent << "UUID: " << this->GetUUID() << "\n";
    }
  else
    {
    os << indent << "UUID: (none)\n";
    }

  os << indent << "PROPERTIES:\n";
  vtkSmartPointer<vtkInformationIterator> propertyIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  propertyIterator->SetInformation( this->Properties );

  vtkInformationObjectBaseKey *key;
  for (propertyIterator->InitTraversal(); !propertyIterator->IsDoneWithTraversal();
    propertyIterator->GoToNextItem())
    {
    key = vtkInformationObjectBaseKey::SafeDownCast(
      propertyIterator->GetCurrentKey() );
    vtkKWEObjectTreePropertyBase::SafeDownCast( this->Properties->Get(key) )->
      PrintSelf(os, indent.GetNextIndent());
    }
  os << indent << "END PROPERTIES:\n";

  // Print the children...
  os << indent << "CHILDREN:\n";
  vtkKWEObjectTreeNodeBaseChildren::const_iterator childIterator;
  for (childIterator = this->Children->begin();
    childIterator != this->Children->end(); childIterator++)
    {
    (*childIterator)->PrintSelf(os, indent.GetNextIndent());
    }
  os << indent << "END CHILDREN:\n";
}
