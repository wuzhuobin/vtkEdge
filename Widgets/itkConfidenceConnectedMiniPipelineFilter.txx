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
#ifndef __itkConfidenceConnectedMiniPipelineFilter_txx_
#define __itkConfidenceConnectedMiniPipelineFilter_txx_

#include "itkConfidenceConnectedMiniPipelineFilter.h"
#include "itkProgressAccumulator.h"
#include "itkNumericTraits.h"

namespace itk
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputImage>
ConfidenceConnectedMiniPipelineFilter<TInputImage, TOutputImage>
::ConfidenceConnectedMiniPipelineFilter()
{
  this->m_SmoothingFilter             = SmoothingFilterType::New();
  this->m_RegionGrowingFilter         = RegionGrowingFilterType::New();
  this->m_HoleFillingFilter           = HoleFillingFilterType::New();

  // Set up the pipeline
  this->m_SmoothingFilter->SetInput(this->GetInput());
  this->m_RegionGrowingFilter->SetInput(this->m_SmoothingFilter->GetOutput());
  this->m_HoleFillingFilter->SetInput(this->m_RegionGrowingFilter->GetOutput());

  // Set up some default parameters of the pipeline filters

  // Smoothing filter params

  typename InputImageType::SizeType medianFilterRadius;
  medianFilterRadius.Fill(1);
  this->m_SmoothingFilter->SetRadius(medianFilterRadius); // 3 x 3 kernel

  // Region growing filter params

  this->m_RegionGrowingFilter->SetNumberOfIterations(2);
  this->m_RegionGrowingFilter->SetMultiplier(1.5);
  this->m_RegionGrowingFilter->SetInitialNeighborhoodRadius(1);
  this->m_RegionGrowingFilter->SetReplaceValue(
      itk::NumericTraits< typename OutputImageType::PixelType >::max() );

  // Hole filling filter params

  typename InputImageType::SizeType holeFillingFilterRadius;
  holeFillingFilterRadius.Fill(2);
  this->m_HoleFillingFilter->SetRadius(holeFillingFilterRadius);
  this->m_HoleFillingFilter->SetBackgroundValue(
      itk::NumericTraits< typename OutputImageType::PixelType >::Zero );
  this->m_HoleFillingFilter->SetForegroundValue(
      itk::NumericTraits< typename OutputImageType::PixelType >::max() );
  this->m_HoleFillingFilter->SetMajorityThreshold(20);
  this->m_UseHoleFilling = true;
}


/**
 * Standard PrintSelf method.
 */
template <class TInputImage, class TOutputImage>
void
ConfidenceConnectedMiniPipelineFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "SmoothingFilter: " << std::endl;
    this->m_SmoothingFilter->Print(os,indent.GetNextIndent());
  os << indent << "RegionGrowingFilter: " << std::endl;
    this->m_RegionGrowingFilter->Print(os,indent.GetNextIndent());
  os << indent << "HoleFillingFilter: " << std::endl;
    this->m_HoleFillingFilter->Print(os,indent.GetNextIndent());
  if (this->m_UseHoleFilling)
    {
    os << indent << "UseHoleFilling: On" << std::endl;
    }
  else
    {
    os << indent << "UseHoleFilling: Off" << std::endl;
    }
}

template <class TInputImage, class TOutputImage>
void
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();

  // Why am I setting the region to largest possible region ?
  //   Because the region growing filters in ITK do that.
  // Why do the region growing filters in ITK do that ?
  //   I don't know.
  if ( this->GetInput() )
    {
    typename InputImageType::Pointer image =
      const_cast< TInputImage * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template <class TInputImage, class TOutputImage>
void
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GenerateOutputInformation()
{
  Superclass::GenerateOutputInformation();

  if (!this->m_UseHoleFilling)
    {
    return;
    }

  const TInputImage * inputPtr = this->GetInput();
  if( !inputPtr )
    {
    return;
    }

  // If we use holefilling, the voting filters require that
  // the input be 1/2 the structuring element size larger on each side.
  // So we will shrink the output

  typename OutputImageType::RegionType outputRegion;
  typename OutputImageType::SizeType   sz;
  typename OutputImageType::IndexType  idx;

  typename InputImageType::SizeType input_sz =
    inputPtr->GetLargestPossibleRegion().GetSize();
  typename InputImageType::IndexType input_idx =
    inputPtr->GetLargestPossibleRegion().GetIndex();

  idx = input_idx + this->m_HoleFillingFilter->GetRadius();
  sz  = input_sz  - this->m_HoleFillingFilter->GetRadius();
  sz  = sz        - this->m_HoleFillingFilter->GetRadius();

  outputRegion.SetSize(sz);
  outputRegion.SetIndex(idx);

  this->GetOutput()->SetLargestPossibleRegion(outputRegion);
}

template <class TInputImage, class TOutputImage>
void
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GenerateData()
{
  // Allocate the output
  this->AllocateOutputs();

  this->m_SmoothingFilter->SetInput(this->GetInput());
  // Create a process accumulator for tracking the progress of this
  // minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter( this->m_SmoothingFilter,     1.0/3.0);
  progress->RegisterInternalFilter( this->m_RegionGrowingFilter, 1.0/3.0);
  progress->RegisterInternalFilter( this->m_HoleFillingFilter,   1.0/3.0);
  progress->ResetProgress();

  // Graft this filter's output to the mini-pipeline.  this sets up
  // the mini-pipeline to write to this filter's output and copies
  // region ivars and meta-data
  if (this->m_UseHoleFilling)
    {
    this->m_HoleFillingFilter->GraftOutput(this->GetOutput());
    this->m_HoleFillingFilter->Update();
    }
  else
    {
    this->m_RegionGrowingFilter->GraftOutput(this->GetOutput());
    this->m_RegionGrowingFilter->Update();
    }
}

template <class TInputImage, class TOutputImage>
typename ConfidenceConnectedMiniPipelineFilter<
         TInputImage,TOutputImage>::SmoothingFilterType *
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GetSmoothingFilter()
{
  return this->m_SmoothingFilter.GetPointer();
}

template <class TInputImage, class TOutputImage>
typename ConfidenceConnectedMiniPipelineFilter<
         TInputImage,TOutputImage>::RegionGrowingFilterType *
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GetRegionGrowingFilter()
{
  return this->m_RegionGrowingFilter.GetPointer();
}

template <class TInputImage, class TOutputImage>
typename ConfidenceConnectedMiniPipelineFilter<
         TInputImage,TOutputImage>::HoleFillingFilterType *
ConfidenceConnectedMiniPipelineFilter<TInputImage,TOutputImage>
::GetHoleFillingFilter()
{
  return this->m_HoleFillingFilter.GetPointer();
}

} // end namespace itk

#endif
