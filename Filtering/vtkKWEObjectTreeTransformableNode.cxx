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
#include "vtkKWEObjectTreeTransformableNode.h"

#include "vtkKWESerializer.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"

#include <vtkstd/set>

vtkCxxRevisionMacro(vtkKWEObjectTreeTransformableNode, "$Revision: 1774 $");
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
