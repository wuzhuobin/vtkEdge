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
// .NAME vtkKWEGPUImageAlgorithmDriver 
// .SECTION Description
// vtkKWEGPUImageAlgorithmDriver is a the driver for all image processing
// algorithms that use the GPU to do the work.

#ifndef __vtkKWEGPUImageAlgorithmDriver_h
#define __vtkKWEGPUImageAlgorithmDriver_h

#include "vtkImageAlgorithm.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkStructuredData.h" // for VTK_XY_PLANE, ...
#include "vtkWeakPointer.h" // needed for vtkWeakPointer
#include "vtkSmartPointer.h" // needed for vtkWeakPointer

//BTX
#include <vtkstd/vector>

//ETX
class vtkRenderWindow;
class vtkKWEDataTransferHelper;
class vtkKWEExtentCalculator;

class VTKEdge_HYBRID_EXPORT vtkKWEGPUImageAlgorithmDriver : public vtkImageAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkKWEGPUImageAlgorithmDriver, vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the context. Note that the context is not reference counted.
  vtkRenderWindow* GetContext();
  virtual void SetContext(vtkRenderWindow*);

//BTX
  enum ExtentTypes
    {
    INVALID=0,
    XY_PLANE = VTK_XY_PLANE,
    YZ_PLANE = VTK_YZ_PLANE,
    XZ_PLANE = VTK_XZ_PLANE,
    XYZ_GRID = VTK_XYZ_GRID
    };

protected:
  class vtkBuses;

  vtkKWEGPUImageAlgorithmDriver();
  ~vtkKWEGPUImageAlgorithmDriver();

  // Description:
  // The method should return how the streamer should break up extents.
  // This is not much different from the split mode specified on
  // vtkExtentTranslator.
  virtual ExtentTypes GetSplitMode(vtkInformation* request, 
    vtkInformationVector** inputVector, 
    vtkInformationVector* outputVector) = 0;

  // Description:
  // Given the output_extent that we need to
  // produce, what extents does the subclass need from  the input on the
  // indicated (port, connection). Return value is a an int indicating the
  // minimum dimensionality of the texture created for the corresponding input.
  // Returns value 0 (or less) indicates error.
  virtual int MapOutputExtentToInput(int input_extent[6],
    int port, int connection, 
    vtkInformation* inInfo, const int output_extent[6]) = 0;


  // Description:
  // Gives the subclasses an opportunity to do some initialization before the
  // looping begins.
  virtual bool InitializeExecution(
    vtkInformation* vtkNotUsed(request), 
    vtkInformationVector** vtkNotUsed(inputVector), 
    vtkInformationVector* vtkNotUsed(outputVector)) {return true;}

  // Description:
  // Actual execution method.
  virtual bool Execute(vtkBuses* upBuses,
    vtkKWEDataTransferHelper* downBus) = 0;
  virtual void Execute() { this->Superclass::Execute(); }

  // Description:
  // Gives the subclasses an opportunity to do some cleanup after the
  // looping ends.
  virtual bool FinalizeExecution(
    vtkInformation* vtkNotUsed(request), 
    vtkInformationVector** vtkNotUsed(inputVector), 
    vtkInformationVector* vtkNotUsed(outputVector)) {return true;}

  // Description:
  // Actual execution. Subclasses should not override this method. They should
  // override Execute() instead.
  virtual int RequestData(vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector);

  // Description:
  // RequestUpdateExtent.
  int RequestUpdateExtent (
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector);

  bool ComputeTCoordsRange(double tcoords[6], 
    const int inputExt[6], const int outputExt[6]);

  vtkWeakPointer<vtkRenderWindow> Context;
  bool OwnContext;

private:
  vtkKWEGPUImageAlgorithmDriver(const vtkKWEGPUImageAlgorithmDriver&); // Not implemented.
  void operator=(const vtkKWEGPUImageAlgorithmDriver&); // Not implemented.

  class vtkPipe;
  friend class vtkPipe;

  // Description:
  // Internal method that uploads the current chunk for all inputs to the GPU.
  vtkBuses* Upload(vtkKWEExtentCalculator* extentCalculator,
    vtkInformationVector** inputVector,
    vtkPipe& pipe);

  bool SetupOutputTexture(ExtentTypes chunkType, vtkKWEDataTransferHelper* down_bus);
//ETX
};

//BTX
class vtkKWEGPUImageAlgorithmDriver::vtkBuses : public vtkObjectBase
{
public:
  vtkTypeRevisionMacro(vtkBuses, vtkObjectBase);
  static vtkBuses* New();

  void SetNumberOfPorts(unsigned int num);
  void SetNumberOfConnections(unsigned int port, unsigned int num);
  vtkKWEDataTransferHelper* GetBus(unsigned int port, unsigned int conn);
  void SetBus(unsigned int port, unsigned int conn, vtkKWEDataTransferHelper* bus);

private:
  vtkBuses() { }
  virtual ~vtkBuses() { }

  typedef vtkstd::vector<vtkSmartPointer<vtkKWEDataTransferHelper> > XferHelperVector;
  typedef vtkstd::vector<XferHelperVector> VectorOfXferHelperVector;

  VectorOfXferHelperVector Buses;
};
//ETX
#endif


