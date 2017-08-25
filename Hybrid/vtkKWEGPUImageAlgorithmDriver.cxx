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
#include "vtkKWEGPUImageAlgorithmDriver.h"

#include "vtkDataArray.h"
#include "vtkDebugLeaks.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkKWEDataTransferHelper.h"
#include "vtkKWEExtentCalculator.h"
#include "vtkKWEStructuredExtent.h"
#include "vtkTextureObject.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkToolkits.h"

vtkSmartPointer<vtkTimerLog> mylog = vtkSmartPointer<vtkTimerLog>::New();
#define START_LOG()\
  mylog->StartTimer();
#define STOP_LOG(msg)\
  glFinish();\
  mylog->StopTimer();\
  cout << msg << " " << mylog->GetElapsedTime() << endl;

#define PRINTEXT(__arg) \
  __arg[0] << ", " << __arg[1] << ", " << __arg[2] << ", " << __arg[3] << ", " << __arg[4] << ", " << __arg[5]

#include <vtkstd/list>
//----------------------------------------------------------------------------
class vtkKWEGPUImageAlgorithmDriver::vtkPipe
{
private:
  vtkKWEGPUImageAlgorithmDriver* Driver;
  bool AbortFlag;
  struct vtkPipeItem
    {
    vtkSmartPointer<vtkKWEGPUImageAlgorithmDriver::vtkBuses> UpBuses;
    vtkSmartPointer<vtkKWEDataTransferHelper> Downbus;
    };

  vtkstd::list<vtkPipeItem> InPipe;
  vtkstd::list<vtkPipeItem> ExecutePipe;
  vtkstd::list<vtkPipeItem> DownloadPipe;
  vtkstd::list<vtkPipeItem> DownloadPipe2;
  vtkstd::list<vtkSmartPointer<vtkKWEGPUImageAlgorithmDriver::vtkBuses> > AvailableUpBuses;

  bool Chug()
    {
    bool result = (!this->AbortFlag);
    if (this->DownloadPipe2.size() > 0)
      {
      START_LOG();
      vtkPipeItem item = this->DownloadPipe2.front();
      this->DownloadPipe2.pop_front();
      // Note that Download() is not going to be called unless result == true
      // i.e. this->AbortFlag == false.
      result = result && item.Downbus->DownloadAsync2();
      STOP_LOG("Download2");
      }

    if (this->DownloadPipe.size() > 0)
      {
      vtkPipeItem item = this->DownloadPipe.front();
      this->DownloadPipe.pop_front();

      // Push it in the available buses.
      this->AvailableUpBuses.push_back(item.UpBuses);

      // release the bus.
      item.UpBuses = NULL;
     
      START_LOG();
      // Note that Download() is not going to be called unless result == true
      // i.e. this->AbortFlag == false.
      result = result && item.Downbus->DownloadAsync1();
      this->DownloadPipe2.push_back(item);
      STOP_LOG("Download1");
      }

    if (this->ExecutePipe.size() > 0)
      {
      START_LOG();
      vtkPipeItem item = this->ExecutePipe.front();
      this->ExecutePipe.pop_front();
      // Note that Execute() is not going to be called unless result == true
      // i.e. this->AbortFlag == false.
      result = result && this->Driver->Execute(item.UpBuses, item.Downbus);
      this->DownloadPipe.push_back(item);
      STOP_LOG("Execute:");
      }

    if (this->InPipe.size() > 0)
      {
      vtkPipeItem item = this->InPipe.front();
      this->InPipe.pop_front();
      this->ExecutePipe.push_back(item);
      }

    return result;
    }
public:
  vtkPipe(vtkKWEGPUImageAlgorithmDriver* driver)
    {
    this->AbortFlag = false;
    this->Driver = driver;
    }

  ~vtkPipe()
    {
    this->Driver = 0;
    this->AbortFlag = true;
    this->Flush();
    }

  vtkKWEGPUImageAlgorithmDriver::vtkBuses* NewBus()
    {
    if (this->AvailableUpBuses.size() > 0)
      {
      vtkKWEGPUImageAlgorithmDriver::vtkBuses* front = this->AvailableUpBuses.front();
      front->Register(0);
      this->AvailableUpBuses.erase(this->AvailableUpBuses.begin());
      return front;
      }
    return vtkKWEGPUImageAlgorithmDriver::vtkBuses::New();
    }


  bool Push(
    vtkKWEGPUImageAlgorithmDriver::vtkBuses* up_buses, vtkKWEDataTransferHelper* down_bus)
    {
    vtkPipeItem item;
    item.UpBuses= up_buses;
    item.Downbus = down_bus;
    this->InPipe.push_back(item);
    return this->Chug();
    }

  void Flush()
    {
    while (this->InPipe.size() > 0 || this->ExecutePipe.size() > 0 || 
      this->DownloadPipe.size() > 0 || this->DownloadPipe2.size() > 0)
      {
      this->Chug();
      }
    this->AvailableUpBuses.clear();
    }

  void Abort()
    {
    this->AbortFlag = true;
    }
};

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEGPUImageAlgorithmDriver::vtkBuses, "$Revision: 1.1$");

vtkKWEGPUImageAlgorithmDriver::vtkBuses* vtkKWEGPUImageAlgorithmDriver::vtkBuses::New()
{
#ifdef VTK_DEBUG_LEAKS
  vtkDebugLeaks::ConstructClass("vtkBuses");
#endif
  return new vtkBuses();
}

//----------------------------------------------------------------------------
void vtkKWEGPUImageAlgorithmDriver::vtkBuses::SetNumberOfPorts(unsigned int num)
{
  this->Buses.resize(num);
}

//----------------------------------------------------------------------------
void vtkKWEGPUImageAlgorithmDriver::vtkBuses::SetNumberOfConnections(
  unsigned int port, unsigned int num)
{
  if (port < this->Buses.size())
    {
    this->Buses[port].resize(num);
    }
}

//----------------------------------------------------------------------------
vtkKWEDataTransferHelper* vtkKWEGPUImageAlgorithmDriver::vtkBuses::GetBus(
  unsigned int port, unsigned int conn)
{
  if (port < this->Buses.size())
    {
    if (conn < this->Buses[port].size())
      {
      return this->Buses[port][conn];
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEGPUImageAlgorithmDriver::vtkBuses::SetBus(
  unsigned int port, unsigned int conn, vtkKWEDataTransferHelper* bus)
{
  if (port < this->Buses.size())
    {
    if (conn < this->Buses[port].size())
      {
      this->Buses[port][conn] = bus;
      }
    }
}

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEGPUImageAlgorithmDriver, "$Revision: 706 $");
//----------------------------------------------------------------------------
vtkKWEGPUImageAlgorithmDriver::vtkKWEGPUImageAlgorithmDriver()
{
  this->Context = 0;
  this->OwnContext = false;
}

//----------------------------------------------------------------------------
vtkKWEGPUImageAlgorithmDriver::~vtkKWEGPUImageAlgorithmDriver()
{
  this->SetContext(0);
}

//----------------------------------------------------------------------------
void vtkKWEGPUImageAlgorithmDriver::SetContext(vtkRenderWindow* context)
{
  if (this->Context == context)
    {
    return;
    }

  if (this->Context && this->OwnContext)
    {
    this->Context->Delete();
    }
  this->Context = context;
  this->OwnContext = false;

  // Don't call Modified() since this may get called in RequestData().
  // this->Modified();
}

//----------------------------------------------------------------------------
vtkRenderWindow* vtkKWEGPUImageAlgorithmDriver::GetContext()
{
  return this->Context;
}

//----------------------------------------------------------------------------
  int vtkKWEGPUImageAlgorithmDriver::RequestUpdateExtent (
    vtkInformation * vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
  int update_ext[6];
  outputVector->GetInformationObject(0)->Get(
    vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), update_ext);

  int numInputPorts = this->GetNumberOfInputPorts();
  for (int port=0; port < numInputPorts; port++)
    {
    int numConns = inputVector[port]->GetNumberOfInformationObjects();
    for (int conn = 0; conn < numConns; conn++)
      {
      vtkInformation* inInfo = inputVector[port]->GetInformationObject(conn);
      int input_update_extent[6];
      if (!this->MapOutputExtentToInput(input_update_extent,
          port, conn, inputVector[port]->GetInformationObject(conn), 
          update_ext))
        {
        // don't know why the subclass said we cannot handle this case. So abort.
        return 0;
        }

      int whole_extent[6];
      inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), whole_extent);

      // clamp extent_to_upload to the input extents.
      vtkKWEStructuredExtent::Clamp(input_update_extent, whole_extent);
      inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), 
        input_update_extent, 6);
      }
    }

  return 1;
}

//----------------------------------------------------------------------------
bool vtkKWEGPUImageAlgorithmDriver::SetupOutputTexture(
  ExtentTypes chunkType, vtkKWEDataTransferHelper* down_bus)
{
  vtkDataArray* outputArray = down_bus->GetArray();

  vtkTextureObject* texture = vtkTextureObject::New();
  texture->SetContext(this->Context);
  int extent[6];
  down_bus->GetGPUExtent(extent);
  int dims[3];
  vtkKWEStructuredExtent::GetDimensions(extent, dims);
  switch (chunkType)
    {
  case XY_PLANE:
    texture->Create2D(dims[0], dims[1],
      outputArray->GetNumberOfComponents(),
      outputArray->GetDataType(),
      false);
    break;

  case YZ_PLANE:
    texture->Create2D(dims[1], dims[2],
      outputArray->GetNumberOfComponents(),
      outputArray->GetDataType(),
      false);
    break;

  case XZ_PLANE:
    texture->Create2D(dims[0], dims[2],
      outputArray->GetNumberOfComponents(),
      outputArray->GetDataType(),
      false);
    break;

  case XYZ_GRID:
    texture->Create3D(dims[0], dims[1], dims[2],
      outputArray->GetNumberOfComponents(),
      outputArray->GetDataType(),
      false);
    break;

  default:
    vtkErrorMacro("Unhandled chunk description: "
                  << static_cast<int>(chunkType));
    texture->Delete();
    return false;
    }

  down_bus->SetTexture(texture);
  texture->Delete();
  return true;
}

//----------------------------------------------------------------------------
vtkKWEGPUImageAlgorithmDriver::vtkBuses* 
vtkKWEGPUImageAlgorithmDriver::Upload(
  vtkKWEExtentCalculator* extentCalculator,
  vtkInformationVector** inputVector,
  vtkKWEGPUImageAlgorithmDriver::vtkPipe& pipe)
{
  START_LOG();
  int numInputPorts = this->GetNumberOfInputPorts();

  vtkBuses* upBuses = pipe.NewBus();
  upBuses->SetNumberOfPorts(numInputPorts);

  for (int port =0; port < numInputPorts; port++)
    {
    int numConns = inputVector[port]->GetNumberOfInformationObjects();
    upBuses->SetNumberOfConnections(port, numConns);
    }

  try
    {
    for (int port = 0; port < numInputPorts; port++)
      {
      int numConnections = inputVector[port]->GetNumberOfInformationObjects();
      for (int conn = 0; conn < numConnections; conn++)
        {
        vtkImageData* input = vtkImageData::GetData(inputVector[port], conn);

        // Give subclass an opportunity to map output extent to input for this port.
        int extent_to_upload[6];
        int min_tex_dims = this->MapOutputExtentToInput(extent_to_upload,
          port, conn, inputVector[port]->GetInformationObject(conn), 
          extentCalculator->GetOutChunkExtent());
        if (min_tex_dims <= 0)
          {
          throw 0;
          }

        if (min_tex_dims < 1 || min_tex_dims > 3)
          {
          vtkWarningMacro("Forcing minimum texture dimensionality () to 2 (instead of " 
            << min_tex_dims << " )");
          min_tex_dims = 2;
          }

        // clamp extent_to_upload to the input extents.
        vtkKWEStructuredExtent::Clamp(extent_to_upload, input->GetExtent());

        // up_bus is used to upload the result from the GPU.
        vtkSmartPointer<vtkKWEDataTransferHelper> up_bus = 
          vtkSmartPointer<vtkKWEDataTransferHelper>::New();
        up_bus->SetContext(this->Context);
        up_bus->SetCPUExtent(input->GetExtent());
        up_bus->SetGPUExtent(extent_to_upload);
        //cout << "Upload CPUExtent: " << 
        //  PRINTEXT(up_bus->GetCPUExtent()) << endl;
        cout << "Upload GPUExtent: " << 
          PRINTEXT(up_bus->GetGPUExtent()) << endl;

        //up_bus->SetMinTextureDimension(min_tex_dims);
        up_bus->SetArray(input->GetPointData()->GetScalars());
        if (!up_bus->Upload(0, 0))
          {
          throw 0;
          }

        upBuses->SetBus(port, conn, up_bus);
        //inputVector2[port]->GetInformationObject(conn)->Set(UPLOAD_BUS(), up_bus);
        // UPLOAD_BUS() will provide the subclass all the information it needs to
        // know about the input including extents.
        }
      }
    }
  catch (int)
    {
    upBuses->Delete();
    upBuses = 0;
    }
  STOP_LOG("Upload");
  return upBuses;
}

#include "vtkTimerLog.h"
//----------------------------------------------------------------------------
int vtkKWEGPUImageAlgorithmDriver::RequestData(vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkTimerLog* timer = vtkTimerLog::New();
  timer->StartTimer();
  if (!this->Context)
    {
    this->SetContext(vtkRenderWindow::New());
    this->OwnContext = true;
    this->Context->Render();
    }

  this->Context->SetReportGraphicErrors(1);
  this->Context->MakeCurrent();

  // Ask the subclass the order of processing the output pieces.
  ExtentTypes chunkType = this->GetSplitMode(request, inputVector, outputVector);
  if (chunkType == vtkKWEGPUImageAlgorithmDriver::INVALID)
    {
    return 0;
    }

  vtkImageData* output = vtkImageData::GetData(outputVector, 0);
  this->AllocateOutputData(output);
  // Now, our subclass has told us how it needs the slices.

  vtkSmartPointer<vtkKWEExtentCalculator> extentCalculator
    = vtkSmartPointer<vtkKWEExtentCalculator>::New();
  extentCalculator->SetChunkDescription(static_cast<int>(chunkType));
  extentCalculator->SetWholeExtent(output->GetExtent());
  extentCalculator->LoadLimits(this->Context);
  // TODO: Take in to consideration if any input has padding.

  vtkKWEGPUImageAlgorithmDriver::vtkPipe pipe(this);

  if (!this->InitializeExecution(request, inputVector, outputVector))
    {
    vtkErrorMacro("Failed initialization.");
    return 0;
    }

  int count = 0;
  for (extentCalculator->Begin(); !extentCalculator->IsDone(); extentCalculator->Next(), count++)
    {
    vtkSmartPointer<vtkBuses> up_buses;
    up_buses.TakeReference(this->Upload(extentCalculator, inputVector, pipe));
    if (!up_buses.GetPointer())
      {
      vtkErrorMacro("Upload failed.");
      pipe.Abort();
      break;
      }

    // down_bus is used to download the result from the GPU.
    vtkSmartPointer<vtkKWEDataTransferHelper> down_bus = 
      vtkSmartPointer<vtkKWEDataTransferHelper>::New();
    down_bus->SetContext(this->Context);
    down_bus->SetCPUExtent(output->GetExtent());
    int down_ext[6];
    extentCalculator->GetOutChunkExtent(down_ext);
    vtkKWEStructuredExtent::Clamp(down_ext, output->GetExtent());
    down_bus->SetGPUExtent(down_ext);
    down_bus->SetArray(output->GetPointData()->GetScalars());

    //cout << "Download CPUExtent: " << 
    //  PRINTEXT(down_bus->GetCPUExtent()) << endl;
    //cout << "Download GPUExtent: " << 
    //  PRINTEXT(down_bus->GetGPUExtent()) << endl;
    if (!this->SetupOutputTexture(chunkType, down_bus))
      {
      vtkErrorMacro("Failed to create download texture.");
      pipe.Abort();
      break;
      }

    if (!pipe.Push(up_buses, down_bus))
      {
      // failed.
      vtkErrorMacro("GPU processing pipe failed");
      pipe.Abort();
      break;
      }
    }
  pipe.Flush();

  if (!this->FinalizeExecution(request, inputVector, outputVector))
    {
    vtkErrorMacro("Failed cleanup.");
    return 0;
    }

  timer->StopTimer();
  cout << "vtkKWEGPUImageAlgorithmDriver ("<<count<<"): " << timer->GetElapsedTime() << endl;
  timer->Delete();
  return 1;
}

//----------------------------------------------------------------------------
bool vtkKWEGPUImageAlgorithmDriver::ComputeTCoordsRange(
  double tcoords[6], 
  const int inputExt[6], const int outputExt[6])
{
  if (!vtkKWEStructuredExtent::Smaller(outputExt, inputExt))
    {
    vtkErrorMacro("Cannot handle cases where output extent is larger than input.");
    return false;
    }

  tcoords[0] = tcoords[2] = tcoords[4] = 0.0;
  tcoords[1] = tcoords[3] = tcoords[5] = 1.0;

  int indims[3];
  vtkKWEStructuredExtent::GetDimensions(inputExt, indims);

  int outdims[3];
  vtkKWEStructuredExtent::GetDimensions(outputExt, outdims);

  tcoords[0] = (outputExt[0] - inputExt[0])/static_cast<double>(indims[0]);
  tcoords[1] = tcoords[0] + outdims[0]/static_cast<double>(indims[0]); 

  tcoords[2] = (outputExt[2] - inputExt[2])/static_cast<double>(indims[1]);
  tcoords[3] = tcoords[2] + outdims[1]/static_cast<double>(indims[1]); 

  tcoords[4] = (outputExt[4] - inputExt[4])/static_cast<double>(indims[2]);
  tcoords[5] = tcoords[4] + outdims[2]/static_cast<double>(indims[2]); 
  return true;
}

//----------------------------------------------------------------------------
void vtkKWEGPUImageAlgorithmDriver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

