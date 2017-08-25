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
// .NAME vtkKWEXMLArchiveWriter - Writes an XML archive to output stream
// .SECTION Description
// Given a vector of vtkObject, vtkKWEXMLArchiveWriter writes
// the object graph in an XML format. Note, to be written an object
// must either be a subclass of vtkSerializableObject or have a helper 
// registered with vtkKWESerializationHelperMap which knows how to 
// serialize the object type.  All objects are written in a flat collection 
// under the root element. All references to these objects are stored using 
// elements of type Pointer. All objects in the input vector are stored under
// a RootObjects element.  For example:
// \code
// <ConceptualModel version="1">
//   <RootObjects>
//     <Item type="Pointer" to_id="1"/>
//   </RootObjects>
//   <Object type="vtkCmbShell" id="1">
//     <Properties type="vtkInformation">
//       <vtkConceptualModelItem::COLOR values="0 0 0 0" length="4"/>
//     </Properties>
//     <Associations type="vtkObjectVectorMap">
//       <Key_1 type="vtkObjectVector">
//         <Item type="Pointer" to_id="2"/>
//       </Key_1>
//     </Associations>
//   </Object>
//   <Object type="vtkCmbModelFaceUse" id="2">
//     <Properties type="vtkInformation">
//       <vtkConceptualModelItem::COLOR values="1 0 0 0" length="4"/>
//     </Properties>
//     <Associations type="vtkObjectVectorMap">
//       <Key_0 type="vtkObjectVector">
//         <Item type="Pointer" to_id="1"/>
//       </Key_0>
//     </Associations>
//   </Object>
// </ConceptualModel>
// \endcode
// .SECTION See Also
// vtkKWESerializer

#ifndef __vtkKWEXMLArchiveWriter_h
#define __vtkKWEXMLArchiveWriter_h

#include "vtkKWESerializer.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include <vtkstd/vector> // Vector of smart pointers
#include "vtkSmartPointer.h" // Vector of smart pointers

//BTX
struct vtkKWEXMLArchiveWriterInternals;
class vtkKWEXMLElement;
//ETX

class VTKEdge_IO_EXPORT vtkKWEXMLArchiveWriter : public vtkKWESerializer
{
public:
  static vtkKWEXMLArchiveWriter *New();
  vtkTypeRevisionMacro(vtkKWEXMLArchiveWriter,vtkKWESerializer);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This method returns true if the serializer is an output
  // serializer (writer). Returns true.
  virtual bool IsWriting() {return true;}

  // Description:
  // This is the main entry point used to write a vector of
  // objects to the XML archive. The rootName is the name
  // of the root XML element in the archive. This name is
  // not used when restoring the archive so it is for information
  // only. For example:
  // \code
  // vtkKWEObjectTreeTransformableNode *root = 
  //   vtkKWEObjectTreeTransformableNode::New();
  // root->SetName("RootNode");
  // 
  // vtkKWEObjectTreeTransformableNode *child = 
  //   vtkKWEObjectTreeTransformableNode::New();
  // root->SetName("Child Node");
  // root->AddChild( child );
  // 
  // vtkSmartPointer<vtkKWEXMLArchiveWriter> writer = 
  //   vtkSmartPointer<vtkKWEXMLArchiveWriter>::New();
  // vtksys_ios::ostringstream ostr;
  // writer->SetArchiveVersion(1);
  // vtkstd::vector<vtkSmartPointer<vtkObject> > objs;
  // objs.push_back(root);
  // writer->Serialize(ostr, "ObjectTree", objs);
  // \endcode
  virtual void Serialize(ostream& ostr, const char* rootName, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);

  // Description:
  // Additional entry point, used to write a vector of
  // objects to the XML archive, in the form of a vtkKWEXMLElement which
  // is assumed to have been allocated by the caller.
  virtual void Serialize(vtkKWEXMLElement* elem, const char* rootName, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);

  // Description:
  // Additional entry point, used to write a single (root) object
  // to the XML archive, in the form of a vtkKWEXMLElement which
  // is assumed to have been allocated by the caller.
  virtual void Serialize(vtkKWEXMLElement* elem, const char* rootName, 
    vtkObject *objs);

  // Description:
  // Serializes a single integer.
  virtual void Serialize(const char* name, int& val);

  // Description:
  // Serializes an array.
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
  // Serializes a single double.
  virtual void Serialize(const char* name, double& val);
  
  // Description:
  // Serializes an array.
  virtual void Serialize(const char* name, double*& val, unsigned int& length);

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, char*& str);

  // Description:
  // Serializes a string.
  virtual void Serialize(const char* name, vtkstd::string& str);

  // Description:
  // Serializes a vtkObject.  Note, the object must either be subclass of
  // vtkKWESerializableObject or have a helper registered with 
  // vtkKWESerializableObjectManager which knows how to serialize the object 
  // type. The weakPtr parameter is actually ignored (we don't write an 
  // attribute indicating it is weak, if it is) because the reading code will
  // have the same parameter specifying the pointer is weak. Here for symmetry.
  virtual void Serialize(const char* name, vtkObject*& object, bool weakPtr = false);

  // Description:
  // Serializes a vtkInformationObject.
  virtual void Serialize(const char* name, vtkInformation* info);

  // Description:
  // Serializes a vector of vtkObjects.  The weakPtr parameter
  // is actually ignored (we don't write an attribute indicating it is weak,
  // if it is) because the reading code will have the same paremter specifying
  // the  pointer is weak.  Here for symmetry.
  virtual void Serialize(const char* name, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs, 
    bool weakPtr = false);

  // Description:
  // Serializes a map from int to vector of vtkObjects.
  virtual void Serialize(const char* name, 
    vtkstd::map<int, vtkstd::vector<vtkSmartPointer<vtkObject> > >& objs);

protected:
  vtkKWEXMLArchiveWriter();
  ~vtkKWEXMLArchiveWriter();

  virtual unsigned int Serialize(vtkObject*& obj);
  
private:
  vtkKWEXMLArchiveWriter(const vtkKWEXMLArchiveWriter&);  // Not implemented.
  void operator=(const vtkKWEXMLArchiveWriter&);  // Not implemented.

  virtual void CreateDOM(const char* rootName, 
    vtkstd::vector<vtkSmartPointer<vtkObject> >& objs);

  void SetRootElement(vtkKWEXMLElement*);

  // Description:
  // Serializes a vtkInformationObject.
  virtual void Serialize(vtkKWEXMLElement* elem, vtkInformation* info);

  vtkKWEXMLArchiveWriterInternals* Internal;
  vtkKWEXMLElement* RootElement;
};

#endif
