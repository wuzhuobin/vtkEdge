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
// .NAME vtkKWEObjectTreeNodeIterator - Iterator for nodes in an ObjectTree
// .SECTION Description
// vtkKWEObjectTreeNodeIterator is an iterator for nodes in an ObjectTree.  The
// iterator can be set to depth-first or breadth-first, to include the BaseNode
// or not, and to only consider nodes up to a maximum dpeth from the BaseNode.
// Additionally, a PatternNode can be sepcified which will act as a filter
// controlling which nodes in the tree will be iterated over.  Matching nodes 
// can be a superset of the PatternNode, but whatever Attributes and Properties
// (as well as node type and the type of the node in the object) are specified 
// on the PatternNode must exist and have matching values in the nodes be 
// considered for iteration.
//
// To use this iterator, SetBaseNode() and then either InitTraversal() or 
// GoToFirstNode().  GetCurrentNode() can then be called to retrieve the 
// top/current node in the iterator.  Repeated calls to GoToNextNode followed by
// GetCurrentNode can be made to move through the values being iterated over 
// (and GetCurrentDepth can be called to get the depth of the current node
// realtive to the BaseNode; 0 - BaseNode, 1 - children of BaseNode, etc)
// When the value of GetCurrentNode is NULL, iteration is complete.  Note that
// values pushed onto the internal stack of the iterator are vtkWeakPointers,
// and thus COULD become NULL during iteration.  However, GoToNextNode() will
// skip over NULL values.  CurrentNode is a vtkSmartPointer, and thus will not
// become invalid unless done so by the iterator (setting it to NULL).  
//
// A simple example:
// \code
// vtkSmartPointer<vtkKWEObjectTreeNodeIterator> iterator =
//   vtkSmartPointer<vtkKWEObjectTreeNodeIterator>::New();
// iterator->SetBaseNode( root );
// iterator->SetTraversalToEntireSubtree();
// iterator->SetTraversalModeToDepthFirst();
//
// // only interested in nodes that have a "user" property, though we don't
// // care about the contents of the user property.
// vtkSmartPointer<vtkKWEObjectTreeTransformableNode> patternNode =
//   vtkSmartPointer<vtkKWEObjectTreeTransformableNode>::New();
// vtkSmartPointer<vtkKWEObjectTreeUserProperty> userProp = 
//   vtkSmartPointer<vtkKWEObjectTreeUserProperty>::New();
// patternNode->AddProperty(userProp);
// iterator->SetPatternNode( patternNode );
// for (iterator->InitTraversal(); 
//   (currentNode = iterator->GetCurrentNode()); iterator->GoToNextNode())
//   {
//   // do something
//   }      
// \endcode
//
// NOTE: A 2nd call to InitTraversal() or GoToFirstNode() clears the 
// existing contents and starts iteration again based on the current tree 
// structure.  Thus, the 2nd (or subsequent) iterations may not give the same
// results (if the ObjectTree has changed).
//
// Also, changes made to the iterater (changing the value of a member variable)
// result in clearing the iteration.  InitiTraversal or GoToFirstNode must be
// called to start the iteration over.  Note, however, that the PatternNode
// CAN be changed without requireing restarting the iteration (though nodes
// already passed-over in the tree because they didn't match the PatternNode
// but which might now match, will not be reconsidered)
//
// .SECTION See Also

#ifndef __vtkKWEObjectTreeNodeIterator_h
#define __vtkKWEObjectTreeNodeIterator_h

#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include <vtkstd/vector>

class vtkKWEObjectTreeNodeBase;
class vtkKWEObjectTreeNodeIteratorInternals;

class VTKEdge_FILTERING_EXPORT vtkKWEObjectTreeNodeIterator : public vtkObject
{
public:
  static vtkKWEObjectTreeNodeIterator* New();
  vtkTypeRevisionMacro(vtkKWEObjectTreeNodeIterator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the BaseNode from which to fill the iterator
  void SetBaseNode(vtkKWEObjectTreeNodeBase* baseNode);
  vtkGetObjectMacro(BaseNode, vtkKWEObjectTreeNodeBase);

  // Description:
  // Set/Get the PatternNode; if set, only nodes matching the PatternNode
  // will be iterated over.  Note: nodes can be a superset of this node but must
  // match Atttributes, Properties, and Object type (actual object not compared)
  // set on the Pattern node
  void SetPatternNode(vtkKWEObjectTreeNodeBase* patternNode);
  vtkGetObjectMacro(PatternNode, vtkKWEObjectTreeNodeBase);

  // Description:
  // Clear stack/iterator and then initialize the stack with the children of
  // the BaseNode, setting CurrentNode to be the 1st of these children.
  // Same as GoToFirstNode.
  void InitTraversal() { this->GoToFirstNode(); };

  // Description:
  // Clear stack/iterator and then initialize the stack with the children of
  // the BaseNode, setting CurrentNode to be the 1st of these children.
  // Same as InitTraversal.
  void GoToFirstNode();

  // Description:
  // Sets the value of CurrentNode to be the next non-zero value in the
  // iterator.  If empty or only NULL values, the CurrentNode is set to NULL.
  void GoToNextNode();

  // Description:
  // Returns pointer to the current node in the iteration.  To iterate
  // through the nodes: InitTraversal(), GetCurrentNode(), GoToNextNode(),
  // GetCurrentNode(), GoToNextNode(), etc. until GetCurrentNode returns a NULL
  // pointer.  CurrentNode is a SmartPointer and thus is protected from
  // destruction elsewhere in the application.
  vtkKWEObjectTreeNodeBase *GetCurrentNode();

  // Description:
  // Get the depth (relative to the BaseNode) of the current node .  0 is the
  // BaseNode level, 1 would be children of the base node, etc.  The value is
  // only valid if CurrentNode is non-NULL;
  vtkGetMacro(CurrentDepth, int);

  // Description:
  // Test whether the iterator is currently positioned at a valid item.
  bool IsDoneWithTraversal();

  // Description:
  // Set/Get whether the BaseNode is included (considered) during traversal.
  vtkBooleanMacro(IncludeBaseNode, bool);
  vtkSetMacro(IncludeBaseNode, bool);
  vtkGetMacro(IncludeBaseNode, bool);

  // Description:
  // Set/Get the maximum depth for traversal
  vtkSetClampMacro(MaximumTraversalDepth, int, 0, VTK_LARGE_INTEGER);
  vtkGetMacro(MaximumTraversalDepth, int);

  // Description:
  // Convenience method for setting iterator to only consider the children
  // of the BaseNode (BaseNode NOT traversed).
  void SetTraversalToChildrenOnly()
    {
    this->SetMaximumTraversalDepth(1);
    this->IncludeBaseNodeOff();
    }

  // Description:
  // Convenience method for setting iterator to consider the entier subtree
  // of the BaseNode (BaseNode is included)
  void SetTraversalToEntireSubtree()
    {
    this->SetMaximumTraversalDepth(VTK_LARGE_INTEGER);
    this->IncludeBaseNodeOn();
    }

  enum TraversalModes
    {
    DEPTH_FIRST = 0,
    BREADTH_FIRST
    };

  // Description:
  // Set/Get whether the tree is iterated over depth-first or breadth-first.
  void SetTraversalModeToDepthFirst()
    { this->SetTraversalMode( DEPTH_FIRST ); }
  void SetTraversalModeToBreadthFirst()
    { this->SetTraversalMode( BREADTH_FIRST ); }
  vtkSetClampMacro(TraversalMode, int, DEPTH_FIRST, BREADTH_FIRST);
  vtkGetMacro(TraversalMode, int);
  const char *GetTraversalModeAsString();

  // Description:
  // Set/Get whether inherited properties are considered when 
  // comparing to a PatternNode.  By default this is off (only properties
  // actually set on the node are compared)
  vtkBooleanMacro(ConsiderInheritedProperties, bool);
  vtkSetMacro(ConsiderInheritedProperties, bool);
  vtkGetMacro(ConsiderInheritedProperties, bool);

protected:
  vtkKWEObjectTreeNodeIterator();
  virtual ~vtkKWEObjectTreeNodeIterator();

  vtkKWEObjectTreeNodeBase *BaseNode;
  vtkKWEObjectTreeNodeBase *PatternNode;

  vtkSmartPointer<vtkKWEObjectTreeNodeBase> CurrentNode;

  vtkKWEObjectTreeNodeIteratorInternals *Internals;

  // Description:
  // Is the BaseNode included in the traversal?
  bool IncludeBaseNode;

  // Description:
  // Time at which we started traversal
  unsigned long InitTraversalTime;

  friend class vtkKWEObjectTreeNodeBase;
  void AddChildren(vtkstd::vector<vtkKWEObjectTreeNodeBase*> &children);

  // Description:
  // Depth (relative to BaseNode) of the CurrentNode
  int CurrentDepth;

  // Description:
  // The maximum depth (relative to BaseNode) of traversal
  int MaximumTraversalDepth;

  // Description:
  // Depth-first versus breadth-first
  int TraversalMode;

  // Description:
  // During comparison the a PatternNode can the node inherit the property
  // to achieve a match
  bool ConsiderInheritedProperties;

  // internal method for clearing the stack
  void Clear();

private:
  vtkKWEObjectTreeNodeIterator(const vtkKWEObjectTreeNodeIterator&); // Not implemented.
  void operator=(const vtkKWEObjectTreeNodeIterator&); // Not implemented.
};

#endif


