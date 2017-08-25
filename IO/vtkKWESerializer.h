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
// .NAME vtkKWESerializer - Abstract superclass of input and output archivers
// .SECTION Description
// vtkKWESerializer and its sub-classes are used to serialize and de-serialize
// a collection of objects to/from an io stream. The serializers can work
// with vtkKWESerializableObject and its subclasses, or vtkObjects in general
// that have a subclass of vtkKWESerializationHelper registered with
// vtkKWESerializationHelperMap. It can walk a object graph and
// serialize/deserialize all objects contained in it. Circular references are
// supported.
// In order to (most easily) add serialization support to your class, subclass
// from vtkKWESerializableObject and re-implement its Serialize() method using
// methods available in the serializer.  Otherwise create a subclass
// vtkKWESerializationHelper and register it with the serialization manager
// See vtkKWESerializableObject for details.
// .SECTION See Also
// vtkKWESerializableObject vtkKWESerializationHelper vtkKWESerializationHelperMap

#ifndef __vtkKWESerializer_h
#define __vtkKWESerializer_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include "vtkSmartPointer.h" // For collections
#include <vtkstd/vector> // For vectors
#include <vtkstd/map> // For maps

class vtkInformation;

class VTKEdge_IO_EXPORT vtkKWESerializer : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkKWESerializer,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This method returns true if the serializer is an output
  // serializer (writer)
  virtual bool IsWriting() = 0;

  // Description:
  // Serializes a single integer.
  virtual void Serialize(const char* name, int& val) = 0;

  // Description:
  // Serializes an array.
  virtual void Serialize(const char* name, int*& val, unsigned int& length) = 0;

  // Description:
  // Serializes a single unsigned long.
  virtual void Serialize(const char* name, unsigned long& val) = 0;

 // Description:
  // Serializes an array.
  virtual void Serialize(const char* name, unsigned long*& val, unsigned int& length) = 0;

  // Description:
  // Serializes a single vtkIdType.
#if defined(VTK_USE_64BIT_IDS)
  virtual void Serialize(const char* name, vtkIdType& val) = 0;
#endif

  // Description:
  // Serializes an array.
#if defined(VTK_USE_64BIT_IDS)
  virtual void Serialize(const char* name, vtkIdType*& val, unsigned int& length) = 0;
#endif

  // Description:
  // Serializes a single double.
  virtual void Serialize(const char* name, double& val) = 0;

  // Description:
  // Serializes an array.
  virtual void Serialize(const char* name, double*& val, unsigned int& length) = 0;

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, char*& str) = 0;

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, vtkstd::string& str) = 0;

  // Description:
  // Serializes a vtkObject.
  virtual void Serialize(const char* name, vtkObject*& obj, bool weakPtr = false) = 0;

  // Description:
  // Serializes a vtkInformationObject. Note that only keys registered
  // with the vtkKWEInformationKeyMap are restored.
  virtual void Serialize(const char* name, vtkInformation* info) = 0;

  // Description:
  // Serializes a vector of vtkObjects.
  virtual void Serialize(const char* name,
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs,
    bool weakPtr = false) = 0;

  // Description:
  // Serializes a map from int to vector of vtkObjects.
  virtual void Serialize(const char* name,
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >& objs) = 0;

  // Description:
  // Set/Get the archive version. Make sure to set the version before
  // writing to an archive. When reading an archive, the version is read
  // from the input stream.
  vtkSetMacro(ArchiveVersion, unsigned int);
  vtkGetMacro(ArchiveVersion, unsigned int);

  // Description:
  // Helper function to make is easier to write containers of sub-classes
  // of vtkObject. For example:
  // \code
  // vtkstd::Container<vtkSmartPointer<vtkSubclass> > sub;
  // vtkstd::vector<vtkSmartPointer<vtkObject> > vec =
  //    vtkKWESerializer::ToBase<Container>(sub);
  //  ser->Serialize("Vector", vec);
  // \endcode
  // The container must support forward iteration, begin(), end(), and
  // push_back.
  template <typename Container>
  static vtkstd::vector<vtkSmartPointer<vtkObject> > ToBase(Container& from)
    {
    vtkstd::vector<vtkSmartPointer<vtkObject> > retVal;
    typename Container::iterator iter = from.begin();
    for(; iter != from.end(); iter++)
      {
      retVal.push_back(*iter);
      }
    return retVal;
    }

  // Description:
  // Helper function to make it easier to read into containers of sub-classes
  // of vtkObject. For example:
  // \code
  // vtkstd::Container<vtkSmartPointer<vtkSubclass> > sub;
  // vtkstd::vector<vtkSmartPointer<vtkObject> >  vec;
  // ser->Serialize("Vector", vec);
  //  vtkKWESerializer::FromBase<vtkConceptualModelItem,Container>(vec, sub);
  // \endcode
  // The container must support forward iteration, begin(), end(), and
  // push_back.
  template <typename T, typename Container>
  static void FromBase(
    vtkstd::vector<vtkSmartPointer<vtkObject> >& from, Container& to)
    {
    vtkstd::vector<vtkSmartPointer<vtkObject> >::iterator iter =
      from.begin();
    for(; iter != from.end(); iter++)
      {
//      to.push_back(static_cast<T*>(iter->GetPointer()));
      to.insert(to.end(), static_cast<T*>(iter->GetPointer()));
      }
    }

  // Description:
  // Helper function to make it easier to write maps of containers of sub-classes
  // of vtkObject. For example:
  // \code
  // vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > > map =
  //  vtkKWESerializer::ToBase<Container>(this->Internal->Associations);
  //  ser->Serialize("Associations", map);
  // \endcode
  // The container must support forward iteration, begin(), end(), and
  // push_back.
  template <typename Container>
  static vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >
    ToBase(vtkstd::map<int, Container>& from)
    {
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > > retVal;
    typename vtkstd::map<int, Container>::iterator iter =
      from.begin();
    for(; iter != from.end(); iter++)
      {
      retVal[iter->first] = (ToBase<Container>(iter->second));
      }
    return retVal;
    }

  // Description:
  // Helper function to make it easier to read maps of containers of sub-classes
  // of vtkObject. For example:
  // \code
  // vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > > map;
  // ser->Serialize("Associations", map);
  // vtkKWESerializer::FromBase<vtkConceptualModelItem,Container>(map, this->Internal->Associations);
  // \endcode
  // The container must support forward iteration, begin(), end(), and
  // push_back.
  template <typename T, typename Container>
  static void FromBase(
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >& from,
    vtkstd::map<int, Container>& to)
    {
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >::iterator iter = from.begin();
    for(; iter != from.end(); iter++)
      {
      vtkstd::vector<vtkSmartPointer<vtkObject> >& f =
        iter->second;
      Container& t = to[iter->first];
      FromBase<T, Container>(f, t);
      }
    }

protected:
  vtkKWESerializer();
  ~vtkKWESerializer() {}

private:
  vtkKWESerializer(const vtkKWESerializer&);  // Not implemented.
  void operator=(const vtkKWESerializer&);  // Not implemented.

  unsigned int ArchiveVersion;
};

#endif
