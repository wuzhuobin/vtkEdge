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







