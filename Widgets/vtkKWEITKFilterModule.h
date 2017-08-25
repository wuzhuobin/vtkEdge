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


#ifndef _vtkKWEITKFilterModule_h
#define _vtkKWEITKFilterModule_h

#include "VTKEdgeConfigure.h" // Needed for export symbols
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkKWEITKFilterModuleBase.h"
#include "itkImage.h"
#include "vtkImageData.h"

#include <string>
#include <iostream>

namespace vtkitk
{

template <class TFilterType >
class FilterModule : public FilterModuleBase 
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
  FilterModule() 
    {
    this->m_ImportFilter       = ImportFilterType::New();
    this->m_ExportFilter       = ExportFilterType::New();
    this->m_Filter             = FilterType::New();
    //this->m_Filter->ReleaseDataFlagOn();
 
    // Set the Observer for updating progress in the GUI
    this->m_Filter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    this->m_Filter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    this->m_Filter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    this->m_LetITKAllocateOutputMemory = false;
    }

  /**  Destructor */
  virtual ~FilterModule() 
    {
    }

  /** Give access to the input image */
  void SetInput(vtkImageData * image) 
    {
    this->m_ImportFilter->SetInput(image);  
    }

  /** Give access to the input image */
  virtual vtkImageData* GetInput() 
    {
    return this->m_ImportFilter->GetInput(); 
    }

  /** Return  a pointer to the internal filter */
  FilterType * GetFilter()
    {
    return this->m_Filter.GetPointer();
    }

  void SetLetITKAllocateOutputMemory(bool s)
    {
    this->m_LetITKAllocateOutputMemory = s;
    }

  // FIXME: We are so busted when doing this for multi-component data.
  // It won't even frickin compile.
  void SetOutputBuffer( void * buffer )
    {
    this->m_LetITKAllocateOutputMemory = false;
    RegionType region = this->m_Filter->GetOutput()->GetLargestPossibleRegion();
    const unsigned long nPixels = region.GetNumberOfPixels();
    this->m_Filter->GetOutput()->SetImportPointer( 
        static_cast< OutputPixelType * >(buffer),  nPixels );
    this->m_Filter->GetOutput()->Allocate( ); // TODO Check if necessary
    }

  /**  Update performs the actual filtering on the data */
  virtual void Update()
    {

    // Progress reporting
    this->InitializeProgressValue();
    this->SetCurrentFilterProgressWeight( 1.0 );

    // Update the filter
    try
      {
      this->m_ImportFilter->Update();
      }
    catch( itk::ProcessAborted & e)
      {
      std::cerr << "ITK error " << e.what() << std::endl;
      }

    // Update the filter
    try
      {
      this->m_Filter->SetInput( this->m_ImportFilter->GetOutput() );
      this->m_Filter->Update();
      }
    catch( itk::ProcessAborted & e)
      {
      std::cerr << "ITK error " << e.what() << std::endl;
      }

    // Update the exporter
    try
      {
      this->m_ExportFilter->SetInput( this->m_Filter->GetOutput() );
      this->m_ExportFilter->Update();
      }
    catch( itk::ProcessAborted & e)
      {
      std::cerr << "ITK error " << e.what() << std::endl;
      }
    }

  vtkImageData * GetOutput()
    {
    return this->m_ExportFilter->GetOutput();
    }

  virtual void SetRequestedExtent( int extent[6] )
    {
    SizeType size;
    size[0] = extent[1] - extent[0] + 1; 
    size[1] = extent[3] - extent[2] + 1; 
    size[2] = extent[5] - extent[4] + 1; 
    IndexType index;
    index[0] = extent[0];
    index[1] = extent[2];
    index[2] = extent[4];
    RegionType region(index,size);

    this->m_Filter->GetOutput()->SetRegions( region );
    this->m_ExportFilter->GetOutput()->SetUpdateExtent(extent);
    }

protected:
  typename ImportFilterType::Pointer    m_ImportFilter;
  typename FilterType::Pointer          m_Filter;
  bool                                  m_LetITKAllocateOutputMemory;
  typename ExportFilterType::Pointer    m_ExportFilter;
};

} // end namespace vtkitk

#endif
