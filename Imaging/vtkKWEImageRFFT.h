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
// .NAME vtkKWEImageRFFT -  Reverse Fast Fourier Transform.
// .SECTION Description
// vtkKWEImageRFFT implements the reverse fast Fourier transform.  The input
// can have real or complex data in any components and data types, but
// the output is always complex doubles with real values in component0, and
// imaginary values in component1.  The filter is fastest for images that
// have power of two sizes.  The filter uses a butterfly fitlers for each
// prime factor of the dimension.  This makes images with prime number dimensions
// (i.e. 17x17) much slower to compute.  Multi dimensional (i.e volumes)
// FFT's are decomposed so that each axis executes in series.
// In most cases the RFFT will produce an image whose imaginary values are all
// zero's. In this case vtkImageExtractComponents can be used to remove
// this imaginary components leaving only the real image.

// .SECTION See Also
// vtkImageExtractComponenents



#ifndef __vtkKWEImageRFFT_h
#define __vtkKWEImageRFFT_h

#include "vtkImageFourierFilter.h"
#include "VTKEdgeConfigure.h" // include configuration header


class VTKEdge_IMAGING_EXPORT vtkKWEImageRFFT : public vtkImageFourierFilter
{
public:
  static vtkKWEImageRFFT *New();
  vtkTypeRevisionMacro(vtkKWEImageRFFT,vtkImageFourierFilter);


  // Description:
  // For streaming and threads.  Splits output update extent into num pieces.
  // This method needs to be called num times.  Results must not overlap for
  // consistent starting extent.  Subclass can override this method.  This
  // method returns the number of pieces resulting from a successful split.
  // This can be from 1 to "total".  If 1 is returned, the extent cannot be
  // split.
  int SplitExtent(int splitExt[6], int startExt[6],
                  int num, int total);

  // Description:
  // CUDA is only threadsafe accross multiple devices.
  // This method raises a vtkError if called.
  virtual void SetNumberOfThreads(int _arg);

protected:
  vtkKWEImageRFFT() { this->NumberOfThreads = 1; }
  ~vtkKWEImageRFFT() {};

  virtual int IterativeRequestInformation(vtkInformation* in,
                                          vtkInformation* out);
  virtual int IterativeRequestUpdateExtent(vtkInformation* in,
                                           vtkInformation* out);

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
                       int outExt[6], int threadId);

private:
  vtkKWEImageRFFT(const vtkKWEImageRFFT&);  // Not implemented.
  void operator=(const vtkKWEImageRFFT&);  // Not implemented.
};

#endif










