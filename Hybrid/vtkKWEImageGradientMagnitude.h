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
  virtual bool Execute(vtkBuses* upBuses, vtkKWEDataTransferHelper* downBus);
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


