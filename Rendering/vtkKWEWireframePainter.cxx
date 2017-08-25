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
#include "vtkKWEWireframePainter.h"

#include "vtkActor.h"
#include "vtkBoundingBox.h"
#include "vtkKWEColorMaterialHelper.h"
#include "vtkFrameBufferObject.h"
#include "vtkKWELightingHelper.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkTextureObject.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkOpenGLRenderWindow.h"

#include <vtkstd/string>
#include <assert.h>
#include "vtkgl.h"

extern const char* vtkKWEWireframePainter_vs;
extern const char* vtkKWEWireframePainter_fs;

vtkStandardNewMacro(vtkKWEWireframePainter);
vtkCxxRevisionMacro(vtkKWEWireframePainter, "$Revision: 647 $");
#define vtkGetIndex(r,c)    (c*4+r)

inline double vtkClamp(double val, const double& min, const double& max)
{
  val = (val < min)? min : val;
  val = (val > max)? max : val;
  return val;
}

class vtkKWEWireframePainter::vtkInternals
{
public:
  vtkWeakPointer<vtkRenderWindow> LastContext;
  int LastViewportSize[2];
  int ViewportExtent[4];

  vtkSmartPointer<vtkFrameBufferObject> FBO;
  vtkSmartPointer<vtkTextureObject> DepthImage;
  vtkSmartPointer<vtkShaderProgram2> Shader;
  vtkSmartPointer<vtkKWELightingHelper> LightingHelper;
  vtkSmartPointer<vtkKWEColorMaterialHelper> ColorMaterialHelper;

  vtkInternals()
    {
    this->LastViewportSize[0] = this->LastViewportSize[1] = 0;
    this->LightingHelper = vtkSmartPointer<vtkKWELightingHelper>::New();
    this->ColorMaterialHelper = vtkSmartPointer<vtkKWEColorMaterialHelper>::New();
    }

  void ClearTextures()
    {
    this->DepthImage = 0;
    if (this->FBO)
      {
      this->FBO->RemoveAllColorBuffers();
      this->FBO->RemoveDepthBuffer();
      }
    }

  void ClearGraphicsResources()
    {
    this->ClearTextures();
    this->FBO = 0;
    this->DepthImage = 0;
    this->LightingHelper->Initialize(0,VTK_SHADER_TYPE_VERTEX);
    this->ColorMaterialHelper->Initialize(0);
    if(this->Shader!=0)
      {
      this->Shader->ReleaseGraphicsResources();
      this->Shader = 0;
      }
    }
};

//----------------------------------------------------------------------------
vtkKWEWireframePainter::vtkKWEWireframePainter()
{
  this->Internals = new vtkInternals();
}

//----------------------------------------------------------------------------
vtkKWEWireframePainter::~vtkKWEWireframePainter()
{
  this->ReleaseGraphicsResources(this->Internals->LastContext);
  delete this->Internals;
}

//----------------------------------------------------------------------------
void vtkKWEWireframePainter::ReleaseGraphicsResources(vtkWindow* win)
{
  this->Internals->ClearGraphicsResources();
  this->Internals->LastContext = 0;
  
  this->Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
bool vtkKWEWireframePainter::CanRender(vtkRenderer* vtkNotUsed(renderer),
  vtkActor* actor)
{
  return (actor->GetProperty()->GetRepresentation() == VTK_WIREFRAME);
}

//----------------------------------------------------------------------------
void vtkKWEWireframePainter::PrepareForRendering(vtkRenderer* renderer, vtkActor* actor)
{
  if (!this->CanRender(renderer, actor))
    {
    this->Internals->ClearGraphicsResources();
    this->Internals->LastContext = 0;
    this->Superclass::PrepareForRendering(renderer, actor);
    return;
    }

  vtkRenderWindow* renWin = renderer->GetRenderWindow();
  if (this->Internals->LastContext != renWin)
    {
    this->Internals->ClearGraphicsResources();
    }
  this->Internals->LastContext = renWin;

  int viewsize[2], vieworigin[2];
  renderer->GetTiledSizeAndOrigin(&viewsize[0], &viewsize[1], &vieworigin[0], &vieworigin[1]);
  if (this->Internals->LastViewportSize[0] != viewsize[0] || 
    this->Internals->LastViewportSize[1] != viewsize[1])
    {
    // View size has changed, we need to re-generate the textures.
    this->Internals->ClearTextures();
    }
  this->Internals->LastViewportSize[0] = viewsize[0];
  this->Internals->LastViewportSize[1] = viewsize[1];

  if (!this->Internals->FBO)
    {
    vtkFrameBufferObject* fbo = vtkFrameBufferObject::New();
    fbo->SetContext(renWin);
    this->Internals->FBO = fbo;
    fbo->Delete();
    }

  if (!this->Internals->DepthImage)
    {
    vtkTextureObject* depthImage = vtkTextureObject::New();
    depthImage->SetContext(renWin);
    depthImage->Create2D(viewsize[0], viewsize[1], 1, VTK_VOID, false);
    this->Internals->FBO->SetDepthBuffer(depthImage);
    this->Internals->DepthImage = depthImage;
    depthImage->Delete();
    }

  if (!this->Internals->Shader)
    {
    vtkShaderProgram2* pgm = vtkShaderProgram2::New();
    pgm->SetContext(static_cast<vtkOpenGLRenderWindow *>(renWin));
    
    vtkShader2 *s1=vtkShader2::New();
    s1->SetType(VTK_SHADER_TYPE_VERTEX);
    s1->SetSourceCode(vtkKWEWireframePainter_vs);
    s1->SetContext(pgm->GetContext());
    
    vtkShader2 *s2=vtkShader2::New();
    s2->SetType(VTK_SHADER_TYPE_FRAGMENT);
    s2->SetSourceCode(vtkKWEWireframePainter_fs);
    s2->SetContext(pgm->GetContext());
    
    pgm->GetShaders()->AddItem(s1);
    pgm->GetShaders()->AddItem(s2);
    s1->Delete();
    s2->Delete();
    
    this->Internals->LightingHelper->Initialize(pgm,VTK_SHADER_TYPE_VERTEX);
    this->Internals->ColorMaterialHelper->Initialize(pgm);
    this->Internals->Shader = pgm;
    pgm->Delete();
    }

  // Now compute the bounds of the pixels that this dataset is going to occupy
  // on the screen.
  this->Internals->ViewportExtent[0] = vieworigin[0];
  this->Internals->ViewportExtent[1] = vieworigin[0] + viewsize[0];
  this->Internals->ViewportExtent[2] = vieworigin[1];
  this->Internals->ViewportExtent[3] = vieworigin[1] + viewsize[1];
  this->Superclass::PrepareForRendering(renderer, actor);
}


//----------------------------------------------------------------------------
void vtkKWEWireframePainter::RenderInternal(vtkRenderer *renderer,
                                            vtkActor *actor,
                                            unsigned long typeflags,
                                            bool forceCompileOnly)
{
  if (!this->CanRender(renderer, actor))
    {
      this->Superclass::RenderInternal(renderer, actor, typeflags,
                                       forceCompileOnly);
    return;
    }

  vtkRenderWindow* renWin = renderer->GetRenderWindow();

  // Save context state to be able to restore.
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_POLYGON_OFFSET_LINE);
  glDisable(GL_POLYGON_OFFSET_POINT);

  // we get the view port size (not the renderwindow size).
  int viewsize[2], vieworigin[2];
  renderer->GetTiledSizeAndOrigin(&viewsize[0], &viewsize[1], &vieworigin[0], &vieworigin[1]);
  
  // Pass One: Render surface, we are only interested in the depth buffer, hence
  // we don't clear the color buffer. However, color buffer attachment is needed
  // for FBO completeness (verify).
  this->Internals->FBO->StartNonOrtho(viewsize[0], viewsize[1], false);
  glClear(GL_DEPTH_BUFFER_BIT);
  this->Superclass::Superclass::RenderInternal(renderer, actor, typeflags,
                                               forceCompileOnly);
  glFlush();
  this->Internals->FBO->UnBind();

  // Now paste back the rendered image into the default framebuffer.
  renWin->MakeCurrent();
  this->Internals->LightingHelper->PrepareForRendering();
  this->Internals->ColorMaterialHelper->PrepareForRendering();
  
  this->Internals->Shader->Build();
  if(this->Internals->Shader->GetLastBuildStatus()
     !=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("Pass Two failed.");
    abort();
    }
  
  this->Internals->ColorMaterialHelper->Render();
  vtkgl::ActiveTexture(vtkgl::TEXTURE0);
  this->Internals->DepthImage->Bind();
  
  int value=0;
  this->Internals->Shader->GetUniformVariables()->SetUniformi("texDepth",1,&value);
  float fvalues[2];
  fvalues[0]=static_cast<float>(viewsize[0]);
  fvalues[1]=static_cast<float>(viewsize[1]);
  this->Internals->Shader->GetUniformVariables()->SetUniformf("uViewSize",2,fvalues);
  this->Internals->Shader->Use();
   if(!this->Internals->Shader->IsValid())
     {
     vtkErrorMacro(<<" validation of the program failed: "<<this->Internals->Shader->GetLastValidateLog());
     }
   this->Superclass::RenderInternal(renderer, actor, typeflags, forceCompileOnly);  
   this->Internals->Shader->Restore();
   
   // Pop the attributes.
   glPopAttrib();
}

//----------------------------------------------------------------------------
void vtkKWEWireframePainter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

