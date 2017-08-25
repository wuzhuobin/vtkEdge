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

#include "vtkKWEGPUArrayCalculator.h"

#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkKWEFunctionToGLSL.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkgl.h"
#include "vtkKWEDataArrayStreamer.h"
#include <vtkstd/vector>
#include "vtkKWEDataTransferHelper.h"
#include "vtkFrameBufferObject.h"
#include "vtkTextureObject.h"
#include <assert.h>
#include "vtkTimerLog.h"
#include "vtkFloatArray.h"

//#define VTKKWE_GPU_ARRAY_CALCULATOR_DEBUG // display debug info

vtkCxxRevisionMacro(vtkKWEGPUArrayCalculator, "$Revision: 710 $");
vtkStandardNewMacro(vtkKWEGPUArrayCalculator);

// ----------------------------------------------------------------------------
vtkKWEGPUArrayCalculator::vtkKWEGPUArrayCalculator()
{
  // initialized by superclass
  assert("this->FunctionParser exists" && this->FunctionParser!=0);

  //  this->FunctionParser->Delete();
  //  this->FunctionParser = vtkKWEFunctionToGLSL::New();
  this->FunctionParserToGLSL=vtkKWEFunctionToGLSL::New();

  this->Context = 0;
  this->SupportedByHardware=false;

  this->MaxGPUMemorySizeInBytes=128*1024*1024;
  this->SizeThreshold=0;
  this->UseCalibration=0;
  this->CalibratedSizeThreshold=0;
  this->CalibrationDone=false;
}

// ----------------------------------------------------------------------------
vtkKWEGPUArrayCalculator::~vtkKWEGPUArrayCalculator()
{
  this->FunctionParserToGLSL->Delete();
  this->FunctionParserToGLSL = NULL;

  this->SetContext(0);
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::SetContext(vtkRenderWindow* context)
{
  vtkSetObjectBodyMacro(Context, vtkRenderWindow, context);
  if (this->Context)
    {
      vtkOpenGLExtensionManager *mgr=vtkOpenGLExtensionManager::New();
      mgr->SetRenderWindow(this->Context);
      
      // We need shader support, PBO, FBO, multitexturing, draw buffers,
      // NPOT textures, float textures
      
      // GL_ARB_draw_buffers requires at least OpenGL>=1.3
      // (multitexturing is part of OpenGL 1.3)
      bool gl13=mgr->ExtensionSupported("GL_VERSION_1_3")==1;
      
      if(!gl13)
        {
          // give up
          this->SupportedByHardware=false;
        }
      else
        {
          bool gl15=mgr->ExtensionSupported("GL_VERSION_1_5")==1;
          bool gl20=mgr->ExtensionSupported("GL_VERSION_2_0")==1;
          bool gl21=mgr->ExtensionSupported("GL_VERSION_2_1")==1;
          
          bool vbo=gl15 ||
            mgr->ExtensionSupported("GL_ARB_vertex_buffer_object")==1;
          
          bool pbo=vbo &&
            (gl21 || mgr->ExtensionSupported("GL_ARB_pixel_buffer_object")==1);
          
          bool drawbuffers=gl20 ||
            mgr->ExtensionSupported("GL_ARB_draw_buffers")==1;
          bool npot=gl20 ||
            mgr->ExtensionSupported("GL_ARB_texture_non_power_of_two")==1;
          
          bool shading_language_100=gl20 ||
            mgr->ExtensionSupported("GL_ARB_shading_language_100");
          bool shader_objects=gl20 ||
            mgr->ExtensionSupported("GL_ARB_shader_objects");
          
          bool fragment_shader=gl20 ||
            mgr->ExtensionSupported("GL_ARB_fragment_shader");
          bool fbo=mgr->ExtensionSupported("GL_EXT_framebuffer_object" )==1;
          
          bool texture_float=mgr->ExtensionSupported("GL_ARB_texture_float")==1;
          
          this->SupportedByHardware=pbo && drawbuffers && npot &&
            shading_language_100 && shader_objects && fragment_shader && fbo &&
            texture_float;
          
          if(this->SupportedByHardware)
            {
              mgr->LoadSupportedExtension("GL_VERSION_1_3");
              if(gl15)
                {
                  mgr->LoadSupportedExtension("GL_VERSION_1_5");
                }
              else
                {
                  mgr->LoadCorePromotedExtension("GL_ARB_vertex_buffer_object");
                }
              if(gl20)
                {
                  mgr->LoadSupportedExtension("GL_VERSION_2_0");
                }
              else
                {
                  mgr->LoadCorePromotedExtension("GL_ARB_draw_buffers");
                  // npot does not provide new functions.
                  mgr->LoadCorePromotedExtension("GL_ARB_shading_language_100");
                  mgr->LoadCorePromotedExtension("GL_ARB_shader_objects");
                  mgr->LoadCorePromotedExtension("GL_ARB_fragment_shader");
                }
              if(gl21)
                {
                  mgr->LoadSupportedExtension("GL_VERSION_2_1");
                }
              else
                {
                  mgr->LoadCorePromotedExtension("GL_ARB_pixel_buffer_object");
                }
              mgr->LoadExtension("GL_EXT_framebuffer_object");
              mgr->LoadExtension( "GL_ARB_texture_float" );
            }
        }
      mgr->Delete();
    }
  else
    {
      this->SupportedByHardware=false;
    }
}

// ----------------------------------------------------------------------------
// Description:
// Tells if the GPU implementation is supported by the graphics card in use.
bool vtkKWEGPUArrayCalculator::GetSupportedByHardware()
{
  return this->SupportedByHardware;
}

// ----------------------------------------------------------------------------
// Description:
// Compute range of values for a scalar array for a subset of tuples.
void vtkKWEGPUArrayCalculator::ComputeSubRange(vtkDataArray *array,
                                               vtkIdType first,
                                               vtkIdType last,
                                               double subRange[2])
{
  assert("pre: array_exists" && array!=0);
  assert("pre: scalar" && array->GetNumberOfComponents()==1);
  assert("pre: array_not_empty" && array->GetNumberOfTuples()>=1);
  assert("pre: positive_first" && first>=0);
  assert("pre: last_in_range" && last<array->GetNumberOfTuples());
  assert("pre: increasing order" && first<=last);

  subRange[0]=VTK_DOUBLE_MAX;
  subRange[1]=VTK_DOUBLE_MIN;
  vtkIdType i=first;
  while(i<=last)
    {
      double value=array->GetComponent(i,0);
      if(value<subRange[0])
        {
          subRange[0]=value;
        }
      if(value>subRange[1])
        {
          subRange[1]=value;
        }
      ++i;
    }
}

// ----------------------------------------------------------------------------
// Description:
// Tell the filter to calibrate itself. Starting for the current value of
// SizeThreshold, try to find the actual threshold size at which the GPU
// implementation is faster than the CPU one. This call is expensive.
// This method is usually called automatically once when UseCalibration is
// on. But you can call it directly to control at what time the calibration
// happens or to force a new calibration.
void vtkKWEGPUArrayCalculator::Calibrate()
{
  if(!this->SupportedByHardware)
    {
      // cannot calibrate.
      this->CalibratedSizeThreshold=0; // reset to initial value.
      this->CalibrationDone=false;
    }

  vtkTimerLog *timer=vtkTimerLog::New();

  // Starting value. Start from last estimate if not null.
  if(this->CalibratedSizeThreshold==0)
    {
      if(this->SizeThreshold>0)
        {
          this->CalibratedSizeThreshold=this->SizeThreshold;
        }
      else
        {
          this->CalibratedSizeThreshold=15000; // estimate from experience.
        }
    }

  double cpuTime=0.0;
  double gpuTime=0.0;

  vtkFloatArray *values=0;

  const int maxIterations=6; // limit the calibration to 6 passes.
  int iteration=0;

  vtkIdType minSize=0;
  vtkIdType maxSize=100000; // resonable upper limit.

  while(!this->CalibrationDone)
    {
      // cpu computation: fill an array with values.
      timer->StartTimer();

      values=vtkFloatArray::New();
      values->SetNumberOfComponents(1);
      values->SetNumberOfTuples(this->CalibratedSizeThreshold);

      vtkIdType i=0;
      while(i<this->CalibratedSizeThreshold)
        {
          double x=static_cast<double>(i);
          values->SetValue(i,static_cast<float>(x));
          x=values->GetValue(i);
          double y;
          if(i>0)
            {
              y=values->GetValue(i-1);
            }
          else
            {
              y=0.0;
            }
          x=exp(sqrt(sin(x)*cos(exp(y))+10.0));
          values->SetValue(i,static_cast<float>(x));
          ++i;
        }
      timer->StopTimer();
      cpuTime=timer->GetElapsedTime();
      cpuTime=cpuTime*10.0; // simulate overhead of bytecode execution.

      timer->StartTimer();
      this->SimulateGPUComputation(values);
      timer->StopTimer();
      gpuTime=timer->GetElapsedTime();

      values->Delete();

      double speedFactor=cpuTime/gpuTime;
      cout<<"iteration="<<iteration<<" speedFactor="<<speedFactor<<" CalibratedSizeThreshold="<<this->CalibratedSizeThreshold<<" cpu="<<cpuTime<<" gpu="<<gpuTime<<endl;

      ++iteration;
      // done if the ratio is 10% apart equal time (between 0.9 and 1.1)
      // or if we reach the maximum number of tries.
      this->CalibrationDone=(speedFactor>=0.9 && speedFactor<=1.1)
        || iteration>=maxIterations;
      
      if(!this->CalibrationDone)
        {
          if(speedFactor<1.0)
            {
              minSize=this->CalibratedSizeThreshold;
              this->CalibratedSizeThreshold=
                (this->CalibratedSizeThreshold+maxSize)/2;
            }
          else
            {
              maxSize=this->CalibratedSizeThreshold;
              this->CalibratedSizeThreshold=
                (minSize+this->CalibratedSizeThreshold)/2;
            }
        }
    }
  timer->Delete();
}

// ----------------------------------------------------------------------------
// gpu computation:
// 1.copy the array into a PBO.
// 2. copy the PBO into a texture.
// 3. copy the texture back into the PBO
// 4. copy the PBO back into the array.
void vtkKWEGPUArrayCalculator::SimulateGPUComputation(vtkFloatArray *values)
{
  vtkKWEDataArrayStreamer *streamer=vtkKWEDataArrayStreamer::New();
  GLint value;

  const int accumulatedTupleSize=sizeof(float);
  vtkIdType numTuples=values->GetNumberOfTuples();
  int maxTupleSize=accumulatedTupleSize;

  glGetIntegerv(vtkgl::MAX_3D_TEXTURE_SIZE,&value);
  streamer->SetMax3DTextureSize(value);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&value);
  streamer->SetMaxTextureSize(value);
  streamer->SetMaxTextureMemorySizeInBytes(0);
  streamer->SetMaxNumberOfTuplesPerBlock(this->MaxGPUMemorySizeInBytes
                                           /accumulatedTupleSize);
  streamer->SetNumberOfTuples(numTuples);

  vtkKWEDataTransferHelper *bus=vtkKWEDataTransferHelper::New();
  bus->SetContext(this->Context);
  bus->SetMinTextureDimension(2);
  bus->SetArray(values);
  bus->SetShaderSupportsTextureInt(false);

  vtkKWEDataTransferHelper *resultBus=vtkKWEDataTransferHelper::New();
  resultBus->SetContext(this->Context);
  resultBus->SetMinTextureDimension(2);
  resultBus->SetArray(values);
  resultBus->SetShaderSupportsTextureInt(false);

  streamer->SetTupleSize(maxTupleSize);
  
  streamer->Begin();
  vtkIdType dims[2];
  bool firstIteration=true;

  vtkFrameBufferObject *fbo=0;
  int fboDims[2];
  fboDims[0]=0; // to remove warning
  fboDims[1]=0; // to remove warning

  while(!streamer->IsDone())
    {
      streamer->GetChunkDimensions(dims);
      vtkIdType cursor=streamer->GetCursor();

#ifdef VTKKWE_GPU_ARRAY_CALCULATOR_DEBUG
      cout<<"stream block start at cursor="<<cursor<<" size="<<(dims[0]*dims[1])<<endl;
#endif
      if(firstIteration)
        {
          // Create the FBO. The extent of the first step of the iteration
          // is the largest one.

          // Create texture to render into.
          vtkTextureObject *resultPlane=vtkTextureObject::New();
          resultPlane->SetContext(this->Context);
          resultPlane->Create2D(static_cast<int>(dims[0]),
                                static_cast<int>(dims[1]),1,
                                VTK_FLOAT,false);
          resultBus->SetTexture(resultPlane);
          resultPlane->Delete();

          fboDims[0]=static_cast<int>(dims[0]);
          fboDims[1]=static_cast<int>(dims[1]);

          // Create fbo to render.
          fbo=vtkFrameBufferObject::New();
          fbo->SetDepthBufferNeeded(false);
          fbo->SetContext(vtkOpenGLRenderWindow::SafeDownCast(this->Context));
          fbo->SetColorBuffer(0,resultPlane);
        }
         
      vtkgl::ActiveTexture(vtkgl::TEXTURE0);
      // code to upload texture here.
      bus->SetArray(values);
      bus->SetCPUExtent(0,static_cast<int>(numTuples-1),0,0,0,0);
      bus->SetGPUExtent(static_cast<int>(cursor),
                        static_cast<int>(cursor+dims[0]*dims[1]-1),0,0,0,0);
      bus->SetTextureExtent(0,static_cast<int>(dims[0]-1),0,
                            static_cast<int>(dims[1]-1),0,0);
      int component=0;
      bus->Upload(1,&component);
      bus->GetTexture()->Bind();
     
      // Run GPU computation by drawing a quad
      fbo->Start(fboDims[0],fboDims[1],false);
      //      fbo->RenderQuad(0,dims[0]-1,0,dims[1]-1);

      // Get the result of the GPU, by downloading the texture.
      bus->SetCPUExtent(0,static_cast<int>(numTuples-1),0,0,0,0);
      bus->SetGPUExtent(static_cast<int>(cursor),
                        static_cast<int>(cursor+dims[0]*dims[1]-1),0,0,0,0);
      bus->SetTextureExtent(0,static_cast<int>(dims[0]-1),0,
                            static_cast<int>(dims[1]-1),0,0);
      bus->Download();

      firstIteration=false;
      streamer->Next();
    }
  if(fbo!=0)
    {
      fbo->Delete(); // do unbind and restore GL resources.
    }
  
  streamer->Delete();
  bus->Delete();
  resultBus->Delete();
}

// ----------------------------------------------------------------------------
int vtkKWEGPUArrayCalculator::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  if(!this->SupportedByHardware)
    {
      return this->Superclass::RequestData(request,inputVector,outputVector);
    }

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject *output = outInfo->Get(vtkDataObject::DATA_OBJECT());

  int resultType = 0; // 0 for scalar, 1 for vector
  int attributeDataType = 0; // 0 for point data, 1 for cell data
  vtkIdType i;
  int j;
  
  vtkDataSetAttributes* inFD = 0;
  vtkDataSetAttributes* outFD = 0;
  vtkDataArray* currentArray;
  vtkIdType numTuples = 0;
  vtkDataArray* resultArray = 0;
  vtkPoints* resultPoints = 0;

  this->FunctionParserToGLSL->SetReplaceInvalidValues(this->ReplaceInvalidValues);
  this->FunctionParserToGLSL->SetReplacementValue(this->ReplacementValue);

  vtkDataSet *dsInput = vtkDataSet::SafeDownCast(input);
  vtkDataSet *dsOutput = vtkDataSet::SafeDownCast(output);
  vtkGraph *graphInput = vtkGraph::SafeDownCast(input);
  vtkGraph *graphOutput = vtkGraph::SafeDownCast(output);
  vtkPointSet* psInput = vtkPointSet::SafeDownCast(input);
  vtkPointSet* psOutput = vtkPointSet::SafeDownCast(output);
  if (dsInput)
    {
    if (this->AttributeMode == VTK_ATTRIBUTE_MODE_DEFAULT ||
        this->AttributeMode == VTK_ATTRIBUTE_MODE_USE_POINT_DATA)
      {
      inFD = dsInput->GetPointData();
      outFD = dsOutput->GetPointData();
      attributeDataType = 0;
      numTuples = dsInput->GetNumberOfPoints();
      }
    else
      {
      inFD = dsInput->GetCellData();
      outFD = dsOutput->GetCellData();
      attributeDataType = 1;
      numTuples = dsInput->GetNumberOfCells();
      }
    }
  else if (graphInput)
    {
    if (this->AttributeMode == VTK_ATTRIBUTE_MODE_DEFAULT ||
        this->AttributeMode == VTK_ATTRIBUTE_MODE_USE_VERTEX_DATA)
      {
      inFD = graphInput->GetVertexData();
      outFD = graphOutput->GetVertexData();
      attributeDataType = 0;
      numTuples = graphInput->GetNumberOfVertices();
      }
    else
      {
      inFD = graphInput->GetEdgeData();
      outFD = graphOutput->GetEdgeData();
      attributeDataType = 1;
      numTuples = graphInput->GetNumberOfEdges();
      }
    }
  
  if (numTuples < 1)
    {
    vtkDebugMacro("Empty data.");
    return 1;
    }

  if(this->UseCalibration)
    {
      if(!this->CalibrationDone)
        {
          this->Calibrate();
        }
    }

  if((!this->UseCalibration && numTuples<SizeThreshold)
     || (this->UseCalibration && numTuples<this->CalibratedSizeThreshold))
    {
      // CPU flavor.
      return this->Superclass::RequestData(request,inputVector,outputVector);
    }
  
  for (i = 0; i < this->NumberOfScalarArrays; i++)
    {
    currentArray = inFD->GetArray(this->ScalarArrayNames[i]);
    if (currentArray)
      {
      if (currentArray->GetNumberOfComponents() >
          this->SelectedScalarComponents[i])
        {
        this->FunctionParserToGLSL->
          SetScalarVariableValue(
            this->ScalarVariableNames[i],
            currentArray->GetComponent(0, this->SelectedScalarComponents[i]));
        }
      else
        {
        vtkErrorMacro("Array " << this->ScalarArrayNames[i]
                      << " does not contain the selected component.");
        return 1;
        }
      }
    else
      {
      vtkErrorMacro("Invalid array name: " << this->ScalarArrayNames[i]);
      return 1;
      }
    }

  for (i = 0; i < this->NumberOfVectorArrays; i++)
    {
    currentArray = inFD->GetArray(this->VectorArrayNames[i]);
    if (currentArray)
      {
      if ((currentArray->GetNumberOfComponents() >
           this->SelectedVectorComponents[i][0]) &&
          (currentArray->GetNumberOfComponents() >
           this->SelectedVectorComponents[i][1]) &&
          (currentArray->GetNumberOfComponents() >
           this->SelectedVectorComponents[i][2]))
        {
        this->FunctionParserToGLSL->
          SetVectorVariableValue(
            this->VectorVariableNames[i],
            currentArray->GetComponent(0, this->SelectedVectorComponents[i][0]),
            currentArray->GetComponent(0, this->SelectedVectorComponents[i][1]),
            currentArray->GetComponent(0, this->SelectedVectorComponents[i][2]));
        }
      else
        {
        vtkErrorMacro("Array " << this->VectorArrayNames[i]
                      << " does not contain one of the selected components.");
        return 1;
        }
      }
    else
      {
      vtkErrorMacro("Invalid array name: " << this->VectorArrayNames[i]);
      return 1;
      }
    }

   // we can add points
  if(attributeDataType == 0)
    {
    for (i = 0; i < this->NumberOfCoordinateScalarArrays; i++)
      {
      double* pt = 0;
      if (dsInput)
        {
        pt = dsInput->GetPoint(0);
        }
      else
        {
        pt = graphInput->GetPoint(0);
        }
      this->FunctionParserToGLSL->
        SetScalarVariableValue(
          this->CoordinateScalarVariableNames[i],
          pt[this->SelectedCoordinateScalarComponents[i]]);
      }

    for (i = 0; i < this->NumberOfCoordinateVectorArrays; i++)
      {
      double* pt = 0;
      if (dsInput)
        {
        pt = dsInput->GetPoint(0);
        }
      else
        {
        pt = graphInput->GetPoint(0);
        }
      this->FunctionParserToGLSL->
        SetVectorVariableValue(
          this->CoordinateVectorVariableNames[i],
          pt[this->SelectedCoordinateVectorComponents[i][0]],
          pt[this->SelectedCoordinateVectorComponents[i][1]],
          pt[this->SelectedCoordinateVectorComponents[i][2]]);
      }
    }

  this->FunctionParserToGLSL->GenerateGLSL();
  if(!this->FunctionParserToGLSL->GetParseStatus())
    {
      return 0;
    }
  
  // output allocation.
  if(this->FunctionParserToGLSL->GetResultDimension()==3)
    {
      resultType=1;
    }
  else
    {
      resultType=0;
    }

  // The number of arrays in input is limited by the maximum number of
  // texture units
  GLint ivalue[4];

  glGetIntegerv(vtkgl::MAX_TEXTURE_IMAGE_UNITS,ivalue);
  // Count the number of arrays used in the expression.
  int usedVariables=this->FunctionParserToGLSL->GetNumberOfUsedScalarVariables()
    +this->FunctionParserToGLSL->GetNumberOfUsedVectorVariables();

  if(usedVariables>ivalue[0])
    {
      vtkErrorMacro(<<" the expression uses " << usedVariables << " but the GPU can process only" << ivalue[0] <<" variables.");
      return 0;
    }

  // iterate first over all used variables (scalar or vector) to
  // find the maximum size for a tuple in bytes.
  // also accumulate size of a tuple when combining all used inputs and the
  // output.

  int maxTupleSize=1; // 1 component of type signed/unsigned char.
  int accumulatedTupleSize=0;
  int c=this->FunctionParserToGLSL->GetNumberOfScalarVariables();
  j=0;
  while(j<c)
    {
      if(this->FunctionParserToGLSL->GetScalarIsUsed(j))
        {
           currentArray=inFD->GetArray(this->ScalarArrayNames[j]);
           if (currentArray)
             {
               int tupleSize=currentArray->GetDataTypeSize();
               if(currentArray->GetDataType()==VTK_DOUBLE)
                 {
                   // pbo will convert double into float
                   tupleSize/=2;
                 }
               if(tupleSize>maxTupleSize)
                 {
                   maxTupleSize=tupleSize;
                 }
               accumulatedTupleSize+=tupleSize;
             }
           else
             {
               vtkErrorMacro("Invalid array name: " << this->ScalarArrayNames[j]);
               return 0;
             }
        }
      ++j;
    }

  c=this->FunctionParserToGLSL->GetNumberOfVectorVariables();
  j=0;
  while(j<c)
    {
      if(this->FunctionParserToGLSL->GetVectorIsUsed(j))
        {
           currentArray=inFD->GetArray(this->VectorArrayNames[j]);
           if (currentArray)
             {
               int tupleSize=currentArray->GetDataTypeSize();
               if(currentArray->GetDataType()==VTK_DOUBLE)
                 {
                   // pbo will convert double into float
                   tupleSize/=2;
                 }
               tupleSize*=3; // vector
               if(tupleSize>maxTupleSize)
                 {
                   maxTupleSize=tupleSize;
                 }
               accumulatedTupleSize+=tupleSize;
             }
           else
             {
               vtkErrorMacro("Invalid array name: " << this->VectorArrayNames[j]);
               return 0;
             }
        }
      ++j;
    }
  
  // add the size of the output (component is always float):
  if(resultType==1)
    {
      accumulatedTupleSize+=12; // (3*4)
    }
  else
    {
      accumulatedTupleSize+=4;
    }

  if(resultType == 1 && CoordinateResults != 0 && (psOutput || graphOutput))
    {
    resultPoints = vtkPoints::New();
    resultPoints->SetNumberOfPoints(numTuples);
    resultArray = resultPoints->GetData();
    }
  else if(CoordinateResults != 0)
    {
    if(resultType != 1)
      {
      vtkErrorMacro("Coordinate output specified, "
                    "but there are no vector results");
      }
    else if(!psOutput)
      {
      vtkErrorMacro("Coordinate output specified, "
                    "but output is not polydata or unstructured grid");
      }
    return 1;
    }
  else
    {
    resultArray=
      vtkDataArray::SafeDownCast(vtkAbstractArray::CreateArray(this->ResultArrayType));
    }

  if (resultType == 0)
    {
    resultArray->SetNumberOfComponents(1);
    resultArray->SetNumberOfTuples(numTuples);
    }
  else
    {
    resultArray->Allocate(numTuples * 3);
    resultArray->SetNumberOfComponents(3);
    }

  vtkShaderProgram2 *prog=vtkShaderProgram2::New();
  prog->SetContext(static_cast<vtkOpenGLRenderWindow *>(this->Context));
  vtkShader2 *shader=vtkShader2::New();
  shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
  shader->SetSourceCode(this->FunctionParserToGLSL->GetGLSLCode()->c_str());
  shader->SetContext(prog->GetContext());
  prog->GetShaders()->AddItem(shader);
  shader->Delete();
  
  prog->Build();
  if(prog->GetLastBuildStatus()!=VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
    {
    vtkErrorMacro("shader build error.");
    return 0;
    }


  vtkKWEDataArrayStreamer *streamer=vtkKWEDataArrayStreamer::New();
  GLint value;
  glGetIntegerv(vtkgl::MAX_3D_TEXTURE_SIZE,&value);
  vtkGraphicErrorMacro(this->Context,"__FILE__ __LINE__");
  streamer->SetMax3DTextureSize(value);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&value);
  vtkGraphicErrorMacro(this->Context,"__FILE__ __LINE__");
  streamer->SetMaxTextureSize(value);
  streamer->SetMaxTextureMemorySizeInBytes(0);
  streamer->SetMaxNumberOfTuplesPerBlock(this->MaxGPUMemorySizeInBytes
                                           /accumulatedTupleSize);
  streamer->SetNumberOfTuples(numTuples);

  vtkstd::vector<vtkKWEDataTransferHelper *> *buses=
    new vtkstd::vector<vtkKWEDataTransferHelper *>(this->FunctionParserToGLSL->GetNumberOfUsedScalarVariables()+this->FunctionParserToGLSL->GetNumberOfUsedVectorVariables());

  size_t busIndex=0;
  size_t numberOfBuses=buses->size();
  while(busIndex<numberOfBuses)
    {
      (*buses)[busIndex]=vtkKWEDataTransferHelper::New();
      (*buses)[busIndex]->SetContext(this->Context);
      (*buses)[busIndex]->SetMinTextureDimension(2);
      ++busIndex;
    }

  vtkKWEDataTransferHelper *resultBus=vtkKWEDataTransferHelper::New();
  resultBus->SetContext(this->Context);
  resultBus->SetArray(resultArray);
  resultBus->SetShaderSupportsTextureInt(false);
  resultBus->SetMinTextureDimension(2);

  streamer->SetTupleSize(maxTupleSize);
  


  streamer->Begin();
  vtkIdType dims[2];
  bool firstIteration=true;

  vtkFrameBufferObject *fbo=0;
  int fboDims[2];
  fboDims[0]=0; // to remove warning
  fboDims[1]=0; // to remove warning

  while(!streamer->IsDone())
    {
      streamer->GetChunkDimensions(dims);
      vtkIdType cursor=streamer->GetCursor();

#ifdef VTKKWE_GPU_ARRAY_CALCULATOR_DEBUG
      cout<<"stream block start at cursor="<<cursor<<" size="<<(dims[0]*dims[1])<<endl;
#endif
      if(firstIteration)
        {
          // Create the FBO. The extent of the first step of the iteration
          // is the largest one.

          // Create texture to render into.
          vtkTextureObject *resultPlane=vtkTextureObject::New();
          resultPlane->SetContext(this->Context);
          resultPlane->Create2D(static_cast<unsigned int>(dims[0]),
                                static_cast<unsigned int>(dims[1]), 
                                this->FunctionParserToGLSL->GetResultDimension(),
                                VTK_FLOAT,false);
          resultBus->SetTexture(resultPlane);
          resultPlane->Delete();

          fboDims[0]=static_cast<int>(dims[0]);
          fboDims[1]=static_cast<int>(dims[1]);

          // Create fbo to render.
          fbo=vtkFrameBufferObject::New();
          fbo->SetDepthBufferNeeded(false);
          fbo->SetContext(vtkOpenGLRenderWindow::SafeDownCast(this->Context));
          fbo->SetColorBuffer(0,resultPlane);
        }

      int textureUnit=-1;
      c=this->FunctionParserToGLSL->GetNumberOfScalarVariables();
      j=0;
      while(j<c)
        {
          if(this->FunctionParserToGLSL->GetScalarIsUsed(j))
            {
              ++textureUnit;
              vtkgl::ActiveTexture(vtkgl::TEXTURE0+textureUnit);
              currentArray=inFD->GetArray(this->ScalarArrayNames[j]);
              if (currentArray)
                {
                  if(firstIteration)
                    {
                    prog->GetUniformVariables()->SetUniformi(this->FunctionParserToGLSL->GetGLSLScalarName(j)->c_str(),1,&textureUnit);
                    }
                  // code to upload texture here.
                  vtkKWEDataTransferHelper *d=(*buses)[textureUnit];
                  d->SetArray(currentArray);
                  d->SetCPUExtent(0,static_cast<int>(numTuples-1),0,0,0,0);
                  d->SetGPUExtent(static_cast<int>(cursor),
                                  static_cast<int>(cursor+dims[0]*dims[1]-1),
                                  0,0,0,0);
                  d->SetTextureExtent(0,static_cast<int>(dims[0]-1),0,
                                      static_cast<int>(dims[1]-1),0,0);
                  int component=this->SelectedScalarComponents[j];
                  d->Upload(1,&component);
                  d->GetTexture()->Bind();
                }
              else
                {
                  vtkErrorMacro("Invalid array name: " << this->ScalarArrayNames[j]);
                  return 0;
                }
            }
          ++j;
        }

      c=this->FunctionParserToGLSL->GetNumberOfVectorVariables();
      j=0;
      while(j<c)
        {
          if(this->FunctionParserToGLSL->GetVectorIsUsed(j))
            {
              ++textureUnit;
              vtkgl::ActiveTexture(vtkgl::TEXTURE0+textureUnit);
              currentArray=inFD->GetArray(this->VectorArrayNames[j]);
              if (currentArray)
                {
                  if(firstIteration)
                    {
                    prog->GetUniformVariables()->SetUniformi(this->FunctionParserToGLSL->GetGLSLVectorName(j)->c_str(),1,&textureUnit);
                    }
                  // code to upload texture here.
                  vtkKWEDataTransferHelper *d=(*buses)[textureUnit];
                  d->SetArray(currentArray);
                  d->SetCPUExtent(0,static_cast<int>(numTuples-1),0,0,0,0);
                  d->SetGPUExtent(static_cast<int>(cursor),
                                  static_cast<int>(cursor+dims[0]*dims[1]-1),
                                  0,0,0,0);
                  d->SetTextureExtent(0,static_cast<int>(dims[0]-1),0,
                                      static_cast<int>(dims[1]-1),0,0);
                  int *components=this->SelectedVectorComponents[j];
                  d->Upload(3,components);
                  d->GetTexture()->Bind();
                }
              else
                {
                  vtkErrorMacro("Invalid array name: " << this->VectorArrayNames[j]);
                  return 0;
                }
            }
          ++j;
        }
      
      // Run GPU computation by drawing a quad
      prog->Use();
      if(!prog->IsValid())
        {
        vtkErrorMacro(<<" validation of the program failed: "<<prog->GetLastValidateLog());
        }
      
      fbo->Start(fboDims[0],fboDims[1],false);
      fbo->RenderQuad(0,static_cast<int>(dims[0]-1),0,
                      static_cast<int>(dims[1]-1));
      prog->Restore();
      
      // Get the result of the GPU, by downloading the texture.
      resultBus->SetCPUExtent(0,static_cast<int>(numTuples-1),0,0,0,0);
      resultBus->SetGPUExtent(static_cast<int>(cursor),
                              static_cast<int>(cursor+dims[0]*dims[1]-1),
                              0,0,0,0);
      resultBus->SetTextureExtent(0,static_cast<int>(dims[0]-1),0,
                                  static_cast<int>(dims[1]-1),0,0);
      resultBus->Download();

      firstIteration=false;
      streamer->Next();
    }
  if(fbo!=0)
    {
    fbo->Delete(); // do unbind and restore GL resources.
    }
  if(prog!=0)
    {
    prog->ReleaseGraphicsResources();
    prog->Delete(); // do unbind and restore GL resources.
    }
  
  streamer->Delete();

  busIndex=0;
  while(busIndex<numberOfBuses)
    {
      (*buses)[busIndex]->Delete();
      ++busIndex;
    }
  delete buses;

  resultBus->Delete();
  
  if(resultPoints)
    {
    if(psInput)
      {
      if(attributeDataType == 0)
        {
        psOutput->CopyStructure(psInput);
        }
      else
        {
        vtkPolyData* pd = vtkPolyData::SafeDownCast(psOutput);
        vtkUnstructuredGrid* ug = vtkUnstructuredGrid::SafeDownCast(psOutput);
        if(pd)
          {
          pd->Reset();
          pd->Allocate(numTuples);
          for (i = 1; i < numTuples; i++)
            {
            pd->InsertNextCell(VTK_VERTEX, 1, &i);
            }
          }
        else if(ug)
          {
          ug->Reset();
          ug->Allocate(numTuples);
          for (i = 1; i < numTuples; i++)
            {
            ug->InsertNextCell(VTK_VERTEX, 1, &i);
            }
          }
        }
      psOutput->SetPoints(resultPoints);
      }
    else
      {
      graphOutput->CopyStructure(graphInput);
      }
    outFD->PassData(inFD);
    resultPoints->Delete();
    }
  else
    {
    dsOutput->CopyStructure(dsInput);
    dsOutput->CopyAttributes(dsInput);
    
    resultArray->SetName(this->ResultArrayName);
    outFD->AddArray(resultArray);
    if (resultType == 0)
      {
      outFD->SetActiveScalars(this->ResultArrayName);
      }
    else
      {
      outFD->SetActiveVectors(this->ResultArrayName);
      }
    
    resultArray->Delete();
    }

  return 1;
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::SetFunction(const char* function)
{
  if (this->Function && function &&
      strcmp(this->Function, function) == 0)
    {
    return;
    }

  this->Modified();
  
  if (this->Function)
    {
    delete [] this->Function;
    this->Function = NULL;
    }
  
  if (function)
    {
    this->Function = new char[strlen(function)+1];
    strcpy(this->Function, function);
    this->FunctionParser->SetFunction(this->Function);
    this->FunctionParserToGLSL->SetFunction(this->Function);
    }
}


// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::RemoveScalarVariables()
{
  this->Superclass::RemoveScalarVariables();
  this->FunctionParserToGLSL->RemoveScalarVariables();
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::RemoveVectorVariables()
{
  this->Superclass::RemoveVectorVariables();
  this->FunctionParserToGLSL->RemoveVectorVariables();
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::RemoveCoordinateScalarVariables()
{
  this->Superclass::RemoveCoordinateScalarVariables();
  this->FunctionParserToGLSL->RemoveScalarVariables();
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::RemoveCoordinateVectorVariables()
{
  this->Superclass::RemoveCoordinateVectorVariables();
  this->FunctionParserToGLSL->RemoveVectorVariables();
}

// ----------------------------------------------------------------------------
void vtkKWEGPUArrayCalculator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Max GPU Memory Size In Bytes: " 
     << this->MaxGPUMemorySizeInBytes << endl;
}
