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
// .NAME vtkKWESerializableObject - Superclass for serializable objects
// .SECTION Description
// vtkKWESerializableObject works with vtkKWESerializer and its subclasses
// to provide a serialization framework. The user asks a vtkKWESerializer to
// write or read an archive consisting of a collection of vtkKWESerializableObjects.
// The serializer then reads/writes the archive in collaboration with the
// serializable objects.
// When reading an archive, the serializer creates the appropriate objects
// (usually using the instantiator) and then calls Serialize() on them.
// It is then the responsability of these objects to restore their state
// by calling functions provided by the serializer. For example, this may
// look like:
// \code
// void vtkKWEObjectTreeNodeBase::Serialize(vtkKWESerializer* ser)
// {
// ser->Serialize("Attributes", this->Attributes);
// ser->Serialize("Properties", this->Properties);
// vtkKWESerializableObject *parent = this->Parent;
// ser->Serialize("Parent", parent, true); // true indicates it is a weak ptr
// if (ser->IsWriting())
//   {
//   vtkstd::vector< vtkSmartPointer<vtkKWESerializableObject> > myVector =
//     vtkKWESerializer::ToBase<vtkstd::vector<vtkSmartPointer<vtkKWEObjectTreeNodeBase> > >(
//        *this->Children );
//   ser->Serialize("Children", myVector);
//   }
// else
//   {
//   this->Parent = vtkKWEObjectTreeNodeBase::SafeDownCast(parent);
//   vtkstd::vector< vtkSmartPointer<vtkKWESerializableObject> > myVector;
//   ser->Serialize("Children", myVector);
//   vtkKWESerializer::FromBase<vtkKWEObjectTreeNodeBase>(myVector, *this->Children);
//   }
// \endcode
// When writing the archive, the serializer calls Serialize() on the "root" 
// objects (the ones passed by the user) which then use the serializer methods
// to write themselves in the archive.
//
// Note that there is only one Serialize() method that does reading and writing.
// This works because the methods in vtkKWESerializer take references to data
// member and can read or write them as appropriate. In cases where the class
// needs to do something different during reading or writing, you can separate
// the implementation using if (ser->IsWriting()) {..} else {..} as shown
// in the example above.
//
// You can also implement more sophisticated serialization code by using
// temporaries during reading and writing and copying them to the actual data
// members yourself. Something like:
// \code
// void vtkSomeSerializableObject::Serialize(vtkKWESerializer* ser)
// {
//   int foo;
//   if (ser->IsWriting)
//     {
//     // ...
//     }
//   else
//     {
//     int foo;
//     ser->Serialize(foo);
//     // Based on the value of foo, do something to data members
//     }
// \endcode
//
// The serializer also supports versioning. You can get the version of the
// archive being written or read using vtkKWESerializer::GetArchiveVersion()

#ifndef __vtkKWESerializableObject_h
#define __vtkKWESerializableObject_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWESerializer;

class VTKEdge_IO_EXPORT vtkKWESerializableObject : public vtkObject
{
public:
  static vtkKWESerializableObject *New();
  vtkTypeRevisionMacro(vtkKWESerializableObject,vtkObject);

  // Description:
  // Reads the state of an instance from an archive OR
  // writes the state of an instance to an archive. See
  // the documentation for this class for details.
  virtual void Serialize(vtkKWESerializer*) {}
  
protected:
  vtkKWESerializableObject();
  ~vtkKWESerializableObject();

private:
  vtkKWESerializableObject(const vtkKWESerializableObject&);  // Not implemented.
  void operator=(const vtkKWESerializableObject&);  // Not implemented.
};

#endif
