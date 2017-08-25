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

#include "vtkKWEITKConnectedThresholdImageFilter.h"
#include "itkNumericTraits.h"
#include "vtkKWEITKFilterModuleStencilOutput.h"

#include "itkImageFileWriter.h"

namespace vtkitk
{

template< class InputPixelType >
vtkKWEITKConnectedThresholdImageFilter< InputPixelType >
::vtkKWEITKConnectedThresholdImageFilter()
{
  this->m_Filter             = new FilterModuleStencilOutput< FilterType >();
}  

template< class InputPixelType >
vtkKWEITKConnectedThresholdImageFilter< InputPixelType >
::~vtkKWEITKConnectedThresholdImageFilter()
{
  delete this->m_Filter;
}

template< class InputPixelType >
void
vtkKWEITKConnectedThresholdImageFilter< InputPixelType >
::AddSeed( int seed[3] )
{
  IndexType index;
  index[0] = seed[0];
  index[1] = seed[1];
  index[2] = seed[2];
  FilterModuleStencilOutput< FilterType > * f = 
    (dynamic_cast< 
     FilterModuleStencilOutput< FilterType > * >(this->m_Filter));
  if (!f)
    {
    itkExceptionMacro( << "dynamic cast returned false.. impossible!");
    }
  
  FilterType *filter = f->GetFilter();
  filter->SetSeed(index);
  InputPixelType val = static_cast< InputPixelType >( 
                          this->m_Filter->GetInput()
                              ->GetScalarComponentAsDouble(
                                         index[0], index[1], index[2], 0) );
  filter->SetUpper( val );
  filter->SetLower( val );
  filter->SetReplaceValue(
      itk::NumericTraits< typename OutputImageType::PixelType >::max() );
}

template< class InputPixelType >
void
vtkKWEITKConnectedThresholdImageFilter< InputPixelType >
::Update()
{
  this->Superclass::Update();

  FilterModuleStencilOutput< FilterType > * f = 
    (dynamic_cast< 
     FilterModuleStencilOutput< FilterType > * >(this->m_Filter));
  if (!f)
    {
    itkExceptionMacro( << "dynamic cast returned false.. impossible!");
    }
     
  // Update the filter.. If any timing analysis needs to be done.. 
  // This method needs to be interactive enough.
  f->Update();

  //typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;
  //typename ImageWriterType::Pointer writer = ImageWriterType::New();
  //writer->SetInput( f->GetFilter()->GetOutput() );
  //writer->SetFileName( "filterConnThresh.mha" );
  //writer->Update();
}

} // end namespace vtkitk

