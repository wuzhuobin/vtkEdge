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
#include "vtkKWELineIntergralConvolution2D.h"

#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkFrameBufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkTextureObject.h"
#include "vtkTimerLog.h"

#include <vtkstd/string>

extern const char *vtkKWELineIntergralConvolution2D_fs;
extern const char *vtkKWELineIntergralConvolution2D_fs1;
extern const char *vtkKWELineIntergralConvolution2D_fs2;

#include "vtkgl.h"

static const char* vtkKWELineIntergralConvolution2DCode = 
// $,$ are replaced with [x,y,z,w]
"vec2 getSelectedComponents(vec4 color)"
"{"
"  return color.$$;"
"}";

//#define VTKKWE_LICDEBUGON

#ifdef VTKKWE_LICDEBUGON
#define vtkKWELICDebug(x) cout << x << endl;
#else
#define vtkKWELICDebug(x)
#endif

inline int MAX(int a, int b)
{
  return (a > b)? a : b;
}

inline int MIN(int a, int b)
{
  return (a < b)? a : b;
}
vtkStandardNewMacro(vtkKWELineIntergralConvolution2D);
vtkCxxRevisionMacro(vtkKWELineIntergralConvolution2D, "$Revision: 808 $");

void vtkRenderQuad(double vTCoords[4], double licTCoords[4],
  unsigned int width, unsigned int height)
{
  glBegin(GL_QUADS);
  
  glTexCoord2f(static_cast<GLfloat>(licTCoords[0]),
               static_cast<GLfloat>(licTCoords[2]));
  vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, static_cast<GLfloat>(vTCoords[0]),
                         static_cast<GLfloat>(vTCoords[2]));
  //vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, 0, 0);
  glVertex2f(0, 0);
  
  glTexCoord2f(static_cast<GLfloat>(licTCoords[1]),
               static_cast<GLfloat>(licTCoords[2]));
  vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, static_cast<GLfloat>(vTCoords[1]),
                         static_cast<GLfloat>(vTCoords[2]));
  //vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, 1, 0);
  glVertex2f(static_cast<GLfloat>(width), 0);
  
  glTexCoord2f(static_cast<GLfloat>(licTCoords[1]),
               static_cast<GLfloat>(licTCoords[3]));
  vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, static_cast<GLfloat>(vTCoords[1]),
                         static_cast<GLfloat>(vTCoords[3]));
 // vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, 1, 1);
  glVertex2f(static_cast<GLfloat>(width), static_cast<GLfloat>(height));
  
  glTexCoord2f(static_cast<GLfloat>(licTCoords[0]),
               static_cast<GLfloat>(licTCoords[3]));
  vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, static_cast<GLfloat>(vTCoords[0]),
                         static_cast<GLfloat>(vTCoords[3]));
  //vtkgl::MultiTexCoord2f(vtkgl::TEXTURE1, 0, 1);
  glVertex2f(0, static_cast<GLfloat>(height));

  glEnd();
}

void vtkRenderQuad(unsigned int extent[4], unsigned int width, unsigned int height)
{
  double minX = extent[0];
  double maxX = extent[1];
  double minY = extent[2];
  double maxY = extent[3];

  glBegin(GL_QUADS);
  glTexCoord2f(static_cast<GLfloat>(minX/width),
               static_cast<GLfloat>(minY/height));
  glVertex2f(static_cast<GLfloat>(minX), static_cast<GLfloat>(minY));
  glTexCoord2f(static_cast<GLfloat>(maxX/width),
               static_cast<GLfloat>(minY/height));
  glVertex2f(static_cast<GLfloat>(maxX), static_cast<GLfloat>(minY));
  glTexCoord2f(static_cast<GLfloat>(maxX/width),
               static_cast<GLfloat>(maxY/height));
  glVertex2f(static_cast<GLfloat>(maxX), static_cast<GLfloat>(maxY));
  glTexCoord2f(static_cast<GLfloat>(minX/width),
               static_cast<GLfloat>(maxY/height));
  glVertex2f(static_cast<GLfloat>(minX),
             static_cast<GLfloat>(maxY));
  glEnd();
}


#define RENDERQUAD vtkRenderQuad(vTCoords, licTCoords, outWidth, outHeight); 
// #define RENDERQUAD vtkRenderQuad(extent, inWidth, inHeight); 


// ----------------------------------------------------------------------------
vtkKWELineIntergralConvolution2D::vtkKWELineIntergralConvolution2D()
{
  this->Magnification = 1;
  this->TransformVectors = 1;
  this->VectorField=0;
  this->Noise=0;
  this->LIC=0;
  this->Steps=1;
  this->StepSize= 0.01;
  this->Components[0] = 0;
  this->Components[1] = 1;
  this->Spacing[0] = this->Spacing[1] = 1.0;
  this->Shift = 0.0;
  this->Scale = 1.0;
}

// ----------------------------------------------------------------------------
vtkKWELineIntergralConvolution2D::~vtkKWELineIntergralConvolution2D()
{
  if(this->VectorField!=0)
    {
    this->VectorField->Delete();
    }
   if(this->Noise!=0)
    {
    this->Noise->Delete();
    }
   if(this->LIC!=0)
     {
     this->LIC->Delete();
    }
}

// ----------------------------------------------------------------------------
void vtkKWELineIntergralConvolution2D::SetVectorField(vtkTextureObject *vectorField)
{
  vtkSetObjectBodyMacro(VectorField, vtkTextureObject, vectorField);
}

// ----------------------------------------------------------------------------
void vtkKWELineIntergralConvolution2D::SetNoise(vtkTextureObject *noise)
{
  vtkSetObjectBodyMacro(Noise, vtkTextureObject,noise);
}

// ----------------------------------------------------------------------------
void vtkKWELineIntergralConvolution2D::SetLIC(vtkTextureObject *lic)
{
  vtkSetObjectBodyMacro(LIC, vtkTextureObject,lic);
}

// ----------------------------------------------------------------------------
int vtkKWELineIntergralConvolution2D::Execute()
{
  unsigned int extent[4]={0, 0, 0, 0};
  extent[1] = this->VectorField->GetWidth()-1;
  extent[3] = this->VectorField->GetHeight()-1;
  return this->Execute(extent);
}

#define UNIFORM1i(name, val)\
{\
  int uvar=pgm->GetUniformLocation(name);\
  vtkGraphicErrorMacro(context, "Get uniform " name);\
  if(uvar==-1)\
    {\
    vtkErrorMacro(<< name << " is not a uniform variable.");\
    }\
  else\
    {\
    vtkgl::Uniform1i(uvar, val);\
    vtkGraphicErrorMacro(context, "Setting " name);\
    }\
}

#define UNIFORM1f(name, val)\
{\
  int uvar=pgm->GetUniformLocation(name);\
  vtkGraphicErrorMacro(context, "Get uniform " name);\
  if(uvar==-1)\
    {\
    vtkErrorMacro(<< name << " is not a uniform variable.");\
    }\
  else\
    {\
    vtkgl::Uniform1f(uvar, val);\
    vtkGraphicErrorMacro(context, "Setting " name);\
    }\
}

#define UNIFORM2f(name, val0, val1)\
{\
  int uvar=pgm->GetUniformLocation(name);\
  vtkGraphicErrorMacro(context, "Get uniform " name);\
  if(uvar==-1)\
    {\
    vtkErrorMacro(<< name << " is not a uniform variable.");\
    }\
  else\
    {\
    vtkgl::Uniform2f(uvar, val0, val1);\
    vtkGraphicErrorMacro(context, "Setting " name);\
    }\
}


// ----------------------------------------------------------------------------
int vtkKWELineIntergralConvolution2D::Execute(int extent[4])
{
  unsigned int uiExtent[4];
  for (int kk=0; kk < 4; kk++)
    {
    if (extent[kk] <0)
      {
      vtkErrorMacro("Invalid input extent.");
      return 0;
      }
    uiExtent[kk] = static_cast<unsigned int>(extent[kk]);
    }
  return this->Execute(uiExtent);
}

// ----------------------------------------------------------------------------
// Description:
// Returns if the context supports the required extensions.
bool vtkKWELineIntergralConvolution2D::IsSupported(vtkRenderWindow* renWin)
{
  return vtkTextureObject::IsSupported(renWin) &&
    vtkFrameBufferObject::IsSupported(renWin) &&
    vtkShaderProgram2::IsSupported(static_cast<vtkOpenGLRenderWindow *>(renWin));
}

// ----------------------------------------------------------------------------
int vtkKWELineIntergralConvolution2D::Execute(unsigned int extent[4])
{
 if(this->Steps<=0)
    {
      vtkErrorMacro("Steps should be positive.");
      return 0;
    }

  if(this->StepSize<=0.0)
    {
      vtkErrorMacro("StepSize should be positive.");
      return 0;
    }
  vtkTimerLog* timer=  vtkTimerLog::New();
  timer->StartTimer();

  int components[2];
  components[0] = this->Components[0];
  components[1] = this->Components[1];

  if (this->VectorField->GetComponents() < 2)
    {
    vtkErrorMacro("VectorField must have atleast 2 components.");
    return 0;
    }

  if (this->VectorField->GetComponents() == 2)
    {
    // for 2 component textures (LA texture)
    components[0] = 0;
    components[1] = 3;
    }

  const char componentNames[] = {'x', 0x0, 'y', 0x0, 'z', 0x0, 'w', 0x0};
 
  vtkstd::string additionalKernel = ::vtkKWELineIntergralConvolution2DCode;
  additionalKernel.replace(additionalKernel.find('$'), 1, &componentNames[2*components[0]]);
  additionalKernel.replace(additionalKernel.find('$'), 1, &componentNames[2*components[1]]);

  unsigned int inWidth = this->VectorField->GetWidth();
  unsigned int inHeight = this->VectorField->GetHeight();

  // Compute the transform for the vector field. This is a 2x2 diagonal matrix.
  // Hence, we only pass the non-NULL diagonal values.
  double vectorTransform[2] = {1.0, 1.0};
  if (this->TransformVectors)
    {
    vectorTransform[0] = 1.0/(inWidth * this->Spacing[0]);
    vectorTransform[1] = 1.0/(inHeight * this->Spacing[1]);
    }
  vtkKWELICDebug("vectorTransform: " << vectorTransform[0] << ", " << vectorTransform[1]);

  unsigned int outWidth = (extent[1] - extent[0] + 1) *
    static_cast<unsigned int>(this->Magnification);
  unsigned int outHeight = (extent[3] - extent[2] + 1) *
    static_cast<unsigned int>(this->Magnification);

  double vTCoords[4];
  vTCoords[0] = extent[0]/static_cast<double>(inWidth-1); //xmin
  vTCoords[1] = extent[1]/static_cast<double>(inWidth-1); //xmax
  vTCoords[2] = extent[2]/static_cast<double>(inHeight-1); //xmin
  vTCoords[3] = extent[3]/static_cast<double>(inHeight-1); //xmax
  double licTCoords[4] = {0.0, 1.0, 0.0, 1.0};

  vtkOpenGLRenderWindow *context=
    vtkOpenGLRenderWindow::SafeDownCast(this->VectorField->GetContext());
  if (!context->GetExtensionManager()->LoadSupportedExtension("GL_VERSION_1_3"))
    {
    vtkErrorMacro("Missing required GL_VERSION_1_3.");
    return 0;
    }
  
  vtkTextureObject *tcoords=vtkTextureObject::New();
  tcoords->SetContext(context);
  // (r,g) == (s,t) tcoords
  // (b) == not-used.
  tcoords->Create2D(outWidth, outHeight, 3, VTK_FLOAT, false);
  vtkKWELICDebug("tcoords TOId="<< tcoords->GetHandle());
  
  vtkTextureObject *lic=vtkTextureObject::New();
  lic->SetContext(context);
  // (r,g,b) == LIC image.
  lic->Create2D(outWidth, outHeight, 3, VTK_FLOAT, false);
  vtkKWELICDebug("lic TOId="<< lic->GetHandle());
  
  vtkTextureObject *tcoords2=vtkTextureObject::New();
  tcoords2->SetContext(context);
  // (r,g) == (s,t) tcoords
  // (b) == not-used.
  tcoords2->Create2D(outWidth, outHeight, 3, VTK_FLOAT, false);
  vtkKWELICDebug("tcoords2 TOId="<< tcoords2->GetHandle());
  
  vtkTextureObject *lic2=vtkTextureObject::New();
  lic2->SetContext(context);
  // (r,g,b) == LIC image.
  lic2->Create2D(outWidth, outHeight, 3, VTK_FLOAT, false);
  vtkKWELICDebug("lic2 TOId="<< lic2->GetHandle());

  vtkFrameBufferObject *fbo=vtkFrameBufferObject::New();
  fbo->SetDepthBufferNeeded(false);
  fbo->SetContext(context);
  fbo->SetColorBuffer(0,lic);
  fbo->SetColorBuffer(1,tcoords);
  fbo->SetColorBuffer(2,lic2);
  fbo->SetColorBuffer(3,tcoords2);
  fbo->SetNumberOfRenderTargets(4);
  
  unsigned int passOne[] = {0, 1};
  unsigned int passTwo[] = {2, 3};

  fbo->SetActiveBuffers(2,passOne);
  fbo->Start(static_cast<int>(outWidth),static_cast<int>(outHeight),false);

  vtkShaderProgram2 *pgm=vtkShaderProgram2::New();
  pgm->SetContext(context);
  
  vtkShader2 *s1=vtkShader2::New();
  s1->SetContext(pgm->GetContext());
  s1->SetType(VTK_SHADER_TYPE_FRAGMENT);
  s1->SetSourceCode(vtkKWELineIntergralConvolution2D_fs);
  pgm->GetShaders()->AddItem(s1);
  s1->Delete();
  
  vtkShader2 *pass1=vtkShader2::New();
  pass1->SetContext(pgm->GetContext());
  pass1->SetType(VTK_SHADER_TYPE_FRAGMENT);
  pass1->SetSourceCode(vtkKWELineIntergralConvolution2D_fs1);
  
  vtkShader2 *pass2=vtkShader2::New();
  pass2->SetContext(pgm->GetContext());
  pass2->SetType(VTK_SHADER_TYPE_FRAGMENT);
  pass2->SetSourceCode(vtkKWELineIntergralConvolution2D_fs2);
  
  vtkShader2 *s4=vtkShader2::New();
  s4->SetContext(pgm->GetContext());
  s4->SetType(VTK_SHADER_TYPE_FRAGMENT);
  s4->SetSourceCode(additionalKernel.c_str());
  pgm->GetShaders()->AddItem(s4);
  s4->Delete();
  
  pgm->GetShaders()->AddItem(pass1);
  
  pgm->Build();
  if(pgm->GetLastBuildStatus()!=
     VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("Pass One failed.");
    return 0;
    }

  vtkKWELICDebug("main: first pgm built.");

  vtkgl::ActiveTexture(vtkgl::TEXTURE0);
  this->VectorField->Bind();
  glTexParameteri(this->VectorField->GetTarget(), 
    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(this->VectorField->GetTarget(), 
    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  vtkKWELICDebug("main: vectorField TOid=" << this->VectorField->GetHandle());
  
  vtkgl::ActiveTexture(vtkgl::TEXTURE1);
  this->Noise->Bind();

  vtkKWELICDebug("main: noisebus bind TOid=" << this->Noise->GetHandle());

  // Determine noise scale factor (this makes it possible to use a smaller noise
  // texture than the input image.
  double noiseScale[2] = { 1.0, 1.0};
  noiseScale[0] = this->Magnification * this->VectorField->GetWidth()/
    static_cast<double>(this->Noise->GetWidth());
  noiseScale[1] = this->Magnification * this->VectorField->GetHeight()/
    static_cast<double>(this->Noise->GetHeight());
  vtkKWELICDebug("*** noiseScale: " << noiseScale[0] << ", " << noiseScale[1]);
  
  float fvalues[2];
  fvalues[0]=static_cast<float>(this->Shift);
  fvalues[1]=static_cast<float>(this->Scale);
  pgm->GetUniformVariables()->SetUniformf("uVectorShiftScale",2,fvalues);
  fvalues[0]=static_cast<float>(noiseScale[0]);
  fvalues[1]=static_cast<float>(noiseScale[1]);
  pgm->GetUniformVariables()->SetUniformf("uNoiseScale",2,fvalues);
  fvalues[0]=static_cast<float>(vectorTransform[0]);
  fvalues[1]=static_cast<float>(vectorTransform[1]);
  pgm->GetUniformVariables()->SetUniformf("uVectorTransform",2,fvalues);
  int value=0;
  pgm->GetUniformVariables()->SetUniformi("texVectorField",1,&value);
  value=1;
  pgm->GetUniformVariables()->SetUniformi("texNoise",1,&value);
  fvalues[0]=static_cast<float>(this->StepSize);
  pgm->GetUniformVariables()->SetUniformf("stepSize",1,fvalues);
  value=this->Steps;
  pgm->GetUniformVariables()->SetUniformi("uNumSteps",1,&value);
  
  pgm->Use();
  if(!pgm->IsValid())
    {
    vtkErrorMacro(<<" validation of the program failed: "<<pgm->GetLastValidateLog());
    }
  
  vtkGraphicErrorMacro(context,"error1");
#ifdef VTKKWE_LICDEBUGON
  cout<<"glFinish before renderquad1"<<endl;
  glFinish();
#endif
  RENDERQUAD
#ifdef VTKKWE_LICDEBUGON
  cout<<"glFinish after renderquad1"<<endl;
  glFinish();
#endif
  vtkGraphicErrorMacro(context,"error2");
  pgm->Restore();
  vtkKWELICDebug("main: first pgm unbind.");
  
  pgm->GetShaders()->RemoveItem(pass1);
  pgm->GetShaders()->AddItem(pass2);
  

  vtkKWELICDebug("bind2");
  
  pgm->Build();
  if(pgm->GetLastBuildStatus()!=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("Pass two not built.");
    return 0;
    }
  
  // did the values uniform changed since pass1?
#if 0
  fvalues[0]=this->Shift;
  fvalues[1]=this->Scale;
  pgm->GetUniformVariables()->SetUniformf("uVectorShiftScale",2,fvalues);
  fvalues[0]=noiseScale[0];
  fvalues[1]=noiseScale[1];
  pgm->GetUniformVariables()->SetUniformf("uNoiseScale",2,fvalues);
  fvalues[0]=vectorTransform[0];
  fvalues[1]=vectorTransform[1];
  pgm->GetUniformVariables()->SetUniformf("uVectorTransform",2,fvalues);
  value=0;
  pgm->GetUniformVariables()->SetUniformi("texVectorField",1,&value);
  value=1;
  pgm->GetUniformVariables()->SetUniformi("texNoise",1,&value);
  fvalues[0]=this->StepSize;
  pgm->GetUniformVariables()->SetUniformf("stepSize",1,fvalues);
  value=this->Steps;
  pgm->GetUniformVariables()->SetUniformi("uNumSteps",1,&value);
#endif
  // new uniforms
  value=2;
  pgm->GetUniformVariables()->SetUniformi("texLIC",1,&value);
  value=3;
  pgm->GetUniformVariables()->SetUniformi("texTCoords",1,&value);
  
  pgm->Use();
  
  unsigned int *curBuf=0; // silent warning, we made sure Steps>=1.
  unsigned int *otherBuf=0; // silent warning, we made sure Steps>=1.
  for (int direction=0; direction < 2; direction++)
    {
    for (int cc=0; cc < this->Steps; cc++)
      {
      curBuf=cc%2==0? passTwo: passOne;
      otherBuf= cc%2==0? passOne : passTwo;
      vtkKWELICDebug(cc << ": Active Render Buffers: " << curBuf[0] << ", " << curBuf[1]);
      fbo->SetActiveBuffers(2, curBuf);
      fbo->Start(outWidth, outHeight, false);

      vtkgl::ActiveTexture(vtkgl::TEXTURE2);
      vtkTextureObject* inLIC = fbo->GetColorBuffer(otherBuf[0]);
      inLIC->Bind();
      vtkKWELICDebug("inLIC: " << inLIC->GetHandle());

      vtkgl::ActiveTexture(vtkgl::TEXTURE3);
      vtkTextureObject* inTcords = fbo->GetColorBuffer(otherBuf[1]);
      inTcords->Bind();
      vtkKWELICDebug("inTcords: " << inTcords->GetHandle());

      value=direction? 1 : -1;
      pgm->GetUniformVariables()->SetUniformi("uDirection",1,&value);
      
      if (direction == 1 && cc == 0)
        {
        value=1;
        }
      else
        {
        value=0;
        }
      pgm->GetUniformVariables()->SetUniformi("reset",1,&value);
      pgm->SendUniforms(); // force resending uniforms
      
      if(!pgm->IsValid())
        {
        vtkErrorMacro(<<" validation of the program failed: "<<pgm->GetLastValidateLog());
        }
      
#ifdef VTKKWE_LICDEBUGON
       cout<<"glFinish before renderquad2"<<endl;
  glFinish();
#endif
      RENDERQUAD
#ifdef VTKKWE_LICDEBUGON
 cout<<"glFinish after renderquad2"<<endl;
  glFinish();
#endif
      }
    }
  glFinish();
  timer->StopTimer();
  pgm->Restore();
  vtkKWELICDebug("Exec Time: " <<  timer->GetElapsedTime());
  timer->Delete();


  this->LIC=fbo->GetColorBuffer(curBuf[0]);
  fbo->GetColorBuffer(otherBuf[0])->Delete();
  pass1->ReleaseGraphicsResources();
  pass1->Delete();
  pass2->ReleaseGraphicsResources();
  pass2->Delete();
  pgm->ReleaseGraphicsResources();
  pgm->Delete();
  fbo->Delete();
  tcoords->Delete();
  tcoords2->Delete();
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWELineIntergralConvolution2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
