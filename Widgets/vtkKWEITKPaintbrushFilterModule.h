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

#ifndef _vtkKWEITKPaintbrushFilterModule_h
#define _vtkKWEITKPaintbrushFilterModule_h

#include "vtkKWEITKFilterModule.h"

namespace vtkitk
{

template <class TFilterType >
class PaintbrushFilterModule : public FilterModule< TFilterType > {

public:

  // Instantiate the image types
  typedef typename Superclass::FilterType         FilterType;
  typedef typename Superclass::InputImageType     InputImageType;
  typedef typename Superclass::OutputImageType    OutputImageType;
  typedef typename Superclass::PixelType          InputPixelType;
  typedef typename Superclass::PixelType          OutputPixelType;

  typedef typename Superclass::SizeType           SizeType;
  typedef typename Superclass::IndexType          IndexType;
  typedef typename Superclass::RegionType         RegionType;

  /**  Constructor */
  FilterModule()
    {
    this->m_ImportFilter       = ImportFilterType::New();
    this->m_ExportFilter       = ExportFilterType::New();
    this->m_Filter             = FilterType::New();
    this->m_Filter->ReleaseDataFlagOn();

    this->m_Filter->SetInput( this->m_ImportFilter->GetOutput() );
    this->m_ExportFilter->SetInput( this->m_Filter->GetOutput() );

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
  const InputImageType * GetInput()
    {
    this->m_ImportFilter->Update();
    return this->m_ImportFilter->GetOutput();
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

  void SetOutputRegion( RegionType region )
    {
    this->m_Filter->GetOutput()->SetRegions( region );
    }

  /**  Update performs the actual filtering on the data */
  virtual void Update( const vtkVVProcessDataStruct * pds )
    {

    // Progress reporting
    this->InitializeProgressValue();
    this->SetCurrentFilterProgressWeight( 1.0 );

    // Execute the importer
    try
      {
      this->m_ImportFilter->Update();
      }
    catch( itk::ProcessAborted & e )
      {
      }

    // Update the filter
    try
      {
      this->m_Filter->Update();
      }
    catch( itk::ProcessAborted & e)
      {
      std::cerr << "ITK error " << e.what() << std::endl;
      }

    // Update the exporter
    try
      {
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

private:

  typename ImportFilterType::Pointer    m_ImportFilter;
  typename FilterType::Pointer          m_Filter;
  bool                                  m_LetITKAllocateOutputMemory;
  typename ExportFilterType::Pointer    m_ExportFilter;
};

} // end namespace vtkitk

#endif
