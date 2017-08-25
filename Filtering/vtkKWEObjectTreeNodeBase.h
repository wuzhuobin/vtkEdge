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
// .NAME vtkKWEObjectTreeNodeBase - Base class for ObjectTree nodes
// .SECTION Description
// The base class for ObjectTree nodes, vtkKWEObjectTreeNodeBase provides the
// ability add and remove children of a node while guaranteeing that a cycle
// isn't created.  Also, to change the characteristics of a "basic" node type,
// subclasses of vtkKWEObjectTreePropertyBase can be added to a node, though
// a node can only have one instance of each property type (such as
// vtkKWEObjectTreeUserProperty).  Though not implemented yet, descendants of
// a node could (or would?) inherit these properties, if they don't have an
// instance of the propeoerty type themselves.  Similar to Properties, but
// not to be inherited by a descendant are Attributes of a node.  These
// are more basic charateristics of the node, including the "Name" of the node
// as well as its "State" (if "inactive", the node is "off", and children are
// not traversed).  If requested, a node will also generate a UUID for the
// purpose of uniquely identifying a node.
//
// As a subclass of vtkKWESerializableObject, this object can easily be
// serialized via its Serialize method.
//
// .SECTION See Also
// vtkKWEObjectTreeTransformableNode vtkKWEObjectTreePropertyBase

#ifndef __vtkKWEObjectTreeNodeBase_h
#define __vtkKWEObjectTreeNodeBase_h

#include "vtkKWESerializableObject.h"
#include "vtkSmartPointer.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkInformation;
class vtkInformationIntegerKey;
class vtkInformationObjectBaseKey;
class vtkInformationStringKey;
class vtkKWEObjectTreePropertyBase;
class vtkKWEObjectTreeNodeBaseChildren;
class vtkKWEObjectTreeNodeIterator;
class vtkKWESerializer;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreeNodeBase : public vtkKWESerializableObject
{
public:
  static vtkKWEObjectTreeNodeBase* New();
  vtkTypeRevisionMacro(vtkKWEObjectTreeNodeBase, vtkKWESerializableObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the vtkObject for this node.
  virtual void SetNodeObject(vtkObject *object);
  vtkGetObjectMacro(NodeObject, vtkObject);

  // Description:
  // Returns number of children this node has.
  virtual unsigned int GetNumberOfChildren();

  // Description:
  // Add the child to this node and set this node as its parent.  The node
  // is added only if unique (not already a child), if the node doesn't have
  // a parent (which actually handles its uniqueness as a child), and if a
  // cycle won't be created by adding the child.
  // Returns index of the child (-1 if not added).
  virtual int AddChild(vtkKWEObjectTreeNodeBase *childNode);

  // Description:
  // Inserts this child at the specified index (0 based)in the child "list"
  // and sets this node as its parent.  The node is added only if unique (not
  // already a child), if the index is valid (will not insert at index 3 if
  // only 2 children in the list), if the node doesn't have a parent (which
  // actually handles its uniqueness as a child), and if a cycle won't be
  // created by adding the child.
  // Return 1 if the node is added (0 otherwise);
  virtual int InsertChild(unsigned int index, vtkKWEObjectTreeNodeBase *childNode);

  // Description:
  // Remove indicated child node. Return index of the child that was removed or
  // -1 if it wasn't a child.
  int RemoveChild(vtkKWEObjectTreeNodeBase *childNode);

  // Description:
  // Remove child at indicated index.  Return 1 if successful (0 otherwise).
  int RemoveChild(unsigned int index);

  // Description:
  // Get child at indicated index.
  vtkKWEObjectTreeNodeBase *GetChild(unsigned int index);

  // Description:
  // Get the parent of this node (a value of NULL indicates this node is the
  // root of the tree
  vtkGetObjectMacro(Parent, vtkKWEObjectTreeNodeBase);

  // Description:
  // Tests whether a node exists in the tree (returns true if it does).
  virtual bool IsDescendant(vtkKWEObjectTreeNodeBase *testNode);

  // Description:
  // Add the property to the node;  returns 0 if the property was already present
  // (does NOT replace the existing property)
  int AddProperty(vtkKWEObjectTreePropertyBase *nodeProperty);

  // Description:
  // Fill the passed vtkInformation object with ALL the properties of this node,
  // including those that this node inherits, IF CanInheritProperties()==true
  void GetAllProperties(vtkInformation *allProperties);

  // Description:
  // Retrieves the requested property, if it exists (otherwise returns NULL).  If
  // includeInheritance == true (and INHERIT_PROPERTIES is ON), will search
  // ancestors for inheritable property of specified type.  The
  // inheritedProperty flag it set to true if the property is inherited from
  // an ancestor.
  vtkKWEObjectTreePropertyBase* GetProperty(vtkInformationObjectBaseKey *propertyKey,
    bool &inheritedProperty, bool includeInheritance = false);

  // Description:
  // Removes the property with the given Key, if it exists.
  // Returns 1 if the property existed and was removed, 0 otherwise.
  int RemoveProperty(vtkInformationObjectBaseKey *propertyKey);

  // Description:
  // Removes the property if it exists.
  // Returns 1 if the property existed and was removed, 0 otherwise.
  int RemoveProperty(vtkKWEObjectTreePropertyBase *nodeProperty);

  // Description:
  // Removes all the node's properties.
  void RemoveAllProperties();

  // Description:
  // Return the number of properties set on this node.
  int GetNumberOfProperties();

  // Description:
  // Update modifed time for this object and also update the TreeModifiedTime
  // (for this object, and pushes up to parent as well)
  virtual void Modified();

  // Description:
  // Return this object's modified time, considering the properties of this
  // node.  This value is not affected by a child changing (but is by adding
  // or removing a child)
  virtual unsigned long GetMTime();

  // Description:
  // Return the modified time for this (sub)tree.
  vtkGetMacro(TreeModifiedTime, unsigned long);

  // Description:
  // Check to see if the specified node is the same (Attributes and Properties)
  // as this node.  If checkDescendants == true, then the children (and their
  // children) are compared as well (and must be in the same order).  If
  // canBeSuperset == true, then it is considered "equal" if all attrbiutes
  // and Properties that are set in the testNode match in "this" node (this
  // node can be a superset of testNode); the final flag
  // "considerInheritedProperties" will compare an inherited property as if
  // it were a property specifed on the node.
  virtual bool IsEqualTo(vtkKWEObjectTreeNodeBase *testNode,
    bool checkDescendants, bool canBeSuperset = false,
    bool considerInheritedProperties = false);

  // Description:
  // Reads the state of an instance from an archive OR
  // writes the state of an instance to an archive.
  virtual void Serialize(vtkKWESerializer*);

  // Description:
  // Set/Get the name of this node.
  const char *GetName();
  void SetName(const char *name);

  // Description:
  // Get the UUID for this node.
  const char *GetUUID();

  // Description:
  // Set the UUID for this node.
  void SetUUID(const char *uuid);

  // Description:
  // Creates a UUID for this node (if one doesn't already exist).  Return 0
  // if already exists, 1 if able to "generate" an uuid, and 2 if it was
  // necessary to "construct" (less unique) an uuid.
  int CreateUUID();

  // Description:
  // Clear/remove the UUID for this node. Calling CreateUUID after ClearUUID
  // will create a new/different UUID.
  void ClearUUID();

  //BTX
  // might expand to have a third state, where node is inactive, but the
  // subtree is not.
  enum NodeStates
    {
    ACTIVE_STATE,
    INACTIVE_STATE
    };
  //ETX
  void SetStateToActive()
    { this->SetState(ACTIVE_STATE); }
  void SetStateToInactive()
    { this->SetState(INACTIVE_STATE); }
  void SetState(int nodeState);
  int GetState();
  const char *GetStateAsString();

  // Description:
  // Set/Get whether or not this node can inherit properties from its ancestors
  void InheritPropertiesOn()
    { this->SetInheritProperties(true); }
  void InheritPropertiesOff()
    { this->SetInheritProperties(false); }
  bool CanInheritProperties()
    { return this->GetInheritProperties(); }
  bool GetInheritProperties();
  void SetInheritProperties(bool inheritState);

  static vtkInformationStringKey* NAME();
  static vtkInformationStringKey* UUID();
  static vtkInformationIntegerKey* STATE();
  static vtkInformationIntegerKey* INHERIT_PROPERTIES();


protected:
  vtkKWEObjectTreeNodeBase();
  virtual ~vtkKWEObjectTreeNodeBase();

  // Description:
  // The parent of this node.
  void SetParent(vtkKWEObjectTreeNodeBase *parent);

  // Description:
  // Adds the children of this node to the iterator according to how the
  // iterator is defined/setup.
  friend class vtkKWEObjectTreeNodeIterator;
  void AddChildren(vtkKWEObjectTreeNodeIterator *iterator);

  // Description:
  // Update the TreeModifiedTime for this node (and pushes the time up
  // to its parent as well)
  friend class vtkKWEObjectTreePropertyBase;
  virtual void UpdateTreeModifiedTime(unsigned long treeTime);

  // Description:
  // Add inheritable properties that don't already exist in allProperties.
  // Should only be called by a child of "this" object and the resulting list
  // is only accurate for the original caller of GetAllProperties()
  void AddInheritedProperties(vtkInformation *allProperties);

  // Description:
  // Returns the number of entries in the vtkInformation object.  This really
  // should go in with the vtkInformation code in VTK, but here for now
  int GetNumberOfInformationEntries(vtkInformation *infoObject);

  // Description:
  // The children of this node. PIMPL
  vtkKWEObjectTreeNodeBaseChildren *Children;

  // Description:
  // Parent of the node.  If NULL, then this node is the root of the tree.
  vtkKWEObjectTreeNodeBase *Parent;

  // Description:
  // Properties that make this node "special".  They could be properties
  // controlling visualization or maybe an application specific property.
  vtkInformation *Properties;

  // Description:
  // "Attributes" of this node that are not inherited by the children
  vtkInformation *Attributes;

  // Description:
  // Cached value for the tree with this node as root
  unsigned long TreeModifiedTime;

  // Description:
  // The vtkObject contained within this node
  vtkObject *NodeObject;

  // Description:
  // Serialize the Object member.  I've separated this out (from Serialize) and
  // made it virtual since subclasses may want to serialize this differently based
  // on the type of the Object that the Node will hold.
  virtual void SerializeObject(vtkKWESerializer*);

private:
  vtkKWEObjectTreeNodeBase(const vtkKWEObjectTreeNodeBase&); // Not implemented.
  void operator=(const vtkKWEObjectTreeNodeBase&); // Not implemented.

  bool RemoveAllPropertiesInternal();
};

#endif
