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
#include "vtkKWEVertexBufferObject.h"

#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkPainterDeviceAdapter.h"

#include <vtkgl.h>
//---------------------------------------------------------------------------
class vtkKWEVertexBufferObject::vtkInternals
{
public:
  const GLvoid **MDEIndexOffsets;
  GLsizei *MDECounts;
  GLsizei MDEPrimCount;

  void Init()
    {
    this->MDEPrimCount = 0;
    this->MDECounts = 0;
    this->MDEIndexOffsets = 0;
    }

  void Clear()
    {
    delete [] this->MDECounts;
    delete [] this->MDEIndexOffsets;
    this->Init();
    }
  vtkInternals()
    {
    this->Init();
    }
  ~vtkInternals()
    {
    this->Clear();
    }
};


vtkStandardNewMacro(vtkKWEVertexBufferObject);
vtkCxxRevisionMacro(vtkKWEVertexBufferObject, "$Revision: 808 $");
//----------------------------------------------------------------------------
vtkKWEVertexBufferObject::vtkKWEVertexBufferObject()
{
  this->Internals = new vtkKWEVertexBufferObject::vtkInternals();
  this->Handle = 0;
  this->Context = 0;
  this->BufferTarget = 0;
  this->Initialized = false;
}

//----------------------------------------------------------------------------
vtkKWEVertexBufferObject::~vtkKWEVertexBufferObject()
{
  this->SetContext(0);
  delete this->Internals;
}

//----------------------------------------------------------------------------
bool vtkKWEVertexBufferObject::LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr)
{
 return (mgr->LoadSupportedExtension("GL_VERSION_2_0") &&
    mgr->LoadSupportedExtension("GL_VERSION_1_5") &&
    mgr->LoadSupportedExtension("GL_ARB_vertex_buffer_object"));
}

//----------------------------------------------------------------------------
vtkRenderWindow* vtkKWEVertexBufferObject::GetContext()
{
  return this->Context;
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::SetContext(vtkRenderWindow* renWin)
{
  if (this->Context == renWin)
    {
    return;
    }

  this->DestroyBuffer();
  vtkOpenGLRenderWindow* openGLRenWin = vtkOpenGLRenderWindow::SafeDownCast(renWin);
  this->Context = openGLRenWin;
  if (openGLRenWin)
    {
    if (!this->LoadRequiredExtensions(openGLRenWin->GetExtensionManager()))
      {
      this->Context = 0;
      vtkErrorMacro("Required OpenGL extensions not supported by the context.");
      }
    }

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::CreateBuffer()
{
  this->Context->MakeCurrent();
  if (!this->Handle)
    {
    GLuint ioBuf;
    vtkgl::GenBuffers(1, &ioBuf);
    vtkGraphicErrorMacro(this->Context, "Generate vertex buffer");
    this->Handle = static_cast<unsigned int>(ioBuf);
    this->Initialized = false;
    }
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::DestroyBuffer()
{
  if (this->Context && this->Handle)
    {
    GLuint ioBuf = static_cast<GLuint>(this->Handle);
    vtkgl::DeleteBuffers(1, &ioBuf);
    this->Initialized = false;
    }
  this->Handle = 0;
  this->Internals->Clear();
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::Bind(BufferType type)
{
  if (!this->Context)
    {
    vtkErrorMacro("No context specified. Cannot bind.");
    return;
    }

  if (!this->Handle)
    {
    vtkErrorMacro("A vertex buffer can be bound only after it has been created."
      " Are you sure you uploaded some data?");
    return;
    }

  GLenum target = this->BufferTarget;
  switch (type)
    {
  case ARRAY_BUFFER:
    target =  vtkgl::ARRAY_BUFFER;
    break;

  case ELEMENT_ARRAY_BUFFER:
    target = vtkgl::ELEMENT_ARRAY_BUFFER;
    break;
    }

  if (this->BufferTarget && this->BufferTarget != target)
    {
    this->UnBind();
    }

  this->BufferTarget = target;
  vtkgl::BindBuffer(this->BufferTarget,
                    this->Handle);
  vtkGraphicErrorMacro(this->Context,"Post Bind");
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::UnBind()
{
  if (this->Context && this->Handle && this->BufferTarget)
    {
    vtkgl::BindBuffer(this->BufferTarget, 0);
    vtkGraphicErrorMacro(this->Context,"Post UnBind");
    this->BufferTarget = 0;
    }
}

//----------------------------------------------------------------------------
bool vtkKWEVertexBufferObject::Upload(vtkDataArray* data, BufferType type)
{
  this->Internals->Clear();
  if (!this->Context)
    {
    vtkErrorMacro("No context specified. Cannot upload data.");
    return false;
    }
  // TODO: Handle double data.
  this->CreateBuffer();
  this->Bind(type);
  vtkgl::BufferData(this->BufferTarget,
    data->GetDataTypeSize() * data->GetNumberOfTuples() * data->GetNumberOfComponents(),
    data->GetVoidPointer(0), vtkgl::STATIC_DRAW); // TODO: Try stream draw
  vtkGraphicErrorMacro(this->Context,"Post Upload");
  this->UnBind();
  this->Initialized = true;

  this->Modified();
  return true;
}

//----------------------------------------------------------------------------
bool vtkKWEVertexBufferObject::UploadIndices(vtkCellArray* ca,
  bool build_draw_multielements_arrays/*=false*/)
{
  this->Internals->Clear();

  if (!this->Context)
    {
    vtkErrorMacro("No context specified. Cannot upload data.");
    return false;
    }

  this->CreateBuffer();
  this->Bind(ELEMENT_ARRAY_BUFFER);

  vtkIdType numCells = ca->GetNumberOfCells();
  vtkIdType numIndices = (ca->GetNumberOfConnectivityEntries() - numCells);
  
  // TODO: try to use minimum data size.
  vtkgl::BufferData(this->BufferTarget,
                    static_cast<vtkgl::GLsizeiptr>(
                      static_cast<size_t>(numIndices)*sizeof(GLuint)),    
                    NULL,
                    vtkgl::STATIC_DRAW); // TODO: Try stream draw 

  GLuint *vboPtr = reinterpret_cast<GLuint*>(
    vtkgl::MapBuffer(vtkgl::ELEMENT_ARRAY_BUFFER, vtkgl::WRITE_ONLY));


  if (build_draw_multielements_arrays)
    {
    this->Internals->MDECounts = new GLsizei[numCells];
    this->Internals->MDEIndexOffsets = new const GLvoid*[numCells];
    this->Internals->MDEPrimCount = static_cast<GLsizei>(numCells);
    }

  size_t currentBufferOffset = 0;
  vtkIdType* caData = ca->GetPointer();
  for (vtkIdType kk=0; kk < numCells; ++kk)
    {
    vtkIdType numPoints = *(caData);
    caData++;
    for (vtkIdType cc=0; cc < numPoints; ++cc)
      {
      *vboPtr = static_cast<GLuint>(*caData);
      caData++;
      vboPtr++;
      }
    if (build_draw_multielements_arrays)
      {
      this->Internals->MDECounts[kk] = static_cast<GLsizei>(numPoints);
      this->Internals->MDEIndexOffsets[kk] = reinterpret_cast<GLvoid*>(currentBufferOffset);
      }
    currentBufferOffset += (static_cast<size_t>(numPoints) * sizeof(GLuint));
    }
  vtkgl::UnmapBuffer(vtkgl::ELEMENT_ARRAY_BUFFER);
  this->UnBind();
  this->Initialized = true;
  this->Modified();
  return true;
}

static const GLenum VTK2OpenGLPrimitive[] = {
  static_cast<GLenum>(0xFFFF),       // 0 - VTK_EMPTY_CELL
  GL_POINTS,            // 1 - VTK_VERTEX
  GL_POINTS,            // 2 - VTK_POLY_VERTEX
  GL_LINES,             // 3 - VTK_LINE
  GL_LINE_STRIP,        // 4 - VTK_POLY_LINE
  GL_TRIANGLES,         // 5 - VTK_TRIANGLE
  GL_TRIANGLE_STRIP,    // 6 - VTK_TRIANGLE_STRIP
  GL_POLYGON,           // 7 - VTK_POLYGON
  static_cast<GLenum>(0xFFFF),       // 8 - VTK_PIXEL
  GL_QUADS,             // 9 - VTK_QUAD
  GL_LINE_LOOP          // 10 - VTK_TETRA
};

//----------------------------------------------------------------------------
bool vtkKWEVertexBufferObject::RenderIndices(int vtkprimtype)
{
  if (vtkprimtype <= 0 || vtkprimtype >= VTK_TETRA)
    {
    vtkErrorMacro("Unsupported primitive: " << vtkprimtype);
    return false;
    }

  if (this->Initialized && this->Internals->MDEPrimCount > 0)
    {
    this->Bind(ELEMENT_ARRAY_BUFFER);
    vtkgl::MultiDrawElements(VTK2OpenGLPrimitive[vtkprimtype],
      this->Internals->MDECounts,
      GL_UNSIGNED_INT,
      this->Internals->MDEIndexOffsets,
      this->Internals->MDEPrimCount);
    this->UnBind();
    return true;
    }

  return false;
}

//----------------------------------------------------------------------------
void vtkKWEVertexBufferObject::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

