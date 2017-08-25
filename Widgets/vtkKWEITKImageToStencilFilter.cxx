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

#include "vtkKWEITKImageToStencilFilter.h"
#include "vtkImageStencilData.h"
#include "vtkImageData.h"

namespace vtkitk
{

vtkKWEITKImageToStencilFilter::vtkKWEITKImageToStencilFilter()
{
  for (int i=0; i < 6; i++)
    {
    this->m_Extent[i] = -1;
    }
  this->m_BoundWithRadius = true;
  this->m_Radius[0] = 0;
  this->m_Radius[1] = 0;
  this->m_Radius[2] = 0;
  this->m_Center[0] = 0;
  this->m_Center[1] = 0;
  this->m_Center[2] = 0;
}

vtkKWEITKImageToStencilFilter::~vtkKWEITKImageToStencilFilter()
{
}

void vtkKWEITKImageToStencilFilter::SetInput( vtkImageData * image )
{
  this->m_Filter->SetInput(image);
}

void vtkKWEITKImageToStencilFilter::SetCenter( int center[3] )
{
  m_Center[0] = center[0];
  m_Center[1] = center[1];
  m_Center[2] = center[2];
}

void vtkKWEITKImageToStencilFilter::SetRadius( double radius[3] )
{
  m_Radius[0] = radius[0];
  m_Radius[1] = radius[1];
  m_Radius[2] = radius[2];
}

int vtkKWEITKImageToStencilFilter::GetOutputAsStencil(
                  vtkImageStencilData *stencilData )
{
  if (this->m_BoundWithRadius)
    {
    return this->m_Filter->GetSphereBoundedOutputAsStencil(stencilData,
                                                           m_Center,
                                                           m_Radius);
    }
  else
    {
    return this->m_Filter->GetOutputAsStencil(stencilData);
    }
}

void vtkKWEITKImageToStencilFilter::SetRequestedExtent( int extent[6] )
{
  if (this->m_Extent[0] == extent[0] &&
      this->m_Extent[1] == extent[1] &&
      this->m_Extent[2] == extent[2] &&
      this->m_Extent[3] == extent[3] &&
      this->m_Extent[4] == extent[4] &&
      this->m_Extent[5] == extent[5])
    {
    return;
    }

  for (int i=0; i < 6; i++)
    {
    this->m_Extent[i] = extent[i];
    }
  this->Modified();
}

void vtkKWEITKImageToStencilFilter::Update()
{
  if ((this->m_Extent[0]) != (-1))
    {
    int extent[6];
    this->m_Filter->GetInput()->GetExtent(extent);
    m_Extent[0] = (m_Extent[0] < extent[0]) ? extent[0] : m_Extent[0];
    m_Extent[1] = (m_Extent[1] > extent[1]) ? extent[1] : m_Extent[1];
    m_Extent[2] = (m_Extent[2] < extent[2]) ? extent[2] : m_Extent[2];
    m_Extent[3] = (m_Extent[3] > extent[3]) ? extent[3] : m_Extent[3];
    m_Extent[4] = (m_Extent[4] < extent[4]) ? extent[4] : m_Extent[4];
    m_Extent[5] = (m_Extent[5] > extent[5]) ? extent[5] : m_Extent[5];
    }

  this->m_Filter->SetRequestedExtent(this->m_Extent);
}

} // end namespace vtkitk

