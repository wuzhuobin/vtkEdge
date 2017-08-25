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

#include "vtkKWEVBOTStripsPainter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkObjectFactory.h"
#include "vtkPainterDeviceAdapter.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkUnsignedCharArray.h"
#include "vtkTriangle.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkKWEVertexBufferObject.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

vtkStandardNewMacro(vtkKWEVBOTStripsPainter);
vtkCxxRevisionMacro(vtkKWEVBOTStripsPainter, "$Revision: 1774 $");

//-----------------------------------------------------------------------------
vtkKWEVBOTStripsPainter::vtkKWEVBOTStripsPainter()
{
  this->PointIDs = vtkKWEVertexBufferObject::New();
  this->PointCoordinates = vtkKWEVertexBufferObject::New();
  this->PointColors = vtkKWEVertexBufferObject::New();
  this->PointNormals = vtkKWEVertexBufferObject::New();
  this->PointTCords = vtkKWEVertexBufferObject::New();
}

//-----------------------------------------------------------------------------
vtkKWEVBOTStripsPainter::~vtkKWEVBOTStripsPainter()
{
  if (this->LastContext)
    {
    this->ReleaseGraphicsResources(this->LastContext);
    this->LastContext = 0;
    }

  this->PointIDs->Delete();
  this->PointCoordinates->Delete();
  this->PointColors->Delete();
  this->PointNormals->Delete();
  this->PointTCords->Delete();
}

//-----------------------------------------------------------------------------
void vtkKWEVBOTStripsPainter::ReleaseGraphicsResources(vtkWindow* w)
{
  if (this->LastContext && this->LastContext.GetPointer() == w)
    {
    this->PointIDs->SetContext(0);
    this->PointCoordinates->SetContext(0);
    this->PointColors->SetContext(0);
    this->PointNormals->SetContext(0);
    this->PointTCords->SetContext(0);
    this->LastContext = 0;
    }

  this->Superclass::ReleaseGraphicsResources(w);
}

//-----------------------------------------------------------------------------
bool vtkKWEVBOTStripsPainter::UpdateGraphicsResources(unsigned long idx,
  vtkDataArray* n,
  vtkUnsignedCharArray* c,
  vtkDataArray* t,
  vtkRenderer* renderer)
{
  bool forceUpdate = false;
  vtkRenderWindow* renWin = renderer->GetRenderWindow();
  if (this->LastContext && this->LastContext != renWin)
    {
    this->ReleaseGraphicsResources(this->LastContext);
    forceUpdate = true;
    }

  this->LastContext = renWin;
  this->PointIDs->SetContext(renWin);
  this->PointCoordinates->SetContext(renWin);
  this->PointColors->SetContext(renWin);
  this->PointNormals->SetContext(renWin);
  this->PointTCords->SetContext(renWin);

  vtkPolyData* pd = this->GetInputAsPolyData();
  vtkPoints* points = pd->GetPoints();
  vtkCellArray* ca = pd->GetStrips();

  if (forceUpdate ||
    !this->PointIDs->GetInitialized() ||
    this->PointIDs->GetMTime() < pd->GetMTime() ||
    this->PointIDs->GetMTime() < ca->GetMTime())
    {
    if (!this->PointIDs->UploadIndices(ca, true))
      {
      return false;
      }
    }

  if (forceUpdate ||
    !this->PointCoordinates->GetInitialized() ||
    this->PointCoordinates->GetMTime() < pd->GetMTime() ||
    this->PointCoordinates->GetMTime() < points->GetMTime())
    {
    if (!this->PointCoordinates->Upload(points->GetData()))
      {
      return false;
      }
    }

  if ((idx & VTK_PDM_NORMALS) && n)
    {
    if (forceUpdate ||
      !this->PointNormals->GetInitialized() ||
      this->PointNormals->GetMTime() < pd->GetMTime() ||
      this->PointNormals->GetMTime() < n->GetMTime())
      {
      if (!this->PointNormals->Upload(n))
        {
        return false;
        }
      }
    }
  else
    {
    // ensures that any allocated buffer are cleared.
    this->PointNormals->SetContext(0);
    }

  if ((idx & VTK_PDM_COLORS) && c && !(idx & VTK_PDM_CELL_COLORS))
    {
    if (forceUpdate ||
      !this->PointColors->GetInitialized() ||
      this->PointColors->GetMTime() < pd->GetMTime() ||
      this->PointColors->GetMTime() < c->GetMTime())
      {
      if (!this->PointColors->Upload(c))
        {
        return false;
        }
      }
    }
  else
    {
    this->PointColors->SetContext(0);
    }

  // transfer texture coords to buffer object
  if (idx & VTK_PDM_TCOORDS && t)
    {
    if (forceUpdate ||
      !this->PointTCords->GetInitialized() ||
      this->PointTCords->GetMTime() < pd->GetMTime() ||
      this->PointTCords->GetMTime() < t->GetMTime())
      {
      if (!this->PointTCords->Upload(t))
        {
        return false;
        }
      }
    }
  else
    {
    this->PointTCords->SetContext(0);
    }

  // everything was transfered O.K.
  return true;
}

//-----------------------------------------------------------------------------
int vtkKWEVBOTStripsPainter::RenderPrimitive(unsigned long idx, vtkDataArray* n,
                                          vtkUnsignedCharArray* c,
                                          vtkDataArray* t, vtkRenderer* ren)
{
  vtkOpenGLRenderWindow* renWin = vtkOpenGLRenderWindow::SafeDownCast(
    ren->GetRenderWindow());

  // anything to draw?
  vtkPolyData* pd = this->GetInputAsPolyData();
  vtkCellArray* ca = pd->GetStrips();
  vtkPoints* points = pd->GetPoints();
  vtkIdType numCells = ca->GetNumberOfCells();
  if (numCells <= 0)
    {
    return 1;
    }

  bool case_handled = true;
  // We don't handle cell attributes or generic attributes. Eventually, we'll
  // support generic attributes.
  if ((idx & VTK_PDM_CELL_NORMALS) ||
    (idx & VTK_PDM_CELL_COLORS) ||
    (idx & VTK_PDM_GENERIC_VERTEX_ATTRIBUTES))
    {
    case_handled = false;
    }

  // build normals is slower using VBOs, pass to delegate
  if (!(idx & (VTK_PDM_NORMALS | VTK_PDM_CELL_NORMALS))
      && this->BuildNormals)
    {
    case_handled = false;
    }

  // check for extension support
  vtkOpenGLExtensionManager* mgr = renWin->GetExtensionManager();
  if (!mgr->ExtensionSupported("GL_VERSION_2_0") &&
    !mgr->ExtensionSupported("GL_ARB_vertex_buffer_object"))
    {
    case_handled = false;
    }

  // generate the VBOs and copy data to the card
  if (case_handled == false ||
    this->UpdateGraphicsResources(idx, n, c, t, ren) == false)
    {
    if (this->LastContext)
      {
      this->ReleaseGraphicsResources(this->LastContext);
      this->LastContext = 0;
      }
    return this->Superclass::RenderPrimitive(idx, n, c, t, ren);
    }

  // enable arrays and client state
  vtkPainterDeviceAdapter* device = ren->GetRenderWindow()->
    GetPainterDeviceAdapter();

  if (this->PointCoordinates->GetInitialized())
    {
    this->PointCoordinates->Bind(vtkKWEVertexBufferObject::ARRAY_BUFFER);
    device->SetAttributePointer(vtkDataSetAttributes::NUM_ATTRIBUTES, 3,
      points->GetDataType(), 0, NULL);
    this->PointCoordinates->UnBind();
    device->EnableAttributeArray(vtkDataSetAttributes::NUM_ATTRIBUTES);
    }

  if (this->PointNormals->GetInitialized())
    {
    this->PointNormals->Bind(vtkKWEVertexBufferObject::ARRAY_BUFFER);
    device->SetAttributePointer(vtkDataSetAttributes::NORMALS, 3,
      n->GetDataType(), 0, NULL);
    this->PointNormals->UnBind();
    device->EnableAttributeArray(vtkDataSetAttributes::NORMALS);
    }

  if (this->PointColors->GetInitialized())
    {
    int numComponents = idx & VTK_PDM_OPAQUE_COLORS ? 3 : 4;
    int dataNumComponents = c->GetNumberOfComponents();
    int stride = (dataNumComponents == 4 && numComponents == 3) ?
      4 * c->GetDataTypeSize() : 0;
    this->PointColors->Bind(vtkKWEVertexBufferObject::ARRAY_BUFFER);
    device->SetAttributePointer(vtkDataSetAttributes::SCALARS,
      numComponents,
      c->GetDataType(), stride, NULL);
    this->PointColors->UnBind();
    device->EnableAttributeArray(vtkDataSetAttributes::SCALARS);
    }

  // tcoords
  if (this->PointTCords->GetInitialized())
    {
    int numComponents = t->GetNumberOfComponents();
    this->PointTCords->Bind(vtkKWEVertexBufferObject::ARRAY_BUFFER);
    device->SetAttributePointer(vtkDataSetAttributes::TCOORDS,
      numComponents, t->GetDataType(), 0, 0);
    this->PointTCords->UnBind();
    device->EnableAttributeArray(vtkDataSetAttributes::TCOORDS);
    }

  this->PointIDs->RenderIndices(VTK_TRIANGLE_STRIP);

  // disable client state
  device->DisableAttributeArray(vtkDataSetAttributes::NUM_ATTRIBUTES);
  device->DisableAttributeArray(vtkDataSetAttributes::NORMALS);
  device->DisableAttributeArray(vtkDataSetAttributes::SCALARS);
  device->DisableAttributeArray(vtkDataSetAttributes::TCOORDS);

  return 1;
}

//-----------------------------------------------------------------------------
void vtkKWEVBOTStripsPainter::PrintSelf(ostream& os ,vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
