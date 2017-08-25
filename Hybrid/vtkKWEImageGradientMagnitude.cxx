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
#include "vtkKWEImageGradientMagnitude.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkKWEDataTransferHelper.h"
#include "vtkFrameBufferObject.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkKWEStructuredExtent.h"
#include "vtkTextureObject.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkgl.h"
extern const char* vtkKWEImageGradientMagnitude_2D_fs;
extern const char* vtkKWEImageGradientMagnitude_3D_fs;

vtkStandardNewMacro(vtkKWEImageGradientMagnitude);
vtkCxxRevisionMacro(vtkKWEImageGradientMagnitude, "$Revision: 706 $");
//----------------------------------------------------------------------------
vtkKWEImageGradientMagnitude::vtkKWEImageGradientMagnitude()
{
  this->Dimensionality = 2;
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkKWEImageGradientMagnitude::~vtkKWEImageGradientMagnitude()
{
}

//----------------------------------------------------------------------------
vtkKWEGPUImageAlgorithmDriver::ExtentTypes 
vtkKWEImageGradientMagnitude::GetSplitMode(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* vtkNotUsed(outputVector))
{
  return vtkKWEGPUImageAlgorithmDriver::XY_PLANE;
}

//----------------------------------------------------------------------------
int vtkKWEImageGradientMagnitude::MapOutputExtentToInput(int input_extent[6],
  int vtkNotUsed(port), int vtkNotUsed(connection), 
  vtkInformation* inInfo, const int output_extent[6])
{
  int dimensionality = 2;
  if (this->Dimensionality == 3)
    {
    int whole_extent[6];
    if (!inInfo->Has(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()))
      {
      vtkErrorMacro("Missing WHOLE_EXTENT().");
      return 0;
      }
    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whole_extent);

    int whole_dims[3];
    vtkKWEStructuredExtent::GetDimensions(whole_extent, whole_dims);
    dimensionality = vtkStructuredData::GetDataDimension(
      vtkStructuredData::GetDataDescription(whole_dims));
    dimensionality = (this->Dimensionality == 3 && dimensionality == 3)?
      3 : 2;
    }

  memcpy(input_extent, output_extent, 6*sizeof(int));
  vtkKWEStructuredExtent::Grow(input_extent, 1);
  if (dimensionality != 3)
    {
    // We don't need ghost levels along Z axis, unless going 3D gradients.
    input_extent[4] = output_extent[4];
    input_extent[5] = output_extent[5];
    }

  return (dimensionality !=3)? 2 : 3;
}

//----------------------------------------------------------------------------
bool vtkKWEImageGradientMagnitude::InitializeExecution(
  vtkInformation* vtkNotUsed(request), 
  vtkInformationVector** inputVector, 
  vtkInformationVector* vtkNotUsed(outputVector))
{
  int dimensionality = 2;
  if (this->Dimensionality == 3)
    {
    vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
    int whole_extent[6];
    if (!inInfo->Has(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()))
      {
      vtkErrorMacro("Missing WHOLE_EXTENT().");
      return 0;
      }
    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whole_extent);

    int whole_dims[3];
    vtkKWEStructuredExtent::GetDimensions(whole_extent, whole_dims);
    dimensionality = vtkStructuredData::GetDataDimension(
      vtkStructuredData::GetDataDescription(whole_dims));
    }
  dimensionality = dimensionality == 3? 3: 2;

  vtkShaderProgram2* pgm = vtkShaderProgram2::New();
  pgm->SetContext(vtkOpenGLRenderWindow::SafeDownCast(this->GetContext()));
  
  vtkShader2 *shader=vtkShader2::New();
  shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
  shader->SetSourceCode(dimensionality == 3? 
                        vtkKWEImageGradientMagnitude_3D_fs:
                        vtkKWEImageGradientMagnitude_2D_fs);
  pgm->GetShaders()->AddItem(shader);
  shader->Delete();
  this->GLSLProgram = pgm;
  pgm->Delete();

  vtkImageData* input = vtkImageData::GetData(inputVector[0], 0);
  input->GetSpacing(this->SpacingReciprocal);
  this->SpacingReciprocal[0] = 0.5/this->SpacingReciprocal[0];
  this->SpacingReciprocal[1] = 0.5/this->SpacingReciprocal[1];
  this->SpacingReciprocal[2] = 0.5/this->SpacingReciprocal[2];
  return true;
}

//----------------------------------------------------------------------------
bool vtkKWEImageGradientMagnitude::Execute(
  vtkKWEGPUImageAlgorithmDriver::vtkBuses* upBuses,
  vtkKWEDataTransferHelper* downBus)
{
  vtkKWEDataTransferHelper* upBus = upBuses->GetBus(0, 0);

  int dims[3];
  dims[0] = downBus->GetTexture()->GetWidth();
  dims[1] = downBus->GetTexture()->GetHeight();
  dims[2] = 0;

  double tcoords[6];
  if (!this->ComputeTCoordsRange(tcoords,
    upBus->GetGPUExtent(), 
    downBus->GetGPUExtent()))
    {
    return false;
    }

  // Create fbo to render.
  vtkFrameBufferObject* fbo = vtkFrameBufferObject::New();
  fbo->SetContext(vtkOpenGLRenderWindow::SafeDownCast(this->Context));
  fbo->SetColorBuffer(0, downBus->GetTexture());
  fbo->Start(dims[0], dims[1], true);

  this->GLSLProgram->Build();
  if(this->GLSLProgram->GetLastBuildStatus()!=
     VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("GLSLProgram bind failed.");
    return false;
    }
  
  int value;
  value=0;
  this->GLSLProgram->GetUniformVariables()->SetUniformi("uInputImage",1,
                                                        &value);
  
  float fvalues[3];
  fvalues[0]=static_cast<float>(upBus->GetTexture()->GetWidth());
  fvalues[1]=static_cast<float>(upBus->GetTexture()->GetHeight());
  fvalues[2]=static_cast<float>(upBus->GetTexture()->GetDepth());
  this->GLSLProgram->GetUniformVariables()->SetUniformf("uInputImageDims",3,
                                                        fvalues);
  
  fvalues[0]=static_cast<float>(this->SpacingReciprocal[0]);
  fvalues[1]=static_cast<float>(this->SpacingReciprocal[1]);
  fvalues[2]=static_cast<float>(this->SpacingReciprocal[2]);
  this->GLSLProgram->GetUniformVariables()->SetUniformf("uSpacingReciprocal",3,
                                                        fvalues);
  
  this->GLSLProgram->Use();
  
  
  double zTexCoord = (downBus->GetGPUExtent()[4] == upBus->GetGPUExtent()[4])? 0.0 : 0.5;
  // render viewport-sized quad to perform actual computation
  upBus->GetTexture()->Bind();
  glTexParameteri(upBus->GetTexture()->GetTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(upBus->GetTexture()->GetTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(upBus->GetTexture()->GetTarget(), vtkgl::TEXTURE_WRAP_R, GL_CLAMP);

  if(!this->GLSLProgram->IsValid())
    {
    vtkErrorMacro(<<" validation of the program failed: "<<this->GLSLProgram->GetLastValidateLog());
    }
  
  glBegin(GL_QUADS);
  
  glTexCoord3f(static_cast<GLfloat>(tcoords[0]),
               static_cast<GLfloat>(tcoords[2]),
               static_cast<GLfloat>(zTexCoord));
  glVertex2f(0.0, 0.0);
  
  glTexCoord3f(static_cast<GLfloat>(tcoords[1]),
               static_cast<GLfloat>(tcoords[2]),
               static_cast<GLfloat>(zTexCoord));
  glVertex2f(static_cast<GLfloat>(dims[0]), 0.0);
  
  glTexCoord3f(static_cast<GLfloat>(tcoords[1]),
               static_cast<GLfloat>(tcoords[3]),
               static_cast<GLfloat>(zTexCoord));
  glVertex2f(static_cast<GLfloat>(dims[0]),
             static_cast<GLfloat>(dims[1]));
  
  glTexCoord3f(static_cast<GLfloat>(tcoords[0]),
               static_cast<GLfloat>(tcoords[3]),
               static_cast<GLfloat>(zTexCoord));
  glVertex2f(0.0, static_cast<GLfloat>(dims[1]));
  glEnd();
  fbo->Delete();
  return true;
}

//----------------------------------------------------------------------------
bool vtkKWEImageGradientMagnitude::FinalizeExecution(
  vtkInformation* vtkNotUsed(request), 
  vtkInformationVector** vtkNotUsed(inputVector), 
  vtkInformationVector* vtkNotUsed(outputVector))
{
  this->GLSLProgram = 0;
  return true;
}

//----------------------------------------------------------------------------
void vtkKWEImageGradientMagnitude::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Dimensionality: " << this->Dimensionality << endl;
}

