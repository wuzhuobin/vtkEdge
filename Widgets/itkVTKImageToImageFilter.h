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



