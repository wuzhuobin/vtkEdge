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

// .NAME vtkKWEGPUArrayCalculator - perform mathematical operations on data in field data arrays on the GPU
// .SECTION Description
// vtkGPUArrayCalculator performs operations on vectors or scalars in field
// data arrays on the GPU.  It uses vtkFunctionParser to do the parsing and to
// evaluate the function for each entry in the input arrays.  The arrays
// used in a given function must be all in point data or all in cell data.
// The resulting array will be stored as a field data array.  The result
// array can either be stored in a new array or it can overwrite an existing
// array.
//
// The functions that this array calculator understands is:
// <pre>
// standard operations: + - * / ^ .
// access vector components: iHat, jHat, kHat
// abs
// acos
// asin
// atan
// ceil
// cos
// cosh
// exp
// floor
// log
// mag
// min
// max
// norm
// sign
// sin
// sinh
// sqrt
// tan
// tanh
// </pre>
// Note that some of these operations work on scalars, some on vectors, and some on
// both (e.g., you can multiply a scalar times a vector). The operations are performed
// tuple-wise (i.e., tuple-by-tuple). The user must specify which arrays to use as
// vectors and/or scalars, and the name of the output data array.
//
// .SECTION See Also
// vtkFunctionParser

#ifndef __vtkKWEGPUArrayCalculator_h
#define __vtkKWEGPUArrayCalculator_h

#include "vtkArrayCalculator.h"
#include "VTKEdgeConfigure.h" // include configuration header


class vtkKWEFunctionToGLSL;
class vtkRenderWindow;
class vtkFloatArray;

class VTKEdge_HYBRID_EXPORT vtkKWEGPUArrayCalculator : public vtkArrayCalculator
{
public:
  vtkTypeRevisionMacro(vtkKWEGPUArrayCalculator,vtkArrayCalculator);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkKWEGPUArrayCalculator *New();

  // Description:
  // Get/Set the rendering context.
  vtkGetObjectMacro(Context, vtkRenderWindow);
  void SetContext(vtkRenderWindow *);

  // Description:
  // Tells if the GPU implementation is supported by the graphics card in use.
  bool GetSupportedByHardware();

  // Description:
  // Set/Get the function to be evaluated.
  virtual void SetFunction(const char* function);

  // Description:
  // Set/Get the dataset size threshold. Under this size,
  // the CPU implementation is used (see vtkArrayCalculator).
  // Above or equal to this size, the GPU is used if it supports the required
  // OpenGL extensions. Initial value is 0, trying to use the GPU in any case.
  // The GPU implementation is faster than the CPU implementation if the data
  // array is large enough. This threshold depends on the both the speed of
  // the CPU and the GPU, the RAM and VRAM. An experiment on an Intel Core 2
  // Duo T9500, 4GB with a nVidia Quadro FX 3600M, 512MB shows that 15000 is
  // a good threshold.
  vtkSetMacro(SizeThreshold,vtkIdType);
  vtkGetMacro(SizeThreshold,vtkIdType);

  // Description:
  // Tell the filter to use CalibratedSizeThreshold instead of SizeThreshold.
  // If CalibratedSizeThreshold has not been computed yet, a calibration is
  // performed (expensive call to Calibrate() that happens once).
  vtkGetMacro(UseCalibration,int);
  vtkSetMacro(UseCalibration,int);
  vtkBooleanMacro(UseCalibration,int);

  // Description:
  // Tell the filter to calibrate itself. Starting for the current value of
  // SizeThreshold, try to find the actual threshold size at which the GPU
  // implementation is faster than the CPU one. This call is expensive.
  // This method is usually called automatically once when UseCalibration is
  // on. But you can call it directly to control at what time the calibration
  // happens or to force a new calibration.
  void Calibrate();

  // Description:
  // Return the size threshold computed by the last calibration. Initial value
  // is 0.
  vtkGetMacro(CalibratedSizeThreshold,vtkIdType);

  // Description:
  // Remove all the scalar variable names and their associated array names.
  virtual void RemoveScalarVariables();

  // Description:
  // Remove all the scalar variable names and their associated array names.
  virtual void RemoveVectorVariables();

  // Description:
  // Remove all the coordinate variables.
  virtual void RemoveCoordinateScalarVariables();

  // Description:
  // Remove all the coordinate variables.
  virtual void RemoveCoordinateVectorVariables();

  // Description:
  // User-defined maximum size in bytes of GPU memory that can be
  // assigned to the array calculator.
  // Initial value is 134217728 bytes (128*2^20=128Mb).
  // In a 32-bit build, this ivar can encode up to 4GB.
  // A null value means no limit.
  vtkSetMacro(MaxGPUMemorySizeInBytes,vtkIdType);
  vtkGetMacro(MaxGPUMemorySizeInBytes,vtkIdType);

protected:
  vtkKWEGPUArrayCalculator();
  virtual ~vtkKWEGPUArrayCalculator();

  void SimulateGPUComputation(vtkFloatArray *values);

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  virtual void ComputeSubRange(vtkDataArray *array,
                               vtkIdType first,
                               vtkIdType last,
                               double subRange[2]);

  vtkRenderWindow *Context;

  vtkKWEFunctionToGLSL *FunctionParserToGLSL;

  vtkIdType MaxGPUMemorySizeInBytes;

  vtkIdType SizeThreshold;
  int UseCalibration; // boolean
  vtkIdType CalibratedSizeThreshold;
  bool CalibrationDone;
  bool SupportedByHardware;

private:
  vtkKWEGPUArrayCalculator(const vtkKWEGPUArrayCalculator&);  // Not implemented.
  void operator=(const vtkKWEGPUArrayCalculator&);  // Not implemented.
};

#endif
