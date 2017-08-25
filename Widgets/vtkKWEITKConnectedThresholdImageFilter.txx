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

