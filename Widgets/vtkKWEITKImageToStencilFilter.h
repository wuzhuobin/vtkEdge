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

#ifndef _vtkKWEITKImageToStencilFilter_h
#define _vtkKWEITKImageToStencilFilter_h

#include "VTKEdgeConfigure.h" // Needed for export symbols
#include "vtkKWEITKFilterModule.h"

class vtkImageData;
class vtkImageStencilData;

namespace vtkitk
{

class VTKEdge_WIDGETS_EXPORT vtkKWEITKImageToStencilFilter 
  : public itk::Object
{
public:
  typedef vtkKWEITKImageToStencilFilter                   Self;
  typedef itk::Object                                  Superclass;
  typedef itk::SmartPointer< Self >                    Pointer;
  typedef itk::SmartPointer< const Self >              ConstPointer;
  itkTypeMacro(ConfidenceConnectedRunner, itk::Object);
  
  itkStaticConstMacro( Dimension, unsigned int, 3);
  typedef  unsigned char                            OutputPixelType;
  typedef  itk::Image< OutputPixelType, Dimension > OutputImageType; 
  typedef  FilterModuleBase                         InternalFilterType;
  
  // No MTime checks etc.. will update every time.. 
  virtual void Update();
  virtual void SetInput( vtkImageData * );
  virtual int  GetOutputAsStencil( vtkImageStencilData * );
  virtual void SetRequestedExtent( int extent[6] );

  itkSetMacro(               BoundWithRadius, bool );
  itkGetConstReferenceMacro( BoundWithRadius, bool );
  itkBooleanMacro(           BoundWithRadius       );

  void SetCenter( int center[3] );
  void SetRadius( double radius[3] );

protected:
  vtkKWEITKImageToStencilFilter();
  ~vtkKWEITKImageToStencilFilter();
  
  InternalFilterType          * m_Filter;
  int                           m_Extent[6]; 
  bool                          m_BoundWithRadius;
  int                           m_Center[3];
  double                        m_Radius[3];
};

} // end namespace vtkitk

#endif

