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
#ifndef _vtkKWEITKConnectedThresholdFilterModule_h
#define _vtkKWEITKConnectedThresholdFilterModule_h

#include "itkConnectedThresholdImageFilter.h"
#include "vtkKWEITKImageToStencilFilter.h"
#include <vector>

namespace vtkitk
{

template <class InputPixelType>
class vtkKWEITKConnectedThresholdImageFilter : public vtkKWEITKImageToStencilFilter
{
public:
  typedef vtkKWEITKConnectedThresholdImageFilter         Self;
  typedef vtkKWEITKImageToStencilFilter                   Superclass;
  typedef itk::SmartPointer< Self >                    Pointer;
  typedef itk::SmartPointer< const Self >              ConstPointer;
  itkTypeMacro(vtkKWEITKConnectedThresholdImageFilter, vtkKWEITKImageToStencilFilter);
  itkNewMacro(Self);

  typedef  itk::Image< InputPixelType, Dimension >     InputImageType;
  typedef  typename InputImageType::IndexType          IndexType;
  typedef  itk::ConnectedThresholdImageFilter<
    InputImageType,  OutputImageType >                 FilterType;

  virtual void AddSeed( int seed[3] );

  // No MTime checks etc.. will update every time..
  virtual void Update();

protected:
  vtkKWEITKConnectedThresholdImageFilter();
  ~vtkKWEITKConnectedThresholdImageFilter();
};

} // end namespace vtkitk

#ifndef ITK_MANUAL_INSTANTIATION
#include "vtkKWEITKConnectedThresholdImageFilter.txx"
#endif

#endif

