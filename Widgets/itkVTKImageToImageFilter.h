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
#ifndef __itkVTKImageToImageFilter_h
#define __itkVTKImageToImageFilter_h

#include "VTKEdgeConfigure.h" // Needed for export symbol def
#include "itkVTKImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageData.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

namespace itk
{

/** \class VTKImageToImageFilter
 * \brief Converts a VTK image into an ITK image and plugs a
 *  vtk data pipeline to an ITK datapipeline.
 *
 *  This class puts together an itkVTKImageImporter and a vtkImageExporter.
 *  It takes care of the details related to the connection of ITK and VTK
 *  pipelines. The User will perceive this filter as an adaptor to which
 *  a vtkImage can be plugged as input and an itk::Image is produced as
 *  output.
 *
 * \ingroup   ImageFilters
 */
template <class TOutputImage >
class ITK_EXPORT VTKImageToImageFilter : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef VTKImageToImageFilter       Self;
  typedef ProcessObject             Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VTKImageToImageFilter, ProcessObject);

  /** Some typedefs. */
  typedef TOutputImage OutputImageType;
  typedef typename    OutputImageType::ConstPointer    OutputImagePointer;
  typedef VTKImageImport< OutputImageType >   ImporterFilterType;
  typedef typename ImporterFilterType::Pointer         ImporterFilterPointer;

  /** Get the output in the form of a vtkImage.
      This call is delegated to the internal vtkImageImporter filter  */
  const OutputImageType *  GetOutput() const;

  /** Set the input in the form of a vtkImageData */
  void SetInput( vtkImageData * );
  vtkImageData * GetInput();

  /** Return the internal VTK image exporter filter.
      This is intended to facilitate users the access
      to methods in the exporter */
  vtkImageExport * GetExporter() const;

  /** Return the internal ITK image importer filter.
      This is intended to facilitate users the access
      to methods in the importer */
  ImporterFilterType * GetImporter() const;

  /** This call delegate the update to the importer */
  void Update();

protected:
  VTKImageToImageFilter();
  virtual ~VTKImageToImageFilter();

private:
  VTKImageToImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ImporterFilterPointer       m_Importer;
  vtkImageExport            * m_Exporter;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVTKImageToImageFilter.txx"
#endif

#endif



