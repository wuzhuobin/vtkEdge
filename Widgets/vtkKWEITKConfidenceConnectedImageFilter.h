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
#ifndef _vtkKWEITKConfidenceConnectedFilterModule_h
#define _vtkKWEITKConfidenceConnectedFilterModule_h

#include "itkConfidenceConnectedMiniPipelineFilter.h"
#include "vtkKWEITKImageToStencilFilter.h"
#include <vector>

namespace vtkitk
{

template <class InputPixelType>
class vtkKWEITKConfidenceConnectedImageFilter : public vtkKWEITKImageToStencilFilter
{
public:
  typedef vtkKWEITKConfidenceConnectedImageFilter         Self;
  typedef vtkKWEITKImageToStencilFilter                   Superclass;
  typedef itk::SmartPointer< Self >                    Pointer;
  typedef itk::SmartPointer< const Self >              ConstPointer;
  itkTypeMacro(vtkKWEITKConfidenceConnectedImageFilter, vtkKWEITKImageToStencilFilter);
  itkNewMacro(Self);
  
  typedef  itk::Image< InputPixelType, Dimension >     InputImageType; 
  typedef  typename InputImageType::IndexType          IndexType;
  typedef  itk::ConfidenceConnectedMiniPipelineFilter< 
    InputImageType,  OutputImageType >                 FilterType;
  
  virtual void AddSeed( int seed[3] );

  // No MTime checks etc.. will update every time.. 
  virtual void Update();

protected:
  vtkKWEITKConfidenceConnectedImageFilter();
  ~vtkKWEITKConfidenceConnectedImageFilter();
};

} // end namespace vtkitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "vtkKWEITKConfidenceConnectedImageFilter.txx"
#endif

#endif 

