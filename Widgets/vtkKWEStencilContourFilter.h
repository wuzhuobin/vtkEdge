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
// .NAME vtkKWEStencilContourFilter - generate isosurface/isoline from a vtkImageStencilData
// .SECTION Description

#ifndef __vtkKWEStencilContourFilter_h
#define __vtkKWEStencilContourFilter_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkPolyDataAlgorithm.h"

class vtkContourFilter;
class vtkImageStencilData;
class vtkImageData;

class VTKEdge_WIDGETS_EXPORT vtkKWEStencilContourFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkKWEStencilContourFilter *New();
  vtkTypeRevisionMacro(vtkKWEStencilContourFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get extents. The contour filter will update only within the specified 
  // extents. If the extents aren't set, the extents of the input will be used.
  vtkSetVector6Macro( Extent, int );
  vtkGetVector6Macro( Extent, int );

  // Description:
  // Methods to set / get contour values.
  void   SetValue(int i, double value);
  double GetValue(int i);
  double *GetValues();
  void GetValues(double *contourValues);
  void SetNumberOfContours(int number);
  int GetNumberOfContours();
  void GenerateValues(int numContours, double range[2]);
  void GenerateValues(int numContours, double rangeStart, double rangeEnd);

  // Description:
  // Modified GetMTime Because we delegate to vtkContourValues
  unsigned long GetMTime();

  // Description:
  // Set/Get the computation of normals. Normal computation is fairly
  // expensive in both time and storage. If the output data will be
  // processed by filters that modify topology or geometry, it may be
  // wise to turn Normals and Gradients off.
  void SetComputeNormals( int );
  int GetComputeNormals();
  vtkBooleanMacro(ComputeNormals,int);

  // Description:
  // Set/Get the computation of gradients. Gradient computation is
  // fairly expensive in both time and storage. Note that if
  // ComputeNormals is on, gradients will have to be calculated, but
  // will not be stored in the output dataset.  If the output data
  // will be processed by filters that modify topology or geometry, it
  // may be wise to turn Normals and Gradients off.
  void SetComputeGradients( int );
  int GetComputeGradients();
  vtkBooleanMacro(ComputeGradients,int);

  // Description:
  // Set/Get the computation of scalars.
  void SetComputeScalars( int );
  int GetComputeScalars();
  vtkBooleanMacro(ComputeScalars,int);

protected:
  vtkKWEStencilContourFilter();
  ~vtkKWEStencilContourFilter();

  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  virtual int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);
  virtual int FillInputPortInformation(int port, vtkInformation *info);

  vtkContourFilter *ContourFilter;
  vtkImageData     *ImageData;
  int               Extent[6];

private:
  vtkKWEStencilContourFilter(const vtkKWEStencilContourFilter&);  // Not implemented.
  void operator=(const vtkKWEStencilContourFilter&);  // Not implemented.
};

#endif

