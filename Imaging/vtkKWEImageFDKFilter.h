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
// .NAME vtkKWEImageFDKFilter - Generic filter that has N inputs.
// .SECTION Description
// vtkKWEImageFDKFilter is a super class for filters that
// have any number of inputs. Streaming is not available in this class yet.

// .SECTION See Also
// vtkImageToImageFilter vtkImageInPlaceFilter vtkImageTwoInputFilter



#ifndef __vtkKWEImageFDKFilter_h
#define __vtkKWEImageFDKFilter_h


#include "vtkImageAlgorithm.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkDoubleArray;


class VTKEdge_IMAGING_EXPORT  vtkKWEImageFDKFilter : public vtkImageAlgorithm
{
public:
  static vtkKWEImageFDKFilter *New();
  vtkTypeRevisionMacro(vtkKWEImageFDKFilter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Distance from the detector to the center of
  // rotation of the source object.
  vtkSetMacro(Radius,float);
  vtkGetMacro(Radius,float);

  // Description:
  // Collection of the angles of each projection
  vtkGetObjectMacro(Angles, vtkDoubleArray);

protected:
  vtkKWEImageFDKFilter();
  ~vtkKWEImageFDKFilter();

  float Radius; // Distance from the detector to the center of rotation of the source object
  vtkDoubleArray * Angles;



  virtual int RequestInformation (vtkInformation *vtkNotUsed(request),
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector);

  virtual int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *);

  void GenerateWeightingFunction(int projExt[6]);
  void ApplyWeightingFunction(int row, int slice, vtkImageData * projection, vtkImageData * output);
  void GenerateFilter(float * filter, int order);

private:
  vtkKWEImageFDKFilter(const vtkKWEImageFDKFilter&);  // Not implemented.
  void operator=(const vtkKWEImageFDKFilter&);  // Not implemented.

  vtkImageData * WeightFunction;
};

#endif







