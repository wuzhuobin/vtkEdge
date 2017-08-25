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

#ifndef _itkVTKImageToImageFilter_txx
#define _itkVTKImageToImageFilter_txx

#include "itkVTKImageToImageFilter.h"

namespace itk
{



/**
 * Constructor
 */
template <class TOutputImage>
VTKImageToImageFilter<TOutputImage>
::VTKImageToImageFilter()
{

  m_Exporter = vtkImageExport::New();

  m_Importer = ImporterFilterType::New();

  m_Importer->SetUpdateInformationCallback( m_Exporter->GetUpdateInformationCallback());
  m_Importer->SetPipelineModifiedCallback( m_Exporter->GetPipelineModifiedCallback());
  m_Importer->SetWholeExtentCallback( m_Exporter->GetWholeExtentCallback());
  m_Importer->SetSpacingCallback( m_Exporter->GetSpacingCallback());
  m_Importer->SetOriginCallback( m_Exporter->GetOriginCallback());
  m_Importer->SetScalarTypeCallback( m_Exporter->GetScalarTypeCallback());
  m_Importer->SetNumberOfComponentsCallback( m_Exporter->GetNumberOfComponentsCallback());
  m_Importer->SetPropagateUpdateExtentCallback( m_Exporter->GetPropagateUpdateExtentCallback());
  m_Importer->SetUpdateDataCallback( m_Exporter->GetUpdateDataCallback());
  m_Importer->SetDataExtentCallback( m_Exporter->GetDataExtentCallback());
  m_Importer->SetBufferPointerCallback( m_Exporter->GetBufferPointerCallback());
  m_Importer->SetCallbackUserData( m_Exporter->GetCallbackUserData());

}




/**
 * Destructor
 */
template <class TOutputImage>
VTKImageToImageFilter<TOutputImage>
::~VTKImageToImageFilter()
{
  if( m_Exporter )
    {
    m_Exporter->Delete();
    m_Exporter = 0;
    }
}



/**
 * Set a vtkImageData as input
 */
template <class TOutputImage>
void
VTKImageToImageFilter<TOutputImage>
::SetInput( vtkImageData * inputImage )
{
  m_Exporter->SetInput( inputImage );
}

/**
 * Get the vtkImageData set as input
 */
template <class TOutputImage>
vtkImageData *
VTKImageToImageFilter<TOutputImage>
::GetInput()
{
  return this->m_Exporter->GetInput();
}


/**
 * Get an itk::Image as output
 */
template <class TOutputImage>
const typename VTKImageToImageFilter<TOutputImage>::OutputImageType *
VTKImageToImageFilter<TOutputImage>
::GetOutput() const
{
  return m_Importer->GetOutput();
}




/**
 * Get the exporter filter
 */
template <class TOutputImage>
vtkImageExport *
VTKImageToImageFilter<TOutputImage>
::GetExporter() const
{
  return m_Exporter;
}



/**
 * Get the importer filter
 */
template <class TOutputImage>
typename VTKImageToImageFilter<TOutputImage>::ImporterFilterType *
VTKImageToImageFilter<TOutputImage>
::GetImporter() const
{
  return m_Importer;
}




/**
 * Delegate the Update to the importer
 */
template <class TOutputImage>
void
VTKImageToImageFilter<TOutputImage>
::Update()
{
  m_Importer->Update();
}




} // end namespace itk

#endif

