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
// .NAME vtkKWEXMLArchiveReader - Reads an XML archive from input stream
// .SECTION Description
// This concrete subclass of vtkKWESerializer reads an XML archive from
// an input stream and create a collection of sub-classes of vtkObject.
// For example:
// \code
//  vtkstd::vector<vtkSmartPointer<vtkObject> > objs;
//  ifstream ifs(filename);
//  
//  vtkSmartPointer<vtkKWEXMLArchiveReader> reader = 
//    vtkSmartPointer<vtkKWEXMLArchiveReader>::New();
//  reader->Serialize(istr, "ObjectTree", objs);
// .. Do something with objs
// \endcode
// See vtkKWEXMLArchiveWriter for details about the XML format.
// .SECTION See Also
// vtkKWESerializer vtkKWEXMLArchiveWriter

#ifndef __vtkKWEXMLArchiveReader_h
#define __vtkKWEXMLArchiveReader_h

#include "vtkKWESerializer.h"
#include "VTKEdgeConfigure.h" // include configuration header

//BTX
class vtkKWEXMLElement;
struct vtkKWEXMLArchiveReaderInternals;
//ETX

class VTKEdge_IO_EXPORT vtkKWEXMLArchiveReader : public vtkKWESerializer
{
public:
  static vtkKWEXMLArchiveReader *New();
  vtkTypeRevisionMacro(vtkKWEXMLArchiveReader,vtkKWESerializer);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This method returns true if the serializer is an output
  // serializer (writer). Returns false.
  virtual bool IsWriting() {return false;}

  // Description:
  // Main entry point called to read an XML archive.
  // It populates the obj vector with the root objects in the
  // archive (under the RootObjects element).
  virtual void Serialize(istream& istr, const char* rootName, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);

  // Description:
  // Additional entry point called to read an XML archive from a
  // vtkKWEXMLElement (as opposed to "from a stream"). It 
  // populates the obj vector with the root objects in the
  // archive (under the RootObjects element).
  virtual void Serialize(vtkKWEXMLElement *rootElement, const char* rootName, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);

  // Description:
  // Reads a single integer.
  virtual void Serialize(const char* name, int& val);

  // Description:
  // Reads an array.
  virtual void Serialize(const char* name, int*& val, unsigned int& length);

  // Description:
  // Serializes a single unsigned long.
  virtual void Serialize(const char* name, unsigned long& val) ;
  
  // Description:
  // Serializes an array.
  virtual void Serialize(const char* name, unsigned long*& val, unsigned int& length);

  // Description:
  // Reads a single vtkIdType.
#if defined(VTK_USE_64BIT_IDS)
  virtual void Serialize(const char* name, vtkIdType& val);
#endif

  // Description:
  // Reads an array.
#if defined(VTK_USE_64BIT_IDS)
  virtual void Serialize(const char* name, vtkIdType*& val, unsigned int& length);
#endif

  // Description:
  // Reads a single double.
  virtual void Serialize(const char* name, double& val);
  
  // Description:
  // Reads an array.
  virtual void Serialize(const char* name, double*& val, unsigned int& length);

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, char*& str);

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, vtkstd::string& str);

  // Description:
  // Reads a vtkObject.  weakPtr should be set to true if this reference to the
  // object should NOT be reference counted.
  virtual void Serialize(const char* name, vtkObject*& obj, bool weakPtr = false);

  // Description:
  // Reads a vtkInformationObject. Note that only keys registered
  // with the vtkKWEInformationKeyMap are restored.
  virtual void Serialize(const char* name, vtkInformation* info);

  // Description:
  // Reads a vector of vtkObjects.
  virtual void Serialize(const char* name, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs,
    bool weakPtr = false); 

  // Description:
  // Reads a map from int to vector of vtkObject.
  virtual void Serialize(const char* name, 
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >& objs);

protected:
  vtkKWEXMLArchiveReader();
  ~vtkKWEXMLArchiveReader();

private:
  vtkKWEXMLArchiveReader(const vtkKWEXMLArchiveReader&);  // Not implemented.
  void operator=(const vtkKWEXMLArchiveReader&);  // Not implemented.

  void Serialize(vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);
  int ParseStream(istream& str);
  vtkKWEXMLElement* RootElement;

  // Description:
  // weakPtr is true if the object is NOT to be reference counted.
  vtkObject* ReadObject(int id, bool weakPtr);

  // Description:
  // Reads a vtkInformationObject. Note that only keys registered
  // with the vtkKWEInformationKeyMap are restored.
  virtual void Serialize(vtkKWEXMLElement* elem, vtkInformation* info);


  void SetRootElement(vtkKWEXMLElement* re);
  
  vtkKWEXMLArchiveReaderInternals* Internal;
};

#endif
