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
#ifndef __itkConfidenceConnectedMiniPipelineFilter_h
#define __itkConfidenceConnectedMiniPipelineFilter_h

#include "VTKEdgeConfigure.h" // Needed for export symbol def
#include "itkConfidenceConnectedImageFilter.h"
#include "itkVotingBinaryHoleFillingImageFilter.h"
#include "itkMedianImageFilter.h"


namespace itk{

/** \class ConfidenceConnectedMiniPipelineFilter
 * \brief Confidence connected mini pipeline filter
 *
 * This class is a wrapper around a set of filters that perform region
 * growing on an image. The filter internally uses a three step process
 * for region growing.
 *
 *  Nonlinear Smoothing ->   Region growing  -> Hole filling
 *
 * The smoothing process employed here is Median filter with a 3x3 kernel,
 * cause its fast. The region growing process is 
 * ConfidenceConnectedRegionGrowing. Hole filling is accomplised by a 
 * binary voting filter.
 *
 * The class gives you access to the three filters. It is up to you to
 * set the parameters to those filters. You can turn off hole filling if
 * you like.
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT ConfidenceConnectedMiniPipelineFilter:
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ConfidenceConnectedMiniPipelineFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods).  */
  itkTypeMacro(ConfidenceConnectedMiniPipelineFilter, ImageToImageFilter);

  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;

  /** Pipeline filters */
  typedef MedianImageFilter< 
    InputImageType, InputImageType >            SmoothingFilterType;
  typedef ConfidenceConnectedImageFilter< 
        InputImageType, OutputImageType >       RegionGrowingFilterType;
  typedef VotingBinaryHoleFillingImageFilter< 
    OutputImageType, OutputImageType >          HoleFillingFilterType;

  typedef typename 
    RegionGrowingFilterType::SeedsContainerType SeedsContainerType;
  
  void PrintSelf ( std::ostream& os, Indent indent ) const;

  /** Get the pipeline filters */
  virtual SmoothingFilterType     * GetSmoothingFilter();
  virtual RegionGrowingFilterType * GetRegionGrowingFilter();
  virtual HoleFillingFilterType   * GetHoleFillingFilter();

  /** Do hole filling.. turn this off if needed. On by default */
  itkSetMacro(               UseHoleFilling, bool );
  itkGetConstReferenceMacro( UseHoleFilling, bool );
  itkBooleanMacro(           UseHoleFilling       );
  
protected:
  ConfidenceConnectedMiniPipelineFilter();
  ~ConfidenceConnectedMiniPipelineFilter(){};
  
  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion();
  virtual void GenerateOutputInformation();
  void GenerateData();
  
private:
  ConfidenceConnectedMiniPipelineFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename SmoothingFilterType::Pointer       m_SmoothingFilter;
  typename RegionGrowingFilterType::Pointer   m_RegionGrowingFilter;
  typename HoleFillingFilterType::Pointer     m_HoleFillingFilter;
  bool                                        m_UseHoleFilling;
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConfidenceConnectedMiniPipelineFilter.txx"
#endif

#endif
