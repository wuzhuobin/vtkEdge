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
// .NAME vtkKWEObjectTreePropertyBase - Base class for ObjectTree Properties
// .SECTION Description
// vtkKWEObjectTreePropertyBase is the base class for Properties to be added
// to ObjectTree nodes, providing a container for the Properties "Attributes",
// a container for nodes that reference this object (so the nodes can be 
// nodified when the property changes) and a general GetKey() method to 
// retrieve a concrete subclass's  KEY().  The Modified() method updates the
// TreeMTime of all referencing nodes and thus should be called by subclasses
// whenever changes are made to values.  
//
// As a subclass of vtkKWESerializableObject, the Attributes are 
// easily serialized.  Furthermore, the subclass does not need to worry 
// about serialization.
//
// Properties of a node can be inherited by decendants unless the Property
// is set to NOT be inheritable (IsInheritableOff()).
//
// .SECTION See Also

#ifndef __vtkKWEObjectTreePropertyBase_h
#define __vtkKWEObjectTreePropertyBase_h

#include "vtkKWESerializableObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkInformation;
class vtkInformationIntegerKey;
class vtkInformationObjectBaseKey;
class vtkKWEObjectTreeNodeBase;
class vtkKWEObjectTreePropertyBaseReferencingNodes;
class vtkKWESerializer;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreePropertyBase : public vtkKWESerializableObject
{
public:
  vtkTypeRevisionMacro(vtkKWEObjectTreePropertyBase, vtkKWESerializableObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Reads the state of an instance from an archive OR
  // writes the state of an instance to an archive. 
  virtual void Serialize(vtkKWESerializer* ser);

  // Description:
  // When we get modified, update the TreeModifiedTime of nodes that
  // reference us.  Note, subclasses needed to call this after
  // making changes to the Attributes object.
  virtual void Modified();

  // Description:
  // Retrieve the KEY from a concrete subclass 
  virtual vtkInformationObjectBaseKey *GetKey() = 0;

  // Description:
  // Is "this" the same as testProperty (same Attributes)?  If canBeSuperset 
  // == true, then can be considered equal if the only difference is that
  // "this" has additional attributes.
  virtual bool IsEqualTo(vtkKWEObjectTreePropertyBase *testProperty,
    bool canBeSuperset = false);

  // Description:
  // Return the number of attributes that have been set for this Property.
  int GetNumberOfAttributes();

  // Description:
  // Set/Get whether this Property is inherited by it's descendants.  
  // By default it is set to be "true".  Also, if unset, it will return
  // true.  However, if two properties are otherwise equal, they will
  // NOT be considered to be equal if one Property (A) has the value set
  // to true while the other Property (B) doesn't have the value set... unless,
  // A->IsEqualTo(B, true)... A can be a superset of B.  This is actually why
  // unsetting of the value is allowed, so that the value can be unset in a
  // PatternNode given to the iterator: the iteration result will then not 
  // depend on this value.
  static vtkInformationIntegerKey* IS_INHERITABLE();
  void IsInheritableOn() 
    { this->SetIsInheritable(true); }
  void IsInheritableOff() 
    { this->SetIsInheritable(false); }
  bool IsInheritable()
    { return this->GetIsInheritable(); }
  bool GetIsInheritable();
  void SetIsInheritable(bool isInheritable);

  // Description:
  // Unset the IS_INHERITABLE() key.  Note, GetIsInheritable() will return true
  // if it is Unset.  If the key is Unset, a PatternNode of an iterator can be
  // used to iterate over nodes comparing/filtering based on a property without
  // the result depending on whether the Property IsHeritable or not.
  void UnsetIsInheritable();

protected:
  vtkKWEObjectTreePropertyBase();
  virtual ~vtkKWEObjectTreePropertyBase();

  // Description:
  // The main object that object attributes("properties") are stored in
  vtkInformation *Attributes;

  // Description:
  // Add/Remove from our set of nodes that use the Property object.
  friend class vtkKWEObjectTreeNodeBase;
  void AddReferencingNode(vtkKWEObjectTreeNodeBase *node);
  void RemoveReferencingNode(vtkKWEObjectTreeNodeBase *node);
  vtkKWEObjectTreePropertyBaseReferencingNodes *ReferencingNodes;

private:
  vtkKWEObjectTreePropertyBase(const vtkKWEObjectTreePropertyBase&); // Not implemented.
  void operator=(const vtkKWEObjectTreePropertyBase&); // Not implemented.
};

#endif
