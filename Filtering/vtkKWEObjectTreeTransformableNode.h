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
