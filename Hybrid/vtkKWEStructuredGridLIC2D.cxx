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
#include "vtkKWEStructuredGridLIC2D.h"

#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkImageNoiseSource.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkKWEDataTransferHelper.h"
#include "vtkFrameBufferObject.h"
#include "vtkKWELineIntergralConvolution2D.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkKWEStructuredExtent.h"
#include "vtkTextureObject.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkPointData.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <assert.h>
#include "vtkgl.h"

extern const char *vtkKWEStructuredGridLIC2D_fs;

#define PRINTEXTENT(ext) \
  ext[0] << ", " << ext[1] << ", " << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5] 

vtkStandardNewMacro(vtkKWEStructuredGridLIC2D);
vtkCxxRevisionMacro(vtkKWEStructuredGridLIC2D, "$Revision: 706 $");
//----------------------------------------------------------------------------
vtkKWEStructuredGridLIC2D::vtkKWEStructuredGridLIC2D()
{
  this->Context = 0;
  this->Steps=1;
  this->StepSize=1.0;
  this->Magnification=1;
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(2);
  this->OwnWindow = false;

  this->NoiseSource = vtkImageNoiseSource::New();
  this->NoiseSource->SetWholeExtent(0, 127, 0, 127, 0, 0);
  this->NoiseSource->SetMinimum(0.0);
  this->NoiseSource->SetMaximum(1.0);
}

//----------------------------------------------------------------------------
vtkKWEStructuredGridLIC2D::~vtkKWEStructuredGridLIC2D()
{
  this->NoiseSource->Delete();
  this->SetContext(0);
}

//----------------------------------------------------------------------------
vtkRenderWindow* vtkKWEStructuredGridLIC2D::GetContext()
{
  return this->Context;
}

//----------------------------------------------------------------------------
void vtkKWEStructuredGridLIC2D::SetContext(vtkRenderWindow *context)
{
  if (this->Context == context)
    {
    return;
    }

  if (this->Context && this->OwnWindow)
    {
    this->Context->Delete();
    }
  this->OwnWindow = false;


  vtkOpenGLRenderWindow* openGLRenWin = vtkOpenGLRenderWindow::SafeDownCast(context);
  this->Context = openGLRenWin;

  if (openGLRenWin)
    {
    vtkOpenGLExtensionManager* mgr = openGLRenWin->GetExtensionManager();
    
    // optional for texture objects.
    mgr->LoadSupportedExtension("GL_EXT_texture_integer");
    
    if (!mgr->LoadSupportedExtension("GL_VERSION_1_3") ||
    !mgr->LoadSupportedExtension("GL_ARB_texture_non_power_of_two") ||
    !mgr->LoadSupportedExtension("GL_VERSION_1_2") ||
    !mgr->LoadSupportedExtension("GL_VERSION_2_0") ||
    !mgr->LoadSupportedExtension("GL_ARB_texture_float") ||
    !mgr->LoadSupportedExtension("GL_ARB_color_buffer_float"))
      {
      vtkErrorMacro("Required OpenGL extensions not supported.");
      this->Context = 0;
      }
    }
  this->Modified();
}

//----------------------------------------------------------------------------
// Description:
// Fill the input port information objects for this algorithm.  This
// is invoked by the first call to GetInputPortInformation for each
// port so subclasses can specify what they can handle.
// Redefined from the superclass.
int vtkKWEStructuredGridLIC2D::FillInputPortInformation(int port,
                                          vtkInformation *info)
{
  if (port==0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkStructuredGrid");
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(),0);
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(),0);
    }
  else
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(),"vtkImageData");
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(),0);
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(),1);
    }
  return 1;
}

// ----------------------------------------------------------------------------
// Description:
// Fill the output port information objects for this algorithm.
// This is invoked by the first call to GetOutputPortInformation for
// each port so subclasses can specify what they can handle.
// Redefined from the superclass.
int vtkKWEStructuredGridLIC2D::FillOutputPortInformation(int port,
                                                       vtkInformation *info)
{
  if (port==0)
    {
    // input+texcoords
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkStructuredGrid");
    }
  else
    {
    // LIC texture
    info->Set(vtkDataObject::DATA_TYPE_NAME(),"vtkImageData");
    }
  return 1;
}
//----------------------------------------------------------------------------
// We need to report output extent after taking into consideration the
// magnification.
int vtkKWEStructuredGridLIC2D::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  int ext[6];
  double spacing[3];

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(1);

  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), ext);

  spacing[0]=1.0;
  spacing[1]=1.0;
  spacing[2]=1.0;

  for (int axis = 0; axis < 3; axis++)
    {
    int wholeMin = ext[axis*2];
    int wholeMax = ext[axis*2+1];
    int dimension = wholeMax - wholeMin + 1;

    // Scale the output extent
    wholeMin = static_cast<int>(ceil(static_cast<double>(wholeMin * this->Magnification)));
    wholeMax = dimension != 1? wholeMin + static_cast<int>(floor(static_cast<double>(dimension * this->Magnification))) -1:
      wholeMin;

    ext[axis*2] = wholeMin;
    ext[axis*2+1] = wholeMax;
    }

  cout << "request info whole ext=" << PRINTEXTENT(ext) << endl;

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), ext, 6);
  outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEStructuredGridLIC2D::RequestUpdateExtent (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(1);

  
  // Tell the vector field input the extents that we need from it.
  // The downstream request needs to be downsized based on the Magnification.
  int ext[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), ext);

  cout << "request update extent, update ext=" << PRINTEXTENT(ext) << endl;

  for (int axis = 0; axis < 3; axis++)
    {
    int wholeMin = ext[axis*2];
    int wholeMax = ext[axis*2+1];
    int dimension = wholeMax - wholeMin + 1;

    // Scale the output extent
    wholeMin = static_cast<int>(ceil(static_cast<double>(wholeMin / this->Magnification)));
    wholeMax = dimension != 1? wholeMin + static_cast<int>(floor(static_cast<double>(dimension / this->Magnification))) -1:
      wholeMin;

    ext[axis*2] = wholeMin;
    ext[axis*2+1] = wholeMax;
    }
  //cout << "UPDATE_EXTENT: " << 
  //  ext[0] << ", " <<
  //  ext[1] << ", " <<
  //  ext[2] << ", " <<
  //  ext[3] << ", " <<
  //  ext[4] << ", " <<
  //  ext[5] << endl;
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), ext, 6);
  
  cout << "request update extent, update ext2=" << PRINTEXTENT(ext) << endl;


  if(inputVector[1]!=0 && inputVector[1]->GetInformationObject(0) != NULL) // optional input
    {
    inInfo = inputVector[1]->GetInformationObject(0);
    // always request the whole extent
    inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
                inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()),
                6);
    }
  
  return 1;
}

//----------------------------------------------------------------------------
// Stolen from vtkImageAlgorithm. Should be in vtkStructuredGridAlgorithm.
void vtkKWEStructuredGridLIC2D::AllocateOutputData(vtkDataObject *output,
                                                 int outputPort)
{ 
  // set the extent to be the update extent
  vtkStructuredGrid *out = vtkStructuredGrid::SafeDownCast(output);
  if (out)
    {
    // this needs to be fixed -Ken
    vtkStreamingDemandDrivenPipeline *sddp = 
      vtkStreamingDemandDrivenPipeline::SafeDownCast(this->GetExecutive());
    int numInfoObj = sddp->GetNumberOfOutputPorts();
    if (sddp && outputPort<numInfoObj)
      {
      int extent[6];
      sddp->GetOutputInformation(outputPort)->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent);
      out->SetExtent(extent);
      }
    else
      {
      vtkWarningMacro( "Not enough output ports." );
      return;
      }
    this->AllocateScalars(out);
    }
  else
    {
    vtkImageData *out2 = vtkImageData::SafeDownCast(output);
    if (out2)
      {
      // this needs to be fixed -Ken
      vtkStreamingDemandDrivenPipeline *sddp = 
        vtkStreamingDemandDrivenPipeline::SafeDownCast(this->GetExecutive());
      int numInfoObj = sddp->GetNumberOfOutputPorts();
      if (sddp && outputPort<numInfoObj)
        {
        int extent[6];
        sddp->GetOutputInformation(outputPort)->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),extent);
        out2->SetExtent(extent);
        }
      else
        {
        vtkWarningMacro( "Not enough output ports." );
        return;
        }
      out2->SetNumberOfScalarComponents(3);
      out2->AllocateScalars();
      }
    }
}

//----------------------------------------------------------------------------
// Stolen from vtkImageData. Should be in vtkStructuredGrid.
void vtkKWEStructuredGridLIC2D::AllocateScalars(vtkStructuredGrid *sg)
{
  int newType = VTK_DOUBLE;
  int newNumComp = 1;

  // basically allocate the scalars based on the
  sg->GetProducerPort();
  if(vtkInformation* info = sg->GetPipelineInformation())
    {
    vtkInformation *scalarInfo = vtkDataObject::GetActiveFieldInformation(info,
      vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
    if (scalarInfo)
      {
      newType = scalarInfo->Get( vtkDataObject::FIELD_ARRAY_TYPE() );
      if ( scalarInfo->Has(vtkDataObject::FIELD_NUMBER_OF_COMPONENTS()) )
        {
        newNumComp = scalarInfo->Get( vtkDataObject::FIELD_NUMBER_OF_COMPONENTS() );
        }
      }
    }

  vtkDataArray *scalars;

  // if the scalar type has not been set then we have a problem
  if (newType == VTK_VOID)
    {
    vtkErrorMacro("Attempt to allocate scalars before scalar type was set!.");
    return;
    }

  const int* extent = sg->GetExtent();
  // Use vtkIdType to avoid overflow on large images
  vtkIdType dims[3];
  dims[0] = extent[1] - extent[0] + 1;
  dims[1] = extent[3] - extent[2] + 1;
  dims[2] = extent[5] - extent[4] + 1;
  vtkIdType imageSize = dims[0]*dims[1]*dims[2];

  // if we currently have scalars then just adjust the size
  scalars = sg->GetPointData()->GetScalars();
  if (scalars && scalars->GetDataType() == newType
      && scalars->GetReferenceCount() == 1)
    {
    scalars->SetNumberOfComponents(newNumComp);
    scalars->SetNumberOfTuples(imageSize);
    // Since the execute method will be modifying the scalars
    // directly.
    scalars->Modified();
    return;
    }

  // allocate the new scalars
  scalars = vtkDataArray::CreateDataArray(newType);
  scalars->SetNumberOfComponents(newNumComp);

  // allocate enough memory
  scalars->SetNumberOfTuples(imageSize);

  sg->GetPointData()->SetScalars(scalars);
  scalars->Delete();
}


//----------------------------------------------------------------------------
int vtkKWEStructuredGridLIC2D::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // 3 passes:
  // pass 1: render to compute the transformed vector field for the points.
  // pass 2: perform LIC with the new vector field. This has to happen in a
  // different pass than computation of the transformed vector.
  // pass 3: Render structured slice quads with correct texture correct
  // tcoords and apply the LIC texture to it.
  
  vtkInformation *inInfo=inputVector[0]->GetInformationObject(0);
  vtkStructuredGrid *input=vtkStructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  int inputRequestedExtent[6];
  inInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
              inputRequestedExtent);

  // Check if the input image is a 2D image (not 0D, not 1D, not 3D)
  int dims[3];
  //  input->GetDimensions(dims);

  vtkKWEStructuredExtent::GetDimensions(inputRequestedExtent,dims);

  cout<<"dims="<<dims[0]<<" "<<dims[1]<<" "<<dims[2]<<endl;
  cout<<"requested ext="<<inputRequestedExtent[0]<<" "<<inputRequestedExtent[1]<<" "<<inputRequestedExtent[2]<<" "<<inputRequestedExtent[3]<<" "<<inputRequestedExtent[4]<<" "<<inputRequestedExtent[5]<<endl;

  if(!(dims[0]==1 && dims[1]>1 && dims[2]>1)
     && !(dims[1]==1 && dims[0]>1 && dims[2]>1)
     && !(dims[2]==1 && dims[0]>1 && dims[1]>1))
    {
    vtkErrorMacro(<<"input is not a 2D image.");
    return 0;
    }
  if(input->GetPointData()==0)
    {
    vtkErrorMacro(<<"input does not have point data.");
    return 0;
    }
  if(input->GetPointData()->GetVectors()==0)
    {
    vtkErrorMacro(<<"input does not vectors on point data.");
    return 0;
    }
  
  if (!this->Context)
    {
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    this->SetContext(renWin);
    this->OwnWindow = true;
    }
  this->Context->SetReportGraphicErrors(1);
  
  
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkStructuredGrid *output = vtkStructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  this->AllocateOutputData(output,0);
  output->ShallowCopy(input);
  
  vtkInformation *outInfoTexture = outputVector->GetInformationObject(1);
  vtkImageData *outputTexture = vtkImageData::SafeDownCast(
    outInfoTexture->Get(vtkDataObject::DATA_OBJECT()));
  this->AllocateOutputData(outputTexture,1);
  
  // Noise.
  vtkInformation *noiseInfo = inputVector[1]->GetInformationObject(0);
  vtkImageData *noise=0;
  if(noiseInfo==0)
    {
    this->NoiseSource->Update();
    noise = this->NoiseSource->GetOutput();
    }
  else
    {
    noise=vtkImageData::SafeDownCast(
      noiseInfo->Get(vtkDataObject::DATA_OBJECT()));
    
    if(noise->GetPointData()==0)
      {
      vtkErrorMacro(<<"provided noise does not have point data.");
      return 0;
      }
    if(noise->GetPointData()->GetScalars()==0)
      {
      vtkErrorMacro(<<"provided noise does not have scalars on point data.");
      return 0;
      }
    }
 
  int width;
  int height;
  int firstComponent;
  int secondComponent;
  int slice;
  if(dims[0]==1)
    {
      cout<<"x"<<endl;
       firstComponent = 1;
       secondComponent = 2;
      slice=0;
    }
  else
    {
    if(dims[1]==1)
      {
        cout<<"y"<<endl;
      firstComponent = 0;
      secondComponent = 2;
      slice=1;
      }
    else
      {
        cout<<"z"<<endl;
        firstComponent = 0;
        secondComponent = 1;
        slice=2;
      }
    }
  
  width=dims[firstComponent];
  height=dims[secondComponent];

  cout<<"w="<<width<<" h="<<height<<endl;

  vtkKWEDataTransferHelper *vectorFieldBus=vtkKWEDataTransferHelper::New();
  vectorFieldBus->SetContext(this->Context);
  vectorFieldBus->SetCPUExtent(inputRequestedExtent); // input->GetExtent());
  vectorFieldBus->SetGPUExtent(inputRequestedExtent); // input->GetExtent());
  //  vectorFieldBus->SetTextureExtent(input->GetExtent());
  vectorFieldBus->SetArray(input->GetPointData()->GetVectors());
  
  vtkKWEDataTransferHelper *pointBus=vtkKWEDataTransferHelper::New();
  pointBus->SetContext(this->Context);
  pointBus->SetCPUExtent(inputRequestedExtent); // input->GetExtent());
  pointBus->SetGPUExtent(inputRequestedExtent); // input->GetExtent());
  //  pointBus->SetTextureExtent(input->GetExtent());
  pointBus->SetArray(input->GetPoints()->GetData());
  
  vtkOpenGLExtensionManager* mgr = vtkOpenGLExtensionManager::New();
  mgr->SetRenderWindow(this->Context);
  
  // Vector field in image space.
  vtkTextureObject *vector2=vtkTextureObject::New();
  vector2->SetContext(this->Context);
  vector2->Create2D(width,height,3,VTK_FLOAT,false);
  
  cout<<"Vector field in image space (target) textureId="<<vector2->GetHandle()<<endl;

  vtkFrameBufferObject *fbo=vtkFrameBufferObject::New();
  fbo->SetContext(this->Context);
  fbo->SetColorBuffer(0,vector2);
  fbo->SetNumberOfRenderTargets(1);
  fbo->SetActiveBuffer(0);
  fbo->Start(width,height,false);
  
  vtkShaderProgram2 *pgm=vtkShaderProgram2::New();
  pgm->SetContext(static_cast<vtkOpenGLRenderWindow *>(this->Context.GetPointer()));
  
  vtkShader2 *shader=vtkShader2::New();
  shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
  shader->SetSourceCode(vtkKWEStructuredGridLIC2D_fs);
  shader->SetContext(pgm->GetContext());
  pgm->GetShaders()->AddItem(shader);
  shader->Delete();
  
  pgm->Build();
  if(pgm->GetLastBuildStatus()!=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("shader build error.");
    return 0;
    }
  
  int value=0;
  pgm->GetUniformVariables()->SetUniformi("texPoints",1,&value);
  value=1;
  pgm->GetUniformVariables()->SetUniformi("texVectorField",1,&value);
  float fvalues[3];
  fvalues[0]=static_cast<float>(dims[0]);
  fvalues[1]=static_cast<float>(dims[1]);
  fvalues[2]=static_cast<float>(dims[2]);
  pgm->GetUniformVariables()->SetUniformf("uDimensions",3,fvalues);
  value=slice;
  pgm->GetUniformVariables()->SetUniformi("uSlice",1,&slice);
  
  vtkgl::ActiveTexture(vtkgl::TEXTURE0);
  pointBus->Upload(0,0);
  vtkTextureObject *points=pointBus->GetTexture();
  points->Bind();
  glTexParameteri(points->GetTarget(),GL_TEXTURE_WRAP_S,
                  GL_CLAMP);
  glTexParameteri(points->GetTarget(),GL_TEXTURE_WRAP_T,
                  GL_CLAMP);
  glTexParameteri(points->GetTarget(), vtkgl::TEXTURE_WRAP_R,
                  GL_CLAMP);
  glTexParameteri(points->GetTarget(), GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST);
  glTexParameteri(points->GetTarget(), GL_TEXTURE_MAG_FILTER,
                  GL_NEAREST);
  
  cout<<"points on texture unit 0, textureId=="<<points->GetHandle()<<endl;

  vtkgl::ActiveTexture(vtkgl::TEXTURE1);
  vectorFieldBus->Upload(0,0);
  vtkTextureObject *vectorField=vectorFieldBus->GetTexture();
  vectorField->Bind();
  glTexParameteri(vectorField->GetTarget(),GL_TEXTURE_WRAP_S,
      GL_CLAMP);
  glTexParameteri(vectorField->GetTarget(),GL_TEXTURE_WRAP_T,
      GL_CLAMP);
  glTexParameteri(vectorField->GetTarget(), vtkgl::TEXTURE_WRAP_R,
      GL_CLAMP);
  glTexParameteri(vectorField->GetTarget(), GL_TEXTURE_MIN_FILTER,
      GL_NEAREST);
  glTexParameteri(vectorField->GetTarget(), GL_TEXTURE_MAG_FILTER,
      GL_NEAREST);
  
  cout<<"vector field on texture unit 1, textureId=="<<vectorField->GetHandle()<<endl;
  
  pgm->Use();
  
  if(!pgm->IsValid())
    {
    vtkErrorMacro(<<" validation of the program failed: "<<pgm->GetLastValidateLog());
    }
  
  cout<<"glFinish before rendering quad"<<endl;
  glFinish(); // debug: GL break point 
  vtkGraphicErrorMacro(this->Context,"error1");
  fbo->RenderQuad(0, width-1,0,height-1);
  vtkGraphicErrorMacro(this->Context,"error2");

  cout<<"glFinish after rendering quad"<<endl;
  glFinish(); // debug: GL break point 

  pgm->Restore();
  
  vtkKWELineIntergralConvolution2D *internal=vtkKWELineIntergralConvolution2D::New();
  internal->SetSteps(this->Steps);
  internal->SetStepSize(this->StepSize);
  internal->SetMagnification(this->Magnification);
  internal->SetComponents(firstComponent,secondComponent);

  vtkKWEDataTransferHelper *noiseBus=vtkKWEDataTransferHelper::New();
  noiseBus->SetContext(this->Context);
  noiseBus->SetCPUExtent(noise->GetExtent());
  noiseBus->SetGPUExtent(noise->GetExtent());
  //  noiseBus->SetTextureExtent(noise->GetExtent());
  noiseBus->SetArray(noise->GetPointData()->GetScalars());
  noiseBus->Upload(0,0);

  internal->SetVectorField(vector2);
  internal->SetNoise(noiseBus->GetTexture());
  internal->Execute();

  vtkKWEDataTransferHelper *outputBus=vtkKWEDataTransferHelper::New();
  outputBus->SetContext(this->Context);
  outputBus->SetCPUExtent(outputTexture->GetExtent());
  outputBus->SetGPUExtent(outputTexture->GetExtent());
  //  outputBus->SetTextureExtent(outputTexture->GetExtent());
  outputBus->SetTexture(internal->GetLIC());
  outputBus->SetArray(outputTexture->GetPointData()->GetScalars());
  outputBus->Download();
  outputBus->Delete();
  
  // Pass three. Generate texture coordinates. Software.
  vtkFloatArray *tcoords=vtkFloatArray::New();
  tcoords->SetNumberOfComponents(2);
  tcoords->SetNumberOfTuples(dims[0]*dims[1]*dims[2]);
  output->GetPointData()->SetTCoords(tcoords);
  tcoords->Delete();
  
  double ddim[3];
  ddim[0]=static_cast<double>(dims[0]-1);
  ddim[1]=static_cast<double>(dims[1]-1);
  ddim[2]=static_cast<double>(dims[2]-1);
  
  int tz=0;
  while(tz<dims[slice])
    {
    int ty=0;
    while(ty<dims[secondComponent])
      {
      int tx=0;
      while(tx<dims[firstComponent])
        {
        tcoords->SetTuple2((tz*dims[secondComponent]+ty)*dims[firstComponent]+tx,tx/ddim[firstComponent],ty/ddim[secondComponent]);
        ++tx;
        }
      ++ty;
      }
    ++tz;
    }

  internal->Delete();
  noiseBus->Delete();
  vectorFieldBus->Delete();
  pointBus->Delete();
  mgr->Delete();
  vector2->Delete();
  fbo->Delete();
  pgm->ReleaseGraphicsResources();
  pgm->Delete();

  // Make sure the active texture is back to texture0 for the part of the
  // pipeline using OpenGL 1.1 (texture on actor)
  vtkgl::ActiveTexture(vtkgl::TEXTURE0);

  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEStructuredGridLIC2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}