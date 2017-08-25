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
// .NAME vtkKWEImageGradientMagnitude
// .SECTION Description

#ifndef __vtkKWEImageGradientMagnitude_h
#define __vtkKWEImageGradientMagnitude_h

#include "vtkKWEGPUImageAlgorithmDriver.h"

class vtkShaderProgram2;
class VTKEdge_HYBRID_EXPORT vtkKWEImageGradientMagnitude : public vtkKWEGPUImageAlgorithmDriver
{
public:
  static vtkKWEImageGradientMagnitude* New();
  vtkTypeRevisionMacro(vtkKWEImageGradientMagnitude, vtkKWEGPUImageAlgorithmDriver);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Determines how the input is interpreted (set of 2d slices ...)
  vtkSetClampMacro(Dimensionality,int,2,3);
  vtkGetMacro(Dimensionality,int);

//BTX
protected:
  vtkKWEImageGradientMagnitude();
  ~vtkKWEImageGradientMagnitude();

  // Description:
  // The method should return how the streamer should break up extents.
  // This is not much different from the split mode specified on
  // vtkExtentTranslator.
  virtual ExtentTypes GetSplitMode(vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector);

  // Description:
  // Given the output_extent that we need to
  // produce, what extents does the subclass need from  the input on the
  // indicated (port, connection). Return value is a an int indicating the
  // minimum dimensionality of the texture created for the corresponding input.
  // Returns value 0 (or less) indicates error.
  virtual int MapOutputExtentToInput(int input_extent[6],
    int port, int connection,
    vtkInformation* inInfo, const int output_extent[6]);

  // Description:
  // Actual execution method.
  virtual bool Execute(vtkBuses* upBuses, vtkDataTransferHelper* downBus);
  virtual void Execute() { this->Superclass::Execute(); }

  // Description:
  // Gives the subclasses an opportunity to do some initialization before the
  // looping begins.
  bool InitializeExecution(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector);

  // Description:
  // Gives the subclasses an opportunity to do some cleanup after the
  // looping ends.
  bool FinalizeExecution(
    vtkInformation* request,
    vtkInformationVector** inputVector,
    vtkInformationVector* outputVector);

  int Dimensionality;
  vtkSmartPointer<vtkShaderProgram2> GLSLProgram;

  double SpacingReciprocal[3];
private:
  vtkKWEImageGradientMagnitude(const vtkKWEImageGradientMagnitude&); // Not implemented.
  void operator=(const vtkKWEImageGradientMagnitude&); // Not implemented.
//ETX
};

#endif


