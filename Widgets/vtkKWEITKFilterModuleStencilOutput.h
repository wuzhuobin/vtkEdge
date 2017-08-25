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

#ifndef _vtkKWEITKFilterModuleStencilOutput_h
#define _vtkKWEITKFilterModuleStencilOutput_h

#include "vtkKWEITKFilterModule.h"
#include "vtkImageStencilData.h"
#include "vtkImageData.h"
#include "vtkImageIterator.h"

namespace vtkitk
{

template <class TFilterType >
class FilterModuleStencilOutput : public FilterModule< TFilterType > 
{
public:

   // Instantiate the image types
  typedef TFilterType                             FilterType;
  typedef typename FilterType::InputImageType     InputImageType;
  typedef typename FilterType::OutputImageType    OutputImageType;
  typedef typename InputImageType::PixelType      InputPixelType;
  typedef typename OutputImageType::PixelType     OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef itk::VTKImageToImageFilter< InputImageType >
                                                    ImportFilterType;
  typedef itk::ImageToVTKImageFilter< OutputImageType >
                                                    ExportFilterType;

  typedef typename InputImageType::SizeType      SizeType;
  typedef typename InputImageType::IndexType     IndexType;
  typedef typename InputImageType::RegionType    RegionType;

  /**  Constructor */
  FilterModuleStencilOutput(); 
  virtual ~FilterModuleStencilOutput();

  /** Will return 0 if the output is not of type UNSIGNED_CHAR */
  int GetOutputAsStencil( vtkImageStencilData *stencilData );

  /** Filters inherently run on image regions. These are cuboids, defined by
   * bounded by a set of extents. Often, when a local region growing is 
   * performed with a seed or a paintbrush, you would expect the regions to 
   * be spherical, and not have rectangular artefacts. 
   */
  int GetSphereBoundedOutputAsStencil( vtkImageStencilData *stencilData,
                                       int center[3], double radius[3] );

};

} // end namespace vtkitk

#include "vtkKWEITKFilterModuleStencilOutput.txx"

#endif
