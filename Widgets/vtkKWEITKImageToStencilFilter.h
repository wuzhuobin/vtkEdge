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

