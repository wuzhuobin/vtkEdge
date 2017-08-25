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

#include "vtkKWEObjectTreeNodeIterator.h"

#include "vtkKWEObjectTreeNodeBase.h"
#include "vtkObjectFactory.h"
#include "vtkWeakPointer.h"

#include <vtkstd/stack>
#include <vtkstd/queue>

vtkCxxRevisionMacro(vtkKWEObjectTreeNodeIterator, "$Revision: 686 $");
vtkStandardNewMacro(vtkKWEObjectTreeNodeIterator);

vtkCxxSetObjectMacro(vtkKWEObjectTreeNodeIterator, BaseNode, vtkKWEObjectTreeNodeBase);
vtkCxxSetObjectMacro(vtkKWEObjectTreeNodeIterator, PatternNode, vtkKWEObjectTreeNodeBase);

//-----------------------------------------------------------------------------
class vtkKWEObjectTreeNodeIteratorInternals
{
public:
  vtkKWEObjectTreeNodeIteratorInternals()
    {
    this->TraversalMode = vtkKWEObjectTreeNodeIterator::DEPTH_FIRST;
    }

  // Move to next node and remove it from the appropriate container
  void NextNode(vtkKWEObjectTreeNodeBase*& node, int& depth);

  // Add a node (and its depth) to the appropriate container
  void AddNode(vtkKWEObjectTreeNodeBase* node, int depth);

  // Add a vector of node to the appropriate container
  void AddNodes(vtkstd::vector<vtkKWEObjectTreeNodeBase*> &nodes, int depth);

  // Depth-first (use Stack) versus breadth-first (use Queue)
  void SetMode(int mode)
    {
    this->TraversalMode = mode;
    }

  // Is the relevant container empty?
  bool IsEmpty();

  // Empty both containers
  void Clear();

private:
  int TraversalMode;

  typedef struct
    {
    vtkWeakPointer<vtkKWEObjectTreeNodeBase> Node;
    int Depth;  // 0 = base, 1 = children, 2 = grandchildren, etc.  
    } NodeInfo;

  vtkstd::stack< NodeInfo > Stack;
  vtkstd::queue< NodeInfo > Queue;
};

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIteratorInternals::NextNode(vtkKWEObjectTreeNodeBase*& node,
                                                     int& depth)
{
  if (this->TraversalMode == vtkKWEObjectTreeNodeIterator::DEPTH_FIRST)
    {
    if (this->Stack.empty())
      {
      node = 0;
      depth = -1;
      return;
      }
    node = this->Stack.top().Node;
    depth = this->Stack.top().Depth;
    this->Stack.pop();
    }
  else // breadth-first
    {
    if (this->Queue.empty())
      {
      node = 0;
      depth = -1;
      return;
      }
    node = this->Queue.front().Node;
    depth = this->Queue.front().Depth;
    this->Queue.pop();
    }
  }

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIteratorInternals::AddNodes(
  vtkstd::vector<vtkKWEObjectTreeNodeBase*> &nodes, int depth)
{
  if (this->TraversalMode == vtkKWEObjectTreeNodeIterator::DEPTH_FIRST)
    {
    // (reverse) iterate because we're adding these to a Stack, and want to pull
    // the 1st node from the iterator 1st (before the other nodes)
    vtkstd::vector<vtkKWEObjectTreeNodeBase*>::reverse_iterator nodeIter;
    for (nodeIter = nodes.rbegin(); nodeIter != nodes.rend(); nodeIter++)
      {
      this->AddNode(*nodeIter, depth);
      }
    }
  else // breadth first, so we can add them in the order they are in the list
    {

    vtkstd::vector<vtkKWEObjectTreeNodeBase*>::const_iterator nodeIter;
    for (nodeIter = nodes.begin(); nodeIter != nodes.end(); nodeIter++)
      {
      this->AddNode(*nodeIter, depth);
      }
    }
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIteratorInternals::AddNode(vtkKWEObjectTreeNodeBase* node,
                                                    int depth)
{
  NodeInfo tmpNodeInfo;
  tmpNodeInfo.Node = node;
  tmpNodeInfo.Depth = depth;

  if (this->TraversalMode == vtkKWEObjectTreeNodeIterator::DEPTH_FIRST)
    {
    this->Stack.push( tmpNodeInfo );
    }
  else
    {
    this->Queue.push( tmpNodeInfo );
    }
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeIteratorInternals::IsEmpty()
{
  if (this->TraversalMode == vtkKWEObjectTreeNodeIterator::DEPTH_FIRST)
    {
    return this->Stack.empty();
    }

  // if not depth-first, assume breadth-first
  return this->Queue.empty();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIteratorInternals::Clear()
{
  while (!this->Stack.empty())
    {
    this->Stack.pop();
    }
  while (!this->Queue.empty())
    {
    this->Queue.pop();
    }
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreeNodeIterator::vtkKWEObjectTreeNodeIterator()
{
  this->Internals = new vtkKWEObjectTreeNodeIteratorInternals;
  this->BaseNode = 0;
  this->PatternNode = 0;
  this->CurrentNode = 0;
  this->CurrentDepth = 0;
  this->InitTraversalTime = 0;
  this->TraversalMode = DEPTH_FIRST;
  this->ConsiderInheritedProperties = false;
  // default to adding only the children of the BaseNode
  this->IncludeBaseNode = false;
  this->MaximumTraversalDepth = 1;
}

//-----------------------------------------------------------------------------
vtkKWEObjectTreeNodeIterator::~vtkKWEObjectTreeNodeIterator()
{
  this->SetBaseNode(0);
  this->SetPatternNode(0);
  this->CurrentNode = 0;
  delete this->Internals;
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIterator::GoToFirstNode()
{
  // reset before starting traversal
  this->Internals->Clear();
  this->Internals->SetMode( this->TraversalMode );
  this->CurrentNode = 0;

  if (!this->BaseNode)
    {
    vtkErrorMacro("Unable to init traversal; BaseNode must be set!");
    return;
    }

  this->InitTraversalTime = this->GetMTime();
  if (this->IncludeBaseNode)
    {
    this->Internals->AddNode(this->BaseNode, 0);
    }
  else if (this->MaximumTraversalDepth > 0)
    {
    this->CurrentDepth = 0;
    this->BaseNode->AddChildren( this );
    }
  this->GoToNextNode();
}

//-----------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIterator::GoToNextNode()
{
  this->CurrentNode = 0;

  // if the iterator has changed since starting traversal, clear the CurrentNode
  // I'm allowing the Pattern to change during traversal, thus NOT checking
  // (this->PatternNode && this->InitTraversalTime < this->PatternNode->GetMTime()))
  if (this->InitTraversalTime < this->GetMTime())
    {
    this->Internals->Clear();
    return;
    }
  
  vtkKWEObjectTreeNodeBase *candidateNode;
  while (!this->CurrentNode)
    {
    // If Container is empty... we're done (and CurrentNode will equal 0)
    if (this->Internals->IsEmpty())
      {
      return;
      }

    // grab the node at the front of the container and pop it off the container
    this->Internals->NextNode(candidateNode, this->CurrentDepth);
    // If the candidate node exists and we are traversing beyond its depth,
    // add its children
    if (candidateNode && this->CurrentDepth < this->MaximumTraversalDepth)
      {
      candidateNode->AddChildren( this );
      }
    // node has to exist (non-NULL) and match the PatternNode (if set) to be
    // the CurrentNode; othersie, "throw it away" and keep looking
    if (candidateNode && 
      (!this->PatternNode || candidateNode->IsEqualTo(this->PatternNode, 
                             false, true, this->ConsiderInheritedProperties)))
      {
      this->CurrentNode = candidateNode;
      }
    }
}

//-----------------------------------------------------------------------------
bool vtkKWEObjectTreeNodeIterator::IsDoneWithTraversal()
{  
  return this->CurrentNode ? false : true;
}

// ---------------------------------------------------------------------------
vtkKWEObjectTreeNodeBase *vtkKWEObjectTreeNodeIterator::GetCurrentNode()
{
  return this->CurrentNode;
}

// ---------------------------------------------------------------------------
// Description:
// Return the traversal mode as a descriptive character string.
const char *vtkKWEObjectTreeNodeIterator::GetTraversalModeAsString()
{
  switch (this->TraversalMode)
    {
    case DEPTH_FIRST:
      return "Depth-first";
    case BREADTH_FIRST:
      return "Breadth-first";
    }

  return "Unrecognized";
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIterator::AddChildren(vtkstd::vector<vtkKWEObjectTreeNodeBase*> &children)
{
  this->Internals->AddNodes(children, this->CurrentDepth + 1);
}

// ---------------------------------------------------------------------------
void vtkKWEObjectTreeNodeIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "TraversalMode: " << this->GetTraversalModeAsString() << "\n";
  os << indent << "IncludeBaseNode: " << (this->IncludeBaseNode ? "On\n" : "Off\n");
  os << indent << "ConsiderInheritedProperties: " << 
    (this->ConsiderInheritedProperties ? "On\n" : "Off\n");
  os << indent << "MaximumTraversalDepth: " << this->MaximumTraversalDepth << "\n";
}
