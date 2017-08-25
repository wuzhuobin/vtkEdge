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
#include "vtkKWEObjectTreeTransformableNode.h"

#include "vtkKWESerializer.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"

#include <vtkstd/set>

vtkCxxRevisionMacro(vtkKWEObjectTreeTransformableNode, "$Revision: 702 $");
vtkStandardNewMacro(vtkKWEObjectTreeTransformableNode);

vtkCxxSetObjectMacro(vtkKWEObjectTreeTransformableNode, Transform, vtkTransform);

class vtkKWEObjectTreeTransformableNodeReferencingNodes : public vtkstd::set< vtkKWEObjectTreeTransformableNode* >
{
};

//-----------------------------------------------------------------------------
vtkKWEObjectTreeTransformableNode::vtkKWEObjectTreeTransformableNode()
{
  this->Transform = 0;
  this->ReferencingNodes = new vtkKWEObjectTreeTransformableNodeReferencingNodes;
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreeTransformableNode::~vtkKWEObjectTreeTransformableNode()
{
  this->SetTransform(0);
  delete this->ReferencingNodes;
}

//-----------------------------------------------------------------------------
unsigned long vtkKWEObjectTreeTransformableNode::GetMTime()
{  
  unsigned long mTime = this->Superclass::GetMTime();
  if (this->Transform && this->Transform->GetMTime() > mTime)
    {
    mTime = this->Transform->GetMTime();
    }

  return mTime;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeTransformableNode::UpdateTreeModifiedTime(unsigned long treeTime)
{
  if (this->TreeModifiedTime == treeTime)
    {
    return;
    }

  this->Superclass::UpdateTreeModifiedTime(treeTime);

  // iterate through referencing nodes, pushing the treeTime up their tree 
  vtkKWEObjectTreeTransformableNodeReferencingNodes::const_iterator refIterator;
  for (refIterator = this->ReferencingNodes->begin(); 
    refIterator != this->ReferencingNodes->end(); refIterator++)
    {
    (*refIterator)->UpdateTreeModifiedTime(treeTime);
    }
}

// ---------------------------------------------------------------------------
bool vtkKWEObjectTreeTransformableNode::IsEqualTo(vtkKWEObjectTreeNodeBase *testNode,
                                                  bool checkDescendants, 
                                                  bool canBeSuperset/*=false*/,
                                                  bool considerInheritedProperties/*=false*/)
{
  if (!this->Superclass::IsEqualTo(testNode, checkDescendants, 
    canBeSuperset, considerInheritedProperties))
    {
    return false;
    }

  vtkKWEObjectTreeTransformableNode *transformableTestNode = 
    vtkKWEObjectTreeTransformableNode::SafeDownCast(testNode);
  // if the testNode is NOT a vtkKWEObjectTreeTransformableNode, then only should
  // have gotten here if canBeSuperset == true.  
  if (!transformableTestNode)
    {
    if (!canBeSuperset)
      {
      vtkErrorMacro("Unexpected node type - must be logic error in base class IsEqualTo!");
      return false;
      }
    // so, canBeSuperset==true as expected... and it is just that, a superset
    return true;
    }

  // if canBeSuperset==true and there is no transform on the testNode, then
  // regardless of whether we have a transform or not we pass this equality test;
  // Also test possiblity that transforms share same ptr (including NULL)
  if ((canBeSuperset && !transformableTestNode->Transform) ||
    transformableTestNode->Transform == this->Transform)
    {
    return true;
    }

  if (this->Transform && transformableTestNode->Transform)
    {
    // compare the transforms value by value
    for (int i = 0; i < 4; i++)
      {
      for (int j = 0; j < 4; j++)
        {
        if (this->Transform->GetMatrix()->GetElement(i,j) !=
          transformableTestNode->Transform->GetMatrix()->GetElement(i,j))
          {
          return false;
          }
        }
      }
    }
  else // one is Null, other is not and we're not in "canBeSuperset" situation
    {
    return false;
    }

  return true;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeTransformableNode::Serialize(vtkKWESerializer* ser)
{
  this->Superclass::Serialize(ser);

  if (ser->IsWriting())
    {
    if (this->Transform)
      {
      vtkObject *transform = this->Transform;
      ser->Serialize("Transform", transform);
      }
    }
  if (!ser->IsWriting())
    {
    if (this->Transform)
      {
      this->Transform->UnRegister( this );
      this->Transform = 0;
      }
    // the object is registered during serialization
    vtkObject *transform = 0;
    ser->Serialize("Transform", transform);
    this->Transform = vtkTransform::SafeDownCast(transform); 
    }
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeTransformableNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Transform: ";
  if (this->Transform)
    {
    os << this->Transform << "\n";
    }
  else
    {
    os << "(none)\n";
    }
}
