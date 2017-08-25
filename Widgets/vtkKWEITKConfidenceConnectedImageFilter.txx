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
#include "vtkKWEITKConfidenceConnectedImageFilter.h"
#include "itkNumericTraits.h"
#include "vtkKWEITKFilterModuleStencilOutput.h"

namespace vtkitk
{

template< class InputPixelType >
vtkKWEITKConfidenceConnectedImageFilter< InputPixelType >
::vtkKWEITKConfidenceConnectedImageFilter()
{
  this->m_Filter             = new FilterModuleStencilOutput< FilterType >();
}  

template< class InputPixelType >
vtkKWEITKConfidenceConnectedImageFilter< InputPixelType >
::~vtkKWEITKConfidenceConnectedImageFilter()
{
  delete this->m_Filter;
}

template< class InputPixelType >
void
vtkKWEITKConfidenceConnectedImageFilter< InputPixelType >
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
  filter->GetRegionGrowingFilter()->AddSeed(index);
}

template< class InputPixelType >
void
vtkKWEITKConfidenceConnectedImageFilter< InputPixelType >
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
  
  FilterType *filter = f->GetFilter();
  
  // Override the propagation of extents.. the voting filters require that 
  // the input be 1/2 the structuring element size larger on each side.
  // So we will shrink the output extents appropriately.
  if (filter->GetUseHoleFilling())
    { 
    typename InputImageType::SizeType radius = 
          filter->GetHoleFillingFilter()->GetRadius();
    int extent[6];
    memcpy(extent, this->m_Extent, 6*sizeof(int));
    extent[0] += radius[0];
    extent[2] += radius[1];
    extent[4] += radius[2];
    extent[1] -= radius[0];
    extent[3] -= radius[1];
    extent[5] -= radius[2];
    if (extent[0] > extent[1] ||
        extent[2] > extent[3] ||
        extent[4] > extent[5])
      {
      // The extracted region on which region growing is being done is
      // too small to be shrunk any further.. turn holefilling off.
      filter->UseHoleFillingOff();
      }
    else
      {
      memcpy(this->m_Extent, extent, 6*sizeof(int));
      this->m_Filter->SetRequestedExtent(this->m_Extent);
      }
    }
  
  // Update the filter.. If any timing analysis needs to be done.. 
  // This method needs to be interactive enough.
  f->Update();
}

} // end namespace vtkitk

