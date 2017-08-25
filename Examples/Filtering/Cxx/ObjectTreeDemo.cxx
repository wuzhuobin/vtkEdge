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

#include "vtkCamera.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleKey.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationIdTypeKey.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkInformationIterator.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationStringKey.h"
#include "vtkKWEUUID.h"
#include "vtkKWEInformationKeyMap.h"
#include "vtkKWEObjectTreeColorProperty.h"
#include "vtkKWEObjectTreeTransformableNode.h"
#include "vtkKWEObjectTreeNodeIterator.h"
#include "vtkKWEObjectTreeUserProperty.h"
#include "vtkKWESerializationHelperMap.h"
#include "vtkKWEXMLArchiveWriter.h"
#include "vtkKWEXMLArchiveReader.h"
#include "vtkKWEXMLElement.h"
#include "vtkKWEFilteringInstantiator.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkTransform.h"
#include "vtkCommonInstantiator.h"
#include "vtkRenderingInstantiator.h"
#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/ios/sstream>


vtkInformationDoubleKey *testUserDoubleKey;
vtkInformationIdTypeKey *testUserIdTypeKey;
vtkInformationIntegerKey *testUserIntKey;
vtkInformationStringKey *testUserStringKey;
vtkInformationIntegerVectorKey *testUserIntVectorKey;
vtkInformationDoubleVectorKey *testUserDoubleVectorKey;

// global so we can easily add it to the objects we write out
vtkSmartPointer<vtkCamera> camera;

#define PASS 0
#define FAIL 1

int BuildOriginalTree(vtkKWEObjectTreeNodeBase *root)
{
  int buildResult = PASS;

  root->SetName("RootNode");

  vtkKWEObjectTreeTransformableNode *(child[10]);
  char name[20];

  // add a vtkIntArray as the node's NodeObject
  vtkSmartPointer<vtkIntArray> intArray = vtkSmartPointer<vtkIntArray>::New();
  intArray->SetNumberOfComponents(1);
  intArray->SetNumberOfTuples(1);
  intArray->SetTuple1(0, 10);
  vtkInformation *arrayInfo = intArray->GetInformation();
  // using a key we're already registering...
  arrayInfo->Set( vtkKWEObjectTreeNodeBase::NAME(), "My Int Array");
  root->SetNodeObject( intArray );

  // 5 children of the root node
  for (int i = 0; i < 5; i++)
    {
    child[i] = vtkKWEObjectTreeTransformableNode::New();
    sprintf(name, "Node %d (child)", i);
    child[i]->SetName(name);
    root->AddChild(child[i]);
    child[i]->Delete();
    }

  // add a vtkDoubleArray as one of the children's NodeObject
  vtkKWEObjectTreeNodeBase *tmpPtr = root->GetChild(1);
  vtkSmartPointer<vtkDoubleArray> doubleArray = vtkSmartPointer<vtkDoubleArray>::New();
  doubleArray->SetNumberOfComponents(3);
  doubleArray->SetNumberOfTuples(1);
  doubleArray->SetTuple3(0, 5.5, 4.4, 3.3);
  tmpPtr->SetNodeObject( doubleArray );

  // add a vtkIdTypeArray as one of the children's NodeObject
  tmpPtr = root->GetChild(2);
  vtkSmartPointer<vtkIdTypeArray> idTypeArray = vtkSmartPointer<vtkIdTypeArray>::New();
  idTypeArray->SetNumberOfComponents(3);
  idTypeArray->SetNumberOfTuples(2);
  idTypeArray->SetTuple3(0, 5, 4, 3);
  idTypeArray->SetTuple3(1, 2, 1, 0);
  tmpPtr->SetNodeObject( idTypeArray );

  // add "grandchildren" to one of the nodes
  child[5] = vtkKWEObjectTreeTransformableNode::New();
  child[5]->SetName("Node 5 (grandchild)");
  vtkSmartPointer<vtkTransform> tmpTransform2 = vtkSmartPointer<vtkTransform>::New();
  tmpTransform2->Translate(5.0, 0, 0);
  child[5]->SetTransform(tmpTransform2);
  tmpPtr->AddChild(child[5]);
  child[5]->Delete();

  // actually add a transform
  child[6] = vtkKWEObjectTreeTransformableNode::New();
  child[6]->SetName("Node 6 (grandchild)");
  vtkSmartPointer<vtkTransform> tmpTransform = vtkSmartPointer<vtkTransform>::New();
  child[6]->SetTransform(tmpTransform);
  camera = vtkSmartPointer<vtkCamera>::New();
  camera->Print(cout);
  child[6]->SetNodeObject( camera );
  tmpPtr->InsertChild(0, child[6]);
  child[6]->Delete();

  // add a node of the base node type, for iteration example in TestIteration
  vtkKWEObjectTreeNodeBase *baseChild = vtkKWEObjectTreeNodeBase::New();
  baseChild->SetName("Node 7 (grandchild)");
  tmpPtr->InsertChild(1, baseChild);
  baseChild->Delete();

  // and finally, one great grandchild (that doesn't inherit proeprties)
  child[8] = vtkKWEObjectTreeTransformableNode::New();
  child[8]->SetName("Node 8 (great-grandchild)");
  child[8]->InheritPropertiesOff();
  baseChild->AddChild(child[8]);
  child[8]->Delete();

  // now remove some children
  root->RemoveChild(child[0]);
  root->RemoveChild(3);

  // and finally, start adding some properties
  vtkSmartPointer<vtkKWEObjectTreeColorProperty> colorProp =
    vtkSmartPointer<vtkKWEObjectTreeColorProperty>::New();
  double tmpColor[3] = {0.1, 0.5, 0.1};
  colorProp->SetColor(tmpColor);

  vtkSmartPointer<vtkKWEObjectTreeNodeIterator> childIterator =
    vtkSmartPointer<vtkKWEObjectTreeNodeIterator>::New();
  childIterator->SetBaseNode( tmpPtr );
  childIterator->SetTraversalToChildrenOnly();
  vtkKWEObjectTreeNodeBase *currentChild;
  cout << "Iterating through children of Node 2:\n";
  for (childIterator->InitTraversal();
       (currentChild = childIterator->GetCurrentNode());
       childIterator->GoToNextNode())
    {
    cout << "  " << currentChild->GetName() << "\n";
    // want to add to "Node 6"
    if (!strcmp(currentChild->GetName(), "Node 6 (grandchild)"))
      {
      if (currentChild->AddProperty(colorProp) == 0)
        {
        cout << "Unable to add property (already exists)!!!!\n\n";
        buildResult = FAIL;
        }
      }
    }

  // actually set the Object on one of the nodes; We want it to be serialized,
  // so that we get something more than <NodeObject type="Pointer"/>; thus set
  // as the Property we just created.
  baseChild->SetNodeObject( colorProp );

  vtkSmartPointer<vtkKWEObjectTreeColorProperty> colorProp2 =
    vtkSmartPointer<vtkKWEObjectTreeColorProperty>::New();

  // the correct result is that we shouldn't be able to add the same
  // property to the node again
  if (!child[6]->AddProperty(colorProp2) == 0)
    {
    cout << "Property added!!!\n\n";
    buildResult = FAIL;
    }

  // but now remove the original and try adding again, which should succeed
  if (!child[6]->RemoveProperty( colorProp ))
    {
    cout << "Prop NOT removed!!!!\n\n";
    buildResult = FAIL;
    }
  if (child[6]->AddProperty(colorProp2) == 0)
    {
    cout << "Unable to add property (already exists)!!!!\n\n";
    buildResult = FAIL;
    }

  // modify the color property
  bool inheritedPropertyFlag;
  vtkKWEObjectTreeColorProperty *getColorProp =
    vtkKWEObjectTreeColorProperty::SafeDownCast(
    child[6]->GetProperty(vtkKWEObjectTreeColorProperty::KEY(), inheritedPropertyFlag) );
  double newColor[3] = {1, 1, 1};
  if (getColorProp)
    {
    getColorProp->SetColor(newColor);
    }

  // now add a "user" property
  vtkSmartPointer<vtkKWEObjectTreeUserProperty> userProp =
    vtkSmartPointer<vtkKWEObjectTreeUserProperty>::New();

  // make up some keys for the user property
  testUserIntKey = new vtkInformationIntegerKey("UserIntValue", "testTree");
  userProp->GetAttributesPointer()->Set(testUserIntKey, 5);
  testUserDoubleKey = new vtkInformationDoubleKey("UserDoubleValue", "testTree");
  userProp->GetAttributesPointer()->Set(testUserDoubleKey, 37.2);
  testUserIdTypeKey = new vtkInformationIdTypeKey("UserIdTypeValue", "testTree");
  userProp->GetAttributesPointer()->Set(testUserIdTypeKey, 9);
  testUserStringKey = new vtkInformationStringKey("UserStringValue", "testTree");
  userProp->GetAttributesPointer()->Set(testUserStringKey, "Test String");
  testUserIntVectorKey = new vtkInformationIntegerVectorKey("UserIntVectorValue", "testTree",3);
  int white[3] = {255, 255, 255};
  userProp->GetAttributesPointer()->Set(testUserIntVectorKey, white, 3);
  testUserDoubleVectorKey = new vtkInformationDoubleVectorKey("UserDoubleVectorValue","testTree",2);
  double tmpDouble[2] = {3.14, 10.0};
  userProp->GetAttributesPointer()->Set(testUserDoubleVectorKey, tmpDouble, 2);

  // and now registor the keys so they will be serialized (specifically, read in)
  vtkKWEInformationKeyMap::RegisterKey(testUserDoubleKey);
  vtkKWEInformationKeyMap::RegisterKey(testUserIdTypeKey);
  vtkKWEInformationKeyMap::RegisterKey(testUserIntKey);
  vtkKWEInformationKeyMap::RegisterKey(testUserStringKey);
  vtkKWEInformationKeyMap::RegisterKey(testUserIntVectorKey);
  vtkKWEInformationKeyMap::RegisterKey(testUserDoubleVectorKey);

  // and, of course, add the property to a node
  if (child[2]->AddProperty(userProp) == 0)
    {
    cout << "Unable to add property (already exists)!!!!\n\n";
    buildResult = FAIL;
    }

  // and, add it to another node
  child[6]->AddProperty(userProp);

  // the internal state... primarily to get coverage for the PrintSelf methods
  root->Print(cout);

  return buildResult;
}

int CompareIterationResult(vtkKWEObjectTreeNodeIterator *iterator,
                           vtkstd::string *(expectedResult),
                           int expectedCount)
{
  int testResult = PASS;
  vtkKWEObjectTreeNodeBase *currentNode;
  int iterationIndex = 0;
  for (iterator->InitTraversal();
       (currentNode = iterator->GetCurrentNode());
       iterator->GoToNextNode(), iterationIndex++)
    {
    vtkstd::string testString;
    for (int i = 0; i < iterator->GetCurrentDepth(); i++)
      {
      testString += "  ";
      }
    testString += currentNode->GetName();
    cout << testString;
    if (iterationIndex >= expectedCount ||
      expectedResult[iterationIndex] != testString)
      {
      cout << " (WRONG!)\n";
      testResult = FAIL;
      }
    else
      {
      cout << "\n";
      }
    }
  if (iterationIndex != expectedCount)
    {
    cout << "Iteration error: expected " << expectedCount <<
      " nodes, but iterated over " << iterationIndex << "\n";
    testResult = FAIL;
    }

  return testResult;
}

int TestIteration( vtkKWEObjectTreeNodeBase *root )
{
  int testResult = PASS;

  // Iteration Test/Example 0
  // "test" the iterator
  vtkSmartPointer<vtkKWEObjectTreeNodeIterator> iterator =
    vtkSmartPointer<vtkKWEObjectTreeNodeIterator>::New();
  iterator->SetBaseNode( root );
  iterator->SetTraversalToChildrenOnly();

  cout << "EXAMPLE 0: Iterating over the children of the BaseNode:\n";
  vtkstd::string expectedResult0[3] = {"  Node 1 (child)",
    "  Node 2 (child)","  Node 3 (child)"};
  testResult =
    CompareIterationResult(iterator, expectedResult0, 3) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 1
  // now the whole tree, depth-first
  iterator->SetTraversalToEntireSubtree();
  iterator->SetTraversalModeToDepthFirst();
  cout << "\nEXAMPLE 1: Iterating through root (depth-first):\n";
  vtkstd::string expectedResult1[8] = {"RootNode","  Node 1 (child)",
    "  Node 2 (child)","    Node 6 (grandchild)","    Node 7 (grandchild)",
    "      Node 8 (great-grandchild)","    Node 5 (grandchild)",
    "  Node 3 (child)"};
  testResult =
    CompareIterationResult(iterator, expectedResult1, 8) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 2
  // now the whole tree, but breadth-first
  iterator->SetTraversalModeToBreadthFirst();
  cout << "\nEXAMPLE 2: Iterating through root (breadth-first):\n";
  vtkstd::string expectedResult2[8] = {"RootNode","  Node 1 (child)",
    "  Node 2 (child)","  Node 3 (child)","    Node 6 (grandchild)",
    "    Node 7 (grandchild)","    Node 5 (grandchild)",
    "      Node 8 (great-grandchild)",};
  testResult =
    CompareIterationResult(iterator, expectedResult2, 8) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 3
  // now only nodes that have a UserProperty
  vtkSmartPointer<vtkKWEObjectTreeTransformableNode> patternNode =
    vtkSmartPointer<vtkKWEObjectTreeTransformableNode>::New();
  vtkSmartPointer<vtkKWEObjectTreeUserProperty> userProp =
    vtkSmartPointer<vtkKWEObjectTreeUserProperty>::New();
  patternNode->AddProperty(userProp);

  iterator->SetPatternNode( patternNode );
  cout << "\nEXAMPLE 3: Iterating through root (breadth-first), with matching node:\n";
  vtkstd::string expectedResult3[2] = {"  Node 2 (child)","    Node 6 (grandchild)"};
  testResult =
    CompareIterationResult(iterator, expectedResult3, 2) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 4
  // now only nodes that have a UserProperty... which can be inherited;
  // Note, "Node 7" will inherit a UserProperty but is of the wrong type
  // (the PatternNode is more specific... vtkKWEObjectTreeTransformableNode)
  // We repeat the same test in Test/Example 5, but make the PatternNode a
  // vtkKWEObjectTReeNodeBase, which will then include "Node 7"
  iterator->ConsiderInheritedPropertiesOn();
  cout << "\nEXAMPLE 4: Iterating through root (breadth-first), with matching node \n(can inherit property):\n";
  vtkstd::string expectedResult4[3] = {"  Node 2 (child)",
    "    Node 6 (grandchild)","    Node 5 (grandchild)"};
  testResult =
    CompareIterationResult(iterator, expectedResult4, 3) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 5
  // Same as #4, add an identity transform
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  patternNode->SetTransform(transform);
  cout << "\nEXAMPLE 5: Iterating through root (breadth-first), with matching node \n(can inherit property) and identity transform:\n";
  vtkstd::string expectedResult5[1] = {"    Node 6 (grandchild)"};
  testResult =
    CompareIterationResult(iterator, expectedResult5, 1) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 6
  // Save as #4, but now the PatternNode is a vtkKWEObjectTreeNodeBase, so that
  // all vtkKWEObjectTreeNodeBase(s) and subclasses are potential matches
  vtkSmartPointer<vtkKWEObjectTreeNodeBase> basePatternNode =
    vtkSmartPointer<vtkKWEObjectTreeNodeBase>::New();
  basePatternNode->AddProperty(userProp);
  iterator->SetPatternNode( basePatternNode );
  cout << "\nEXAMPLE 6: Iterating through root (breadth-first), with (more basic) \nPatternNode (+ can inherit property):\n";
  vtkstd::string expectedResult6[4] = {"  Node 2 (child)",
    "    Node 6 (grandchild)","    Node 7 (grandchild)","    Node 5 (grandchild)"};
  testResult =
    CompareIterationResult(iterator, expectedResult6, 4) == FAIL ? FAIL : testResult;

  // Iteration Test/Example 6
  // Find the Camera....
  basePatternNode->RemoveAllProperties();
  vtkSmartPointer<vtkCamera> tmpCamera = vtkSmartPointer<vtkCamera>::New();
  basePatternNode->SetNodeObject( tmpCamera );
  cout << "\nEXAMPLE 7: Iterating through root to find the (any) camera\n";
  vtkstd::string expectedResult7[1] = {"    Node 6 (grandchild)"};
  testResult =
    CompareIterationResult(iterator, expectedResult7, 1) == FAIL ? FAIL : testResult;

  cout << endl;

  return testResult;
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

bool AreInfoObjectsEqual(vtkInformation *info0, vtkInformation *info1)
{
  if (info0->GetNumberOfKeys() != info1->GetNumberOfKeys())
    {
    return false;
    }

  vtkSmartPointer<vtkInformationIterator> infoIterator =
    vtkSmartPointer<vtkInformationIterator>::New();
  infoIterator->SetInformation( info0 );
  for (infoIterator->InitTraversal(); !infoIterator->IsDoneWithTraversal();
    infoIterator->GoToNextItem())
    {
    vtkInformationKey *key = infoIterator->GetCurrentKey();
    if (key->IsA("vtkInformationIntegerKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationIntegerKey>( info1,
        info0, static_cast<vtkInformationIntegerKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationDoubleKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationDoubleKey>( info1,
        info0, static_cast<vtkInformationDoubleKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationIdTypeKey"))
      {
      if (!AreScalarKeysEqual<vtkInformationIdTypeKey>( info1,
        info0, static_cast<vtkInformationIdTypeKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationStringKey"))
      {
      vtkInformationStringKey *stringKey = static_cast<vtkInformationStringKey*>(key);
      if (!info1->Has(stringKey) ||
        strcmp(info1->Get(stringKey), info0->Get(stringKey)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationIntegerVectorKey"))
      {
      if (!AreVectorKeysEqual<vtkInformationIntegerVectorKey, int>(
        info1, info0, static_cast<vtkInformationIntegerVectorKey*>(key)))
        {
        return false;
        }
      }
    else if (key->IsA("vtkInformationDoubleVectorKey"))
      {
      if (!AreVectorKeysEqual<vtkInformationDoubleVectorKey, double>(
        info1, info0, static_cast<vtkInformationDoubleVectorKey*>(key)))
        {
        return false;
        }
      }
    }

  return true;
}

//-----------------------------------------------------------------------------
template <typename ArrayType, typename ArrayDataType>
bool AreTreeObjectDataArraysEqual(vtkKWEObjectTreeNodeBase *tree1,
                                  vtkKWEObjectTreeNodeBase *tree2)
{
  vtkSmartPointer<vtkKWEObjectTreeNodeIterator> iterator =
  vtkSmartPointer<vtkKWEObjectTreeNodeIterator>::New();
  iterator->SetTraversalToEntireSubtree();
  vtkSmartPointer<vtkKWEObjectTreeTransformableNode> patternNode =
    vtkSmartPointer<vtkKWEObjectTreeTransformableNode>::New();
  patternNode->SetNodeObject( vtkSmartPointer<ArrayType>::New() );
  iterator->SetPatternNode( patternNode );

  vtkKWEObjectTreeNodeBase *node;
  ArrayType *array1 = 0, *array2 = 0;

  // get array from the 1st tree
  iterator->SetBaseNode( tree1 );
  iterator->InitTraversal();
  node = iterator->GetCurrentNode();
  if (node)
    {
    array1 = ArrayType::SafeDownCast( node->GetNodeObject() );
    }

  // get array from the 1st tree
  iterator->SetBaseNode( tree2 );
  iterator->InitTraversal();
  node = iterator->GetCurrentNode();
  if (node)
    {
    array2 = ArrayType::SafeDownCast( node->GetNodeObject() );
    }

  // we very much expect there to be an array in each tree, but just in case
  if (!array1 || !array2)
    {
    cout << "Error: Iterator failed to find vtkDataArrays of expected type\n";
    return false;
    }

  if (array1->GetDataSize() != array2->GetDataSize() ||
    array1->GetNumberOfComponents() != array2->GetNumberOfComponents())
    {
    cout << "Error: Comparing vtkDataArrays... sizes don't match\n";
    return false;
    }

  for (vtkIdType i = 0; i < array1->GetDataSize(); i++)
    {
    if (array1->GetValue(i) != array2->GetValue(i))
      {
      cout << "Error: vtkDataArrays don't match\n";
      return false;
      }
    }

  if (array1->HasInformation() && array2->HasInformation())
    {
    if (!AreInfoObjectsEqual(array1->GetInformation(), array2->GetInformation()))
      {
      cout << "Error: vtkDataArray vtkInformation doesn't match!\n";
      return false;
      }
    }
  else if (array1->HasInformation() || array2->HasInformation())
    {
    cout << "Error: Missing vtkInformation on vtkDataArray\n";
    return false;
    }

  return true;
}

static int ObjectTreeDemo(int argc, char* argv[])
{
  vtkstd::string filename;

  vtksys::CommandLineArguments arg;
  arg.StoreUnusedArguments(1);
  arg.Initialize(argc, argv);

  typedef vtksys::CommandLineArguments argT;
  arg.AddArgument("--input", argT::EQUAL_ARGUMENT, &filename,
    "(optional) Enter xml file to load for comparison to tree created in memory");

  if (!arg.Parse())
    {
    cerr << "Problem parsing arguments." << endl;
    cerr << arg.GetHelp() << endl;
    return 1;
    }

  int testResult = PASS;

  // Register keys with the map. This makes it possible for the
  // archiver to access this key when reading an archive
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeNodeBase::NAME());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeNodeBase::UUID());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeNodeBase::STATE());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeNodeBase::INHERIT_PROPERTIES());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeUserProperty::KEY());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeColorProperty::KEY());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreeColorProperty::COLOR());
  vtkKWEInformationKeyMap::RegisterKey(vtkKWEObjectTreePropertyBase::IS_INHERITABLE());

  // ObjectTree we build
  vtkSmartPointer<vtkKWEObjectTreeTransformableNode> root =
    vtkSmartPointer<vtkKWEObjectTreeTransformableNode>::New();
  testResult = BuildOriginalTree(root);

  // Output the tree to an archive (a string stream)
  vtkSmartPointer<vtkKWEXMLArchiveWriter> strWriter =
    vtkSmartPointer<vtkKWEXMLArchiveWriter>::New();
  vtksys_ios::ostringstream ostr;
  // Set to version to 1 (default is 0)
  strWriter->SetArchiveVersion(1);
  // The archiver expects a vector of objects
  vtkstd::vector<vtkSmartPointer<vtkObject> > objs;
  objs.push_back(root);
  // just for grins, and because I just changed the input to Serialize to take
  // a vector of vtkObjects instead of vtkSerializableObjects, add the camera
  // that is the "Object" of "Node 6" to the root objects
  objs.push_back(camera);
  // The root node with be called ObjectTree. This is for
  // reference only.
  strWriter->Serialize(ostr, "ObjectTree", objs);
  // Print out the XML
  cout << ostr.str().c_str() << endl;

  // ObjectTree read from disk (saved previously... baseline for what we expect)
  if (filename != "")
    {
    // Create an input stream to read the XML back
    vtksys_ios::ifstream ifstr( filename.c_str() );

    // Read using a vtkKWEXMLArchiveReader
    vtkSmartPointer<vtkKWEXMLArchiveReader> reader =
      vtkSmartPointer<vtkKWEXMLArchiveReader>::New();
    vtkstd::vector<vtkSmartPointer<vtkObject> > inObjs;
    reader->Serialize(ifstr, "ObjectTree", inObjs);
    ifstr.close();

    // compare tree we read in to the one we built
    vtkKWEObjectTreeNodeBase *treeFromDisk =
      vtkKWEObjectTreeNodeBase::SafeDownCast( inObjs[0] );
    if (!root->IsEqualTo(treeFromDisk, true))
      {
      cout << "Error: Tree we built doesn't match baseline tree read from disk.\n\n";
      testResult = FAIL;
      }
    }

  // Test IO via vtkKWEXMLElement instead of a stream
  vtkKWEXMLElement *rootElement = vtkKWEXMLElement::New();
  vtkSmartPointer<vtkKWEXMLArchiveWriter> elemWriter =
    vtkSmartPointer<vtkKWEXMLArchiveWriter>::New();
  elemWriter->Serialize(rootElement, "My Name", root);

  vtkSmartPointer<vtkKWEXMLArchiveReader> elemReader =
    vtkSmartPointer<vtkKWEXMLArchiveReader>::New();
  vtkstd::vector<vtkSmartPointer<vtkObject> > inFromElementObjs;
  elemReader->Serialize(rootElement, 0, inFromElementObjs);
  vtkKWEObjectTreeNodeBase *treeFromElement =
    vtkKWEObjectTreeNodeBase::SafeDownCast( inFromElementObjs[0] );
  if (!root->IsEqualTo(treeFromElement, true))
    {
    cout << "Error: Tree we built doesn't match baseline tree read from XML element.\n\n";
    testResult = FAIL;
    }
  rootElement->Delete();

  // Test iterating over the tree
  if (TestIteration(root) == FAIL)
    {
    testResult = FAIL;
    }

  /////////////////////////////////////////////////////////////////////////////
  // write out the tree we built, then read it back and compare to the original
  // to test write/read...
  /////////////////////////////////////////////////////////////////////////////
  char *tmpFileName =
    vtkTestUtilities::GetArgOrEnvOrDefault("-T", argc, argv, "VTK_TEMP_DIR", ".");
  vtkstd::string testOutputInputFilename = tmpFileName;
  delete [] tmpFileName;
  testOutputInputFilename += "/testObjectTree.xml";

  // Create an output stream to write the XML
  vtksys_ios::ofstream ofstr( testOutputInputFilename.c_str() );
  vtkSmartPointer<vtkKWEXMLArchiveWriter> writer =
    vtkSmartPointer<vtkKWEXMLArchiveWriter>::New();

  // Set to version to 1 (default is 0)
  writer->SetArchiveVersion(1);
  vtkstd::vector<vtkSmartPointer<vtkObject> > outObjs;
  outObjs.push_back(root);
  outObjs.push_back(root);
  vtkSmartPointer<vtkInformation> tmpInfo = vtkSmartPointer<vtkInformation>::New();
  double tmpDouble[2] = {3.14, 10.0};
  tmpInfo->Set(testUserDoubleVectorKey, tmpDouble, 2);
  outObjs.push_back(tmpInfo);

  // The root node with be called ObjectTree. This is for reference only.
  writer->Serialize(ofstr, "ObjectTree", outObjs);
  ofstr.close();

  // now read it back in
  vtksys_ios::ifstream ifstr( testOutputInputFilename.c_str() );

  // Read using a vtkKWEXMLArchiveReader
  vtkSmartPointer<vtkKWEXMLArchiveReader> reader =
    vtkSmartPointer<vtkKWEXMLArchiveReader>::New();
  vtkstd::vector<vtkSmartPointer<vtkObject> > inObjs;
  reader->Serialize(ifstr, "ObjectTree", inObjs);
  ifstr.close();

  // compare tree we read in to the one we built
  vtkKWEObjectTreeNodeBase *treeFromDisk = 0;
  if (inObjs.size() > 0)
    {
    treeFromDisk = vtkKWEObjectTreeNodeBase::SafeDownCast( inObjs[0] );
    if (!root->IsEqualTo(treeFromDisk, true))
      {
      cout << "Error: Tree we built doesn't match tree written then read from disk.\n\n";
      testResult = FAIL;
      }
    else // compare the data arrays
      {
      if (!AreTreeObjectDataArraysEqual<vtkIntArray, int>(root, treeFromDisk))
        {
        cout << "Error: vtkIntArray in tree we built doesn't match the one written then read from disk.\n\n";
        testResult = FAIL;
        }
      if (!AreTreeObjectDataArraysEqual<vtkIdTypeArray, vtkIdType>(root, treeFromDisk))
        {
        cout << "Error: vtkIdTypeArray in tree we built doesn't match the one written then read from disk.\n\n";
        testResult = FAIL;
        }
      if (!AreTreeObjectDataArraysEqual<vtkDoubleArray, double>(root, treeFromDisk))
        {
        cout << "Error: vtkDoubleArray in tree we built doesn't match the one written then read from disk.\n\n";
        testResult = FAIL;
        }
      }
    }
  else
    {
    cout << "No tree(s) read.\n\n";
    return FAIL;
    }

  // compare info we read in to the one we built
  if (inObjs.size() > 2)
    {
    vtkInformation *infoFromDisk =
      vtkInformation::SafeDownCast( inObjs[2] );
    if (!infoFromDisk || !AreInfoObjectsEqual(infoFromDisk, tmpInfo))
      {
      cout << "Error: vtkInfomration we built doesn't match info written then read from disk.\n\n";
      testResult = FAIL;
      }
    }
  else
    {
    cout << "No information object read.\n\n";
    testResult = FAIL;
    }

  /////////////////////////////////////////////////////////////////////////////
  // Finally, let's start mucking around with the tree, trying doing various
  // comparisons along the way
  /////////////////////////////////////////////////////////////////////////////
  bool inheritedPropertyFlag;
  vtkKWEObjectTreeUserProperty *userProperty =
    vtkKWEObjectTreeUserProperty::SafeDownCast(
    treeFromDisk->GetChild(1)->GetProperty( vtkKWEObjectTreeUserProperty::KEY(), inheritedPropertyFlag ) );
  if (userProperty)
    {
    int originalValue = userProperty->GetAttributesPointer()->Get(testUserIntKey);
    userProperty->GetAttributesPointer()->Set(testUserIntKey, originalValue + 2);
    if (root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "changed a property; should be different!!!\n\n";
      testResult = FAIL;
      }
    userProperty->GetAttributesPointer()->Set(testUserIntKey, originalValue);
    if (!root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "changed property back; should be equal!!!\n\n";
      testResult = FAIL;
      }
    userProperty->GetAttributesPointer()->Remove(testUserIntKey);
    if (root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "removed a property; should be different!!!\n\n";
      testResult = FAIL;
      }
    userProperty->GetAttributesPointer()->Set(testUserIntKey, originalValue);
    if (!root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "added property again; should be equal!!!\n\n";
      testResult = FAIL;
      }

    int *vectorValue = userProperty->GetAttributesPointer()->Get(testUserIntVectorKey);
    int original[3] = {vectorValue[0], vectorValue[1], vectorValue[2]};
    int purple[3] = {255, 0, 255};
    userProperty->GetAttributesPointer()->Set(testUserIntVectorKey, purple, 3);
    if (root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "changed a property (vector); should be different!!!\n\n";
      testResult = FAIL;
      }
    userProperty->GetAttributesPointer()->Set(testUserIntVectorKey, original, 3);
    if (!root->IsEqualTo( treeFromDisk, true ))
      {
      cout << "changed property back; should be equal!!!\n\n";
      testResult = FAIL;
      }
    }
  else
    {
    cout << "Didn't get UserProperty as expected!!\n\n";
    testResult = FAIL;
    }

  int uuidResult = root->CreateUUID();
  if (uuidResult == 1)
    {
    cout << "UUID Generated for Node: " << root->GetUUID() << "\n";
    unsigned char uuid[16];
    vtkKWEUUID::ConstructUUID(uuid);
    vtkstd::string uuidString;
    vtkKWEUUID::ConvertBinaryUUIDToString(uuid, uuidString);
    cout << "Constructed UUID (just testing): " << uuidString << "\n\n";
    }
  else //uuidResult == 2
    {
    cout << "UUID Constructed for Node (Generate failed): " <<
      root->GetUUID() << "\n\n";
    }
  if (root->IsEqualTo( treeFromDisk, false))
    {
    cout << "added UUID; should be different!!!\n\n";
    testResult = FAIL;
    }
  if (!root->IsEqualTo( treeFromDisk, false, true))
    {
    cout << "added UUID; but is superset, so should be \"equal\"!!!\n\n";
    testResult = FAIL;
    }
  root->ClearUUID();  //clear UUID so createPseudoUUID will do something

  // change the tree we read in, and now expect a difference
  treeFromDisk->GetChild(1)->RemoveChild(2);
  if (root->IsEqualTo( treeFromDisk, true ))
    {
    cout << "removed a child; should be different!!!\n\n";
    testResult = FAIL;
    }

  if (testResult == PASS)
    {
    cout << "\nAll tests passed!\n\n";
    }

  return testResult;
}


#ifndef KWE_TESTING
int main(int argc, char* argv[])
{
  return ObjectTreeDemo(argc, argv);
}
#endif
