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


