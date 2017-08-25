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

