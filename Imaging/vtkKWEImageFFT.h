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
// .NAME vtkKWEImageFFT -  Fast Fourier Transform.
// .SECTION Description
// vtkKWEImageFFT implements a  fast Fourier transform.  The input
// can have real or complex data in any components and data types, but
// the output is always complex doubles with real values in component0, and
// imaginary values in component1.  The filter is fastest for images that
// have power of two sizes.  The filter uses a butterfly fitlers for each
// prime factor of the dimension.  This makes images with prime number dimensions
// (i.e. 17x17) much slower to compute.  Multi dimensional (i.e volumes)
// FFT's are decomposed so that each axis executes in series.


#ifndef __vtkKWEImageFFT_h
#define __vtkKWEImageFFT_h


#include "vtkImageFourierFilter.h"
#include "VTKEdgeConfigure.h" // include configuration header


class VTKEdge_IMAGING_EXPORT vtkKWEImageFFT : public vtkImageFourierFilter
{
public:
  static vtkKWEImageFFT *New();
  vtkTypeRevisionMacro(vtkKWEImageFFT,vtkImageFourierFilter);


  // Description:
  // Used internally for streaming and threads.
  // Splits output update extent into num pieces.
  // This method needs to be called num times.  Results must not overlap for
  // consistent starting extent.  Subclass can override this method.
  // This method returns the number of pieces resulting from a
  // successful split.  This can be from 1 to "total".
  // If 1 is returned, the extent cannot be split.
  int SplitExtent(int splitExt[6], int startExt[6],
                  int num, int total);

protected:
  vtkKWEImageFFT() { this->NumberOfThreads = 1; }
  ~vtkKWEImageFFT() {};

  virtual int IterativeRequestInformation(vtkInformation* in,
                                          vtkInformation* out);
  virtual int IterativeRequestUpdateExtent(vtkInformation* in,
                                           vtkInformation* out);

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
                       int outExt[6], int threadId);

  // Description:
  // CUDA is only threadsafe accross multiple devices.
  // This method raises a vtkError if called.
  virtual void SetNumberOfThreads(int _arg);

private:
  vtkKWEImageFFT(const vtkKWEImageFFT&);  // Not implemented.
  void operator=(const vtkKWEImageFFT&);  // Not implemented.


};

#endif










