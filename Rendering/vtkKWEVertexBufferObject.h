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
// .NAME vtkKWEVertexBufferObject 
// .SECTION Description

#ifndef __vtkKWEVertexBufferObject_h
#define __vtkKWEVertexBufferObject_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkRenderWindow;
class vtkOpenGLExtensionManager;
class vtkDataArray;
class vtkCellArray;
class vtkPainterDeviceAdapter;

class VTKEdge_RENDERING_EXPORT vtkKWEVertexBufferObject : public vtkObject
{
public:
  static vtkKWEVertexBufferObject* New();
  vtkTypeRevisionMacro(vtkKWEVertexBufferObject, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the context. Context must be a vtkOpenGLRenderWindow. 
  // This does not increase the reference count of the 
  // context to avoid reference loops.
  // SetContext() may raise an error is the OpenGL context does not support the
  // required OpenGL extensions.
  void SetContext(vtkRenderWindow* context);
  vtkRenderWindow* GetContext();

//BTX
  enum BufferType{
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER 
  };

  // Description:
  // Make the buffer active.
  void Bind(BufferType buffer);
//ETX
  // Description:
  // Inactivate the buffer.
  void UnBind();

  // Description:
  // Upload the data array.
  bool Upload(vtkDataArray* array, BufferType type=ARRAY_BUFFER);
  bool UploadIndices(vtkCellArray* ca, bool build_draw_multielements_arrays=false);
  bool RenderIndices(int vtkprimtype); 

  vtkGetMacro(Initialized, bool);
//BTX
protected:
  vtkKWEVertexBufferObject();
  ~vtkKWEVertexBufferObject();

  // Description:
  // Loads all required OpenGL extensions. Must be called every time a new
  // context is set.
  bool LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr);

  // Description:
  // Create the pixel buffer object.
  void CreateBuffer();

  // Description:
  // Destroys the pixel buffer object.
  void DestroyBuffer();

  vtkWeakPointer<vtkRenderWindow> Context;
  unsigned int BufferTarget; // actually GLenum
  unsigned int Handle;
  bool Initialized;

private:
  vtkKWEVertexBufferObject(const vtkKWEVertexBufferObject&); // Not implemented.
  void operator=(const vtkKWEVertexBufferObject&); // Not implemented.

  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif


