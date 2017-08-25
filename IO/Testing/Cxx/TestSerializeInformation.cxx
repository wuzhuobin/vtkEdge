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

#include "vtkKWEXMLArchiveWriter.h"
#include "vtkKWEXMLArchiveReader.h"
#include "vtkKWEInformationKeyMap.h"

#include "vtkCamera.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkInformationDoubleKey.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationStringKey.h"
#include "vtkInformationStringVectorKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationObjectBaseVectorKey.h"
#include "vtkSmartPointer.h"
#include "vtkCommonInstantiator.h"

#include <vtksys/ios/sstream>

#define VTK_CREATE(type, var) \
  vtkSmartPointer<type> var = vtkSmartPointer<type>::New()

#define fail(msg) \
  std::cerr << msg << std::endl; \
  return EXIT_FAILURE;


namespace {
class TestClass
{
public:

  // Method to register the keys
  static void RegisterKeys();

  // The keys
  static vtkInformationIntegerKey*           TestInt();
  static vtkInformationIntegerVectorKey*     TestIntVector();
  static vtkInformationDoubleKey*            TestDouble();
  static vtkInformationDoubleVectorKey*      TestDoubleVector();
  static vtkInformationStringKey*            TestString();
  static vtkInformationStringVectorKey*      TestStringVector();
  static vtkInformationObjectBaseKey*        TestObject();
  static vtkInformationObjectBaseVectorKey*  TestObjectVector();
};

vtkInformationKeyMacro(TestClass, TestInt,          Integer);
vtkInformationKeyMacro(TestClass, TestIntVector,    IntegerVector);
vtkInformationKeyMacro(TestClass, TestDouble,       Double);
vtkInformationKeyMacro(TestClass, TestDoubleVector, DoubleVector);
vtkInformationKeyMacro(TestClass, TestString,       String);
vtkInformationKeyMacro(TestClass, TestStringVector, StringVector);
vtkInformationKeyMacro(TestClass, TestObject,       ObjectBase);
vtkInformationKeyMacro(TestClass, TestObjectVector, ObjectBaseVector);

void TestClass::RegisterKeys()
{
  vtkKWEInformationKeyMap::RegisterKey(TestInt());
  vtkKWEInformationKeyMap::RegisterKey(TestIntVector());
  vtkKWEInformationKeyMap::RegisterKey(TestDouble());
  vtkKWEInformationKeyMap::RegisterKey(TestDoubleVector());
  vtkKWEInformationKeyMap::RegisterKey(TestString());
  vtkKWEInformationKeyMap::RegisterKey(TestStringVector());
  vtkKWEInformationKeyMap::RegisterKey(TestObject());
  vtkKWEInformationKeyMap::RegisterKey(TestObjectVector());
}
}


int TestSerializeInformation(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  TestClass::RegisterKeys();

  // Create info object and serializers
  VTK_CREATE(vtkInformation, info);
  VTK_CREATE(vtkKWEXMLArchiveReader, reader);
  VTK_CREATE(vtkKWEXMLArchiveWriter, writer);
  vtksys_ios::stringstream xmlStream;

  // Create cameras
  VTK_CREATE(vtkCamera, cam1);
  VTK_CREATE(vtkCamera, cam2);
  cam1->SetPosition(1.0, 2.0, 3.0);
  cam2->SetPosition(4.0, 5.0, 6.0);

  // Set values in the information object
  info->Set(TestClass::TestInt(), 1);
  info->Set(TestClass::TestIntVector(), 1, 2, 3);
  info->Set(TestClass::TestDouble(), 1.0);
  info->Set(TestClass::TestDoubleVector(), 1.0, 2.0, 3.0);
  info->Set(TestClass::TestString(), "foo");
  info->Set(TestClass::TestStringVector(), "foo", 0);
  info->Set(TestClass::TestStringVector(), "ba\"r", 1);
  info->Set(TestClass::TestObject(), cam1);
  TestClass::TestObjectVector()->Set(info, cam1, 0);
  TestClass::TestObjectVector()->Set(info, NULL, 1);
  TestClass::TestObjectVector()->Set(info, cam2, 2);


  // The writer expects a vector of objects
  vtkstd::vector<vtkSmartPointer<vtkObject> > objs;
  objs.push_back(info);

  // Serialize the object vector
  writer->Serialize(xmlStream, "info", objs);

  // Print out the XML
  std::cerr << xmlStream.str().c_str() << std::endl;

  // Read the XML to deserialize the objects
  objs.clear();
  reader->Serialize(xmlStream, "info", objs);
  if (objs.size() != 1)
    {
    fail("Serialization reader didn't produce correct number of objects.");
    }

  // Get the new vtkInformation object created for the reader
  vtkSmartPointer<vtkInformation> newInfo = vtkInformation::SafeDownCast(objs[0]);
  objs.clear();
  if (!newInfo)
    {
    fail("Did not get new information object.");
    }

  // Get the values stored in the new information object
  int testInt = info->Get(TestClass::TestInt());
  int* testIntVector = info->Get(TestClass::TestIntVector());
  double testDouble = info->Get(TestClass::TestDouble());
  double* testDoubleVector = info->Get(TestClass::TestDoubleVector());
  const char* testString = info->Get(TestClass::TestString());
  const char* testStringVector[2];
  testStringVector[0] = info->Get(TestClass::TestStringVector(), 0);
  testStringVector[1] = info->Get(TestClass::TestStringVector(), 1);
  vtkCamera* newCam1 = vtkCamera::SafeDownCast(info->Get(TestClass::TestObject()));
  vtkCamera* newCam2 = vtkCamera::SafeDownCast(TestClass::TestObjectVector()->Get(info, 0));
  vtkObjectBase* nullPtr = TestClass::TestObjectVector()->Get(info, 1);
  vtkCamera* newCam3 = vtkCamera::SafeDownCast(TestClass::TestObjectVector()->Get(info, 2));


  // Test all the values

  if (testInt != 1)
    {
    fail("Integer has incorrect value.");
    }

  if (   testIntVector[0] != 1
      || testIntVector[1] != 2
      || testIntVector[2] != 3)
    {
    fail("Integer vector has incorrect value.");
    }

  if (testDouble != 1.0)
    {
    fail("Double has incorrect value.");
    }

  if (   testDoubleVector[0] != 1.0
      || testDoubleVector[1] != 2.0
      || testDoubleVector[2] != 3.0)
    {
    fail("Double vector has incorrect value.");
    }

  if (strcmp(testString, "foo"))
    {
    fail("String has incorrect value.");
    }

  if (   strcmp(testStringVector[0], "foo")
      || strcmp(testStringVector[1], "ba\"r"))
    {
    fail("String vector has incorrect value.");
    }

  if (!newCam1)
    {
    fail ("Failed to get camera object.");
    }

  if (nullPtr)
    {
    fail ("Second object should have been null.");
    }

  // These pointers should be the same since they were the same when we inserted
  // them into the original infomation object
  if (newCam1 != newCam2)
    {
    fail ("Camera objects were not the same pointer.");
    }

  double* newCam2Pos = newCam2->GetPosition();
  double* newCam3Pos = newCam3->GetPosition();

  if (   newCam2Pos[0] != 1.0
      || newCam2Pos[1] != 2.0
      || newCam2Pos[2] != 3.0)
    {
    fail("First camera has incorrect position.");
    }

  if (   newCam3Pos[0] != 4.0
      || newCam3Pos[1] != 5.0
      || newCam3Pos[2] != 6.0)
    {
    fail("Second camera has incorrect position.");
    }

  return EXIT_SUCCESS;
}

