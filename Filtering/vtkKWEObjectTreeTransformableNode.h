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
// .NAME vtkKWEObjectTreeTransformableNode - Transformable nodes that can be referenced
// .SECTION Description
// vtkKWEObjectTreeTransformableNode adds a vtkTransform object to 
// vtkKWEObjectTreeNodeBase and also is the base class of nodes that can be
// "referenced" by other nodes.
// .SECTION See Also

#ifndef __vtkKWEObjectTreeTransformableNode_h
#define __vtkKWEObjectTreeTransformableNode_h

#include "vtkKWEObjectTreeNodeBase.h"
#include "vtkSmartPointer.h"
#include "VTKEdgeConfigure.h" // include configuration header


class vtkInformation;
class vtkTransform;
class vtkKWEObjectTreeTransformableNodeReferencingNodes;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreeTransformableNode : public vtkKWEObjectTreeNodeBase
{
public:
  static vtkKWEObjectTreeTransformableNode* New();
  vtkTypeRevisionMacro(vtkKWEObjectTreeTransformableNode, vtkKWEObjectTreeNodeBase);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get this objects transform
  void SetTransform(vtkTransform *transform);
  vtkGetObjectMacro(Transform, vtkTransform);

  // Description:
  // Update the TreeModifiedTime for this node (and pushes the time up
  // to any nodes referencing it as well)
  virtual void UpdateTreeModifiedTime(unsigned long treeTime);

  // Description:
  // Return this object's modified time, considering the transforms MTime
  virtual unsigned long GetMTime();

  // Description:
  // Adds test of the this node's transform to the equality test performed
  // by the base class.
  virtual bool IsEqualTo(vtkKWEObjectTreeNodeBase *testNode, 
    bool checkDescendants, bool canBeSuperset = false,
    bool considerInheritedProperties = false);

  // Description:
  // Reads the state of an instance from an archive OR
  // writes the state of an instance to an archive. 
  virtual void Serialize(vtkKWESerializer*);

  // Description:
  // Keep track of referencing nodes (vtkKWEObjectTreeReferenceNode) so we can
  // notify them when we have changed
//  void AddReferencingNode( vtkKWEObjectTreeTransformableNode *referencingNode ) {};
  //void RemoveReferencingNode( vtkKWEObjectTreeTransformableNode *referencingNode ) {};

protected:
  vtkKWEObjectTreeTransformableNode();
  virtual ~vtkKWEObjectTreeTransformableNode();

  // Description:
  // This node's transform.  
  vtkTransform *Transform;

  // Description:
  // Keep track of referencing nodes (vtkKWEObjectTreeReferenceNode) so we can
  // notify them when we have changed.  PIMPL
  vtkKWEObjectTreeTransformableNodeReferencingNodes *ReferencingNodes;

private:
  vtkKWEObjectTreeTransformableNode(const vtkKWEObjectTreeTransformableNode&); // Not implemented.
  void operator=(const vtkKWEObjectTreeTransformableNode&); // Not implemented.
};

#endif
