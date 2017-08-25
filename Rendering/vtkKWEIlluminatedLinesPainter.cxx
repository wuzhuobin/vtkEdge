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
#include "vtkKWEIlluminatedLinesPainter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkChooserPainter.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkLightingHelper.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkPainterDeviceAdapter.h"
#include "vtkPainterPolyDataMapper.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkUnsignedCharArray.h"

#include "vtkgl.h"
extern const char* vtkKWEIlluminatedLinesPainter_vs;
extern const char* vtkKWEIlluminatedLinesPainter_fs;

vtkStandardNewMacro(vtkKWEIlluminatedLinesPainter);
vtkCxxRevisionMacro(vtkKWEIlluminatedLinesPainter, "$Revision: 1973 $");
vtkInformationKeyMacro(vtkKWEIlluminatedLinesPainter, ENABLE, Integer);
//-----------------------------------------------------------------------------
vtkKWEIlluminatedLinesPainter::vtkKWEIlluminatedLinesPainter()
{
  this->Enable = 1;
  this->Program = 0;
  this->FragmentShader=0;
  this->SetSupportedPrimitive(vtkPainter::LINES | vtkPainter::POLYS);
}

//-----------------------------------------------------------------------------
vtkKWEIlluminatedLinesPainter::~vtkKWEIlluminatedLinesPainter()
{
  if (this->LastRenderWindow)
    {
    vtkSmartPointer<vtkWindow> lastWindow =
      this->LastRenderWindow.GetPointer();
    this->ReleaseGraphicsResources(this->LastRenderWindow);
    }

  if(this->FragmentShader!=0)
    {
    this->FragmentShader->Delete();
    this->FragmentShader=0;
    }

  if (this->Program)
    {
    this->Program->Delete();
    this->Program = 0;
    }
}

//-----------------------------------------------------------------------------
void vtkKWEIlluminatedLinesPainter::ReleaseGraphicsResources(vtkWindow* win)
{
  if (this->LastRenderWindow == win)
    {
    if(this->FragmentShader!=0)
      {
      this->FragmentShader->ReleaseGraphicsResources();
      this->FragmentShader->Delete();
      this->FragmentShader=0;
      }
    if (this->Program)
      {
      this->Program->ReleaseGraphicsResources();
      this->Program->Delete();
      this->Program = 0;
      }
    this->LastRenderWindow = 0;
    }

  this->Superclass::ReleaseGraphicsResources(win);
}

//-----------------------------------------------------------------------------
void vtkKWEIlluminatedLinesPainter::ProcessInformation(vtkInformation* info)
{
  if (info->Has(vtkKWEIlluminatedLinesPainter::ENABLE()))
    {
    this->SetEnable(info->Get(vtkKWEIlluminatedLinesPainter::ENABLE()));
    }
  else
    {
    this->SetEnable(1);
    }
  this->Superclass::ProcessInformation(info);
}

//-----------------------------------------------------------------------------
bool vtkKWEIlluminatedLinesPainter::IsSupported(vtkRenderWindow* renWin)
{
  return vtkShaderProgram2::IsSupported(
    static_cast<vtkOpenGLRenderWindow *>(renWin));
}

//-----------------------------------------------------------------------------
void vtkKWEIlluminatedLinesPainter::PrepareForRendering(vtkRenderer* renderer, vtkActor* actor)
{
  if (this->LastRenderWindow.GetPointer() != renderer->GetRenderWindow())
    {
    vtkSmartPointer<vtkWindow> lastWindow = this->LastRenderWindow.GetPointer();
    this->ReleaseGraphicsResources(lastWindow);
    }

  if (!this->LastRenderWindow)
    {
    vtkOpenGLRenderWindow* renWin = vtkOpenGLRenderWindow::SafeDownCast(
      renderer->GetRenderWindow());
    this->LastRenderWindow = renWin;
    vtkOpenGLExtensionManager* mgr = renWin->GetExtensionManager();
    if (mgr->LoadSupportedExtension("GL_VERSION_1_3"))
      {
      this->Program = vtkShaderProgram2::New();
      this->Program->SetContext(static_cast<vtkOpenGLRenderWindow *>(renderer->GetRenderWindow()));

      vtkShader2 *s1=vtkShader2::New();
      s1->SetType(VTK_SHADER_TYPE_VERTEX);
      s1->SetSourceCode(vtkKWEIlluminatedLinesPainter_vs);
      s1->SetContext(this->Program->GetContext());
      this->Program->GetShaders()->AddItem(s1);
      s1->Delete();

      vtkShader2 *s2=vtkShader2::New();
      s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
      s2->SetSourceCode(vtkKWEIlluminatedLinesPainter_fs);
      s2->SetContext(this->Program->GetContext());
      this->Program->GetShaders()->AddItem(s2);
      this->FragmentShader=s2;

#if 0

      this->Program->Build();
  if(this->Program->GetLastBuildStatus()!=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("Program bind failed.");
//    return 0;
    }

  int value=0;
  this->Program->GetUniformVariables()->SetUniformi("tex",1,&value);
  value=1;
  this->Program->GetUniformVariables()->SetUniformi("uMode",1,&value);

  this->Program->Use();
  if(!this->Program->IsValid())
    {
    vtkErrorMacro(<<" validation of the program failed: "<<this->Program->GetLastValidateLog());
    }
  this->Program->Restore();
  cout << "done" << endl;


#endif


      }
    else
      {
      vtkErrorMacro("OpenGL 1.3 support is required.");
      }
    }

  this->Superclass::PrepareForRendering(renderer, actor);
}

template <class T>
inline void DirectionVector(const T p1[3], const T p2[3], T vector[3])
{
  vector[0] = static_cast<T>(p2[0] - p1[0]);
  vector[1] = static_cast<T>(p2[1] - p1[1]);
  vector[2] = static_cast<T>(p2[2] - p1[2]);
}

inline void sendTCoords(int type, const void* ptr, unsigned long offset)
{
  switch (type)
    {
  case VTK_SHORT:
    vtkgl::MultiTexCoord3sv(vtkgl::TEXTURE1, static_cast<const GLshort*>(ptr)+offset);
    break;

  case VTK_INT:
    vtkgl::MultiTexCoord3iv(vtkgl::TEXTURE1, static_cast<const GLint*>(ptr)+offset);
    break;

  case VTK_FLOAT:
    vtkgl::MultiTexCoord3fv(vtkgl::TEXTURE1, static_cast<const GLfloat*>(ptr)+offset);
    break;

  case VTK_DOUBLE:
    vtkgl::MultiTexCoord3dv(vtkgl::TEXTURE1, static_cast<const GLdouble*>(ptr)+offset);
    break;
    }
}

template <class T>
void sendPrevTCoords(int ptype, T* points, vtkIdType *ptId)
{
  const T* cur = static_cast<T*>(points) + (3**ptId);
  const T* next = static_cast<T*>(points) +(3**(ptId+1));
  T prev[3];
  ::DirectionVector(next, cur, prev);
  prev[0] = static_cast<T>(cur[0] + prev[0]);
  prev[1] = static_cast<T>(cur[1] + prev[1]);
  prev[2] = static_cast<T>(cur[2] + prev[2]);

  sendTCoords(ptype, prev, 0);
}

inline void sendPrevTCoords(int ptype, void* points, vtkIdType *ptIds)
{
  switch (ptype)
    {
  case VTK_SHORT:
    sendPrevTCoords(ptype, static_cast<short*>(points), ptIds);
    break;

  case VTK_INT:
    sendPrevTCoords(ptype, static_cast<int*>(points), ptIds);
    break;

  case VTK_FLOAT:
    sendPrevTCoords(ptype, static_cast<float*>(points), ptIds);
    break;

  case VTK_DOUBLE:
    sendPrevTCoords(ptype, static_cast<double*>(points), ptIds);
    break;
    }
}


//-----------------------------------------------------------------------------
// We need to put in the previous(or next) point as the tcoord, so that the
// shader program can compute tangents.
#define vtkDrawPrimsMacro(prim,glVertFuncs,glInitFuncs) \
{ \
  vtkIdType nPts; unsigned short count = 0; \
  glInitFuncs \
  while (ptIds < endPtIds) \
    { \
    nPts = *ptIds; \
    ++ptIds; \
    if (nPts == 0)  \
      { \
      continue; \
      } \
    device->BeginPrimitive(prim);\
    if (nPts < 2)\
      { \
      sendTCoords(ptype, points, 3**ptIds); \
      abort(); \
      } \
    else  \
      { \
      sendPrevTCoords(ptype, points, ptIds);\
      } \
    glVertFuncs \
    ++ptIds; \
    --nPts; \
    while (nPts > 0) \
      { \
      sendTCoords(ptype, points, 3*(*ptIds-1));\
      glVertFuncs \
      ++ptIds; \
      --nPts; \
      } \
    device->EndPrimitive();\
    if (++count == 10000) \
      { \
      cellNum += 10000; \
      count = 0; \
      this->UpdateProgress(static_cast<double>(cellNum-cellNumStart)/static_cast<double>(totalCells)); \
      if (ren->GetRenderWindow()->CheckAbortStatus()) \
        { \
        break; \
        } \
      } \
    } \
  cellNum += count; \
}

//-----------------------------------------------------------------------------
int vtkKWEIlluminatedLinesPainter::RenderPrimitive(unsigned long idx, vtkDataArray* n,
    vtkUnsignedCharArray* c, vtkDataArray* t, vtkRenderer* ren)
{
  if (!this->Enable || !this->Program)
    {
    return this->Superclass::RenderPrimitive(idx, n, c, t, ren);
    }

  vtkPolyData* pd = this->GetInputAsPolyData();
  vtkPoints* p = pd->GetPoints();
  vtkCellArray* ca = (this->RenderPolys)? pd->GetPolys() : pd->GetLines();
  vtkIdType cellNum = pd->GetVerts()->GetNumberOfCells();
  vtkIdType cellNumStart = cellNum;
  vtkIdType totalCells = ca->GetNumberOfCells();

  vtkPainterDeviceAdapter* device = ren->GetRenderWindow()->
    GetPainterDeviceAdapter();
  void *points = p->GetVoidPointer(0);
  void *tcoords = 0;
  unsigned char *colors = 0;
  if (ca->GetNumberOfCells() == 0)
    {
    return 1;
    }

  if (t)
    {
    tcoords = t->GetVoidPointer(0);
    }
  if (c)
    {
    colors = c->GetPointer(0);
    }
  vtkIdType *ptIds = ca->GetPointer();
  vtkIdType *endPtIds = ptIds + ca->GetNumberOfConnectivityEntries();
  int ptype = p->GetDataType();
  int ttype = (t)? t->GetDataType() : 0;
  int tcomps = (t)? t->GetNumberOfComponents() : 0;
  int primitive = (this->RenderPolys)? VTK_TETRA : VTK_POLY_LINE;

  // since this painter does not deal with field colors specially,
  // we just ignore the flag.
  idx &= (~static_cast<unsigned long>(VTK_PDM_FIELD_COLORS));
  idx &= (~static_cast<unsigned long>(VTK_PDM_NORMALS));

  //::vtkUpdateLights();
  vtkLightingHelper* lhelper = vtkLightingHelper::New();
  lhelper->PrepareForRendering();
  lhelper->Delete();

  int mode = 0;
  if (glIsEnabled(GL_COLOR_MATERIAL))
    {
    GLint colorMaterialParameter;
    glGetIntegerv(GL_COLOR_MATERIAL_PARAMETER, &colorMaterialParameter);
    switch (colorMaterialParameter)
      {
    case GL_AMBIENT:
      mode = 1;
      break;

    case GL_DIFFUSE:
      mode = 2;
      break;

    case GL_SPECULAR:
      mode = 3;
      break;

    case GL_AMBIENT_AND_DIFFUSE:
      mode = 4;
      break;

    case GL_EMISSION:
      mode = 5;
      break;
      }
    }

  int handled = 1;
  // draw all the elements, use fast path if available
#if 1
  if (t)
    {
    if(this->Program->GetShaders()->IsItemPresent(this->FragmentShader)==0)
      {
      this->Program->GetShaders()->AddItem(this->FragmentShader);
      }
    }
  else
    {
    int location=
      this->Program->GetShaders()->IsItemPresent(this->FragmentShader);
     if(location>0)
      {
      this->Program->GetShaders()->RemoveItem(location-1);
      }
    }
#endif

  this->Program->Build();
  if(this->Program->GetLastBuildStatus()!=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("Program bind failed.");
    return 0;
    }

  int value=0;
  this->Program->GetUniformVariables()->SetUniformi("tex",1,&value);
  value=mode;
  this->Program->GetUniformVariables()->SetUniformi("uMode",1,&value);

  this->Program->Use();
  if(!this->Program->IsValid())
    {
    vtkErrorMacro(<<" validation of the program failed: "<<this->Program->GetLastValidateLog());
    }

  switch (idx)
    {
    case 0:
      vtkDrawPrimsMacro(primitive,
        device->SendAttribute(vtkPointData::NUM_ATTRIBUTES, 3,
          ptype, points, 3**ptIds);,;);
      break;

    case VTK_PDM_COLORS:
      vtkDrawPrimsMacro(primitive,
        device->SendAttribute(vtkPointData::SCALARS, 4,
          VTK_UNSIGNED_CHAR, colors + 4**ptIds);
        device->SendAttribute(vtkPointData::NUM_ATTRIBUTES, 3,
          ptype, points, 3**ptIds);,;);
      break;

    case VTK_PDM_COLORS | VTK_PDM_OPAQUE_COLORS:
      vtkDrawPrimsMacro(primitive,
        device->SendAttribute(vtkPointData::SCALARS, 3,
          VTK_UNSIGNED_CHAR, colors + 4**ptIds);
        device->SendAttribute(vtkPointData::NUM_ATTRIBUTES, 3,
          ptype, points, 3**ptIds);,;);
      break;

    case VTK_PDM_TCOORDS:
      vtkDrawPrimsMacro(primitive,
        device->SendAttribute(vtkPointData::TCOORDS, tcomps,
          ttype, tcoords, tcomps**ptIds);
        device->SendAttribute(vtkPointData::NUM_ATTRIBUTES, 3,
          ptype, points, 3**ptIds);,;);
    break;

    default:
      handled = 0; // let the delegate painter handle this call.
      vtkErrorMacro("Case not handled. Illumination disabled.");
    }

  this->Program->Restore();
  return handled;
}

//-----------------------------------------------------------------------------
void vtkKWEIlluminatedLinesPainter::UpdateMapper(vtkPainterPolyDataMapper* mapper)
{
  vtkPainter* painter = mapper->GetPainter();
  vtkChooserPainter* chooser = vtkChooserPainter::SafeDownCast(painter);
  while (painter && !chooser)
    {
    painter = painter->GetDelegatePainter();
    chooser = vtkChooserPainter::SafeDownCast(painter);
    }
  if (chooser)
    {
    vtkKWEIlluminatedLinesPainter* lines = vtkKWEIlluminatedLinesPainter::New();
    chooser->SetLinePainter(lines);
    chooser->UseLinesPainterForWireframesOn();
    lines->Delete();
    }
}

//-----------------------------------------------------------------------------
void vtkKWEIlluminatedLinesPainter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
