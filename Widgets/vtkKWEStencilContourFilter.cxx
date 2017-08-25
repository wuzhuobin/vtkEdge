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
#include "vtkKWEStencilContourFilter.h"

#include "vtkKWEPaintbrushUtilities.h"
#include "vtkContourFilter.h"
#include "vtkImageStencilData.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkCxxRevisionMacro(vtkKWEStencilContourFilter, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEStencilContourFilter);

//----------------------------------------------------------------------------
vtkKWEStencilContourFilter::vtkKWEStencilContourFilter()
{
  this->ContourFilter = vtkContourFilter::New();
  this->ImageData     = vtkImageData::New();
  this->ImageData->SetScalarTypeToUnsignedChar();
  this->ImageData->SetNumberOfScalarComponents(1);
  this->ContourFilter->SetInput( this->ImageData );
  this->ContourFilter->SetValue(0, 127.5);
  this->Extent[0] = this->Extent[2] = this->Extent[4] = 0;
  this->Extent[1] = this->Extent[3] = this->Extent[5] = -1;
}

//----------------------------------------------------------------------------
vtkKWEStencilContourFilter::~vtkKWEStencilContourFilter()
{
  this->ContourFilter->Delete();
  this->ImageData->Delete();
}

//----------------------------------------------------------------------------
// Overload standard modified time function. If contour filter is modified,
// then this object is modified as well.
unsigned long vtkKWEStencilContourFilter::GetMTime()
{
  unsigned long mTime=this->Superclass::GetMTime(), time;
  if (this->ContourFilter)
    {
    time = this->ContourFilter->GetMTime();
    mTime = ( time > mTime ? time : mTime );
    }
  return mTime;
}

//----------------------------------------------------------------------------
// This method computes the Region of input necessary to generate outRegion.
// We will take the intersection of the stencil and the supplied clipping
// extents, (if they have been specified), otherwise we will use the whole
// extent of the stencil.
//
int vtkKWEStencilContourFilter::RequestUpdateExtent(vtkInformation* ,
                                          vtkInformationVector** inputVector,
                                          vtkInformationVector* )
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageStencilData *input = vtkImageStencilData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  int extent[6], uExt[6];
  input->GetExtent(extent);

  // Now clip this with the bounding extents, if supplied.
  if (this->Extent[0] <= this->Extent[1])
    {
    if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents( this->Extent, extent, uExt ))
      {
      // They don't intersect..
      inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), this->Extent, 6);
      }
    else
      {
      inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), uExt, 6);
      }
    return 1;
    }

  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), extent, 6);
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::RequestData(
  vtkInformation* ,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the input and output
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageStencilData *input = vtkImageStencilData::SafeDownCast(
                        inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *output = vtkPolyData::SafeDownCast(
          outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!input || !output)
    {
    return 0;
    }

  int uExt[6], currentExtent[6], inputExtent[6], intersectingExtents[6];
  inInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), uExt);
  this->ImageData->GetExtent(currentExtent);

  // Check if the input is outside the update extent. If so, output = NULL.
  input->GetExtent(inputExtent);
  if (vtkKWEPaintbrushUtilities::GetIntersectingExtents(
        uExt, inputExtent, intersectingExtents))
    {
    if (intersectingExtents[0] != currentExtent[0] ||
        intersectingExtents[1] != currentExtent[1] ||
        intersectingExtents[2] != currentExtent[2] ||
        intersectingExtents[3] != currentExtent[3] ||
        intersectingExtents[4] != currentExtent[4] ||
        intersectingExtents[5] != currentExtent[5] )
      {
      this->ImageData->SetExtent(intersectingExtents);
      this->ImageData->AllocateScalars();
      }

    // Popualate image from the stencil
    this->ImageData->SetSpacing(input->GetSpacing());
    this->ImageData->SetOrigin(input->GetOrigin());
    vtkKWEPaintbrushUtilities::GetImageFromStencil(
            this->ImageData, input, 255, 0, true);

      this->ContourFilter->Update();
      output->ShallowCopy(this->ContourFilter->GetOutput());
      return 1;
    }

  // Nothing is there here.
  output->SetPoints(NULL);
  output->SetLines(NULL);
  output->SetPolys(NULL);
  return 0;
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageStencilData");
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::SetValue(int i, double value)
{
  this->ContourFilter->SetValue(i,value);
}

//----------------------------------------------------------------------------
double vtkKWEStencilContourFilter::GetValue(int i)
{
  return this->ContourFilter->GetValue(i);
}

//----------------------------------------------------------------------------
double *vtkKWEStencilContourFilter::GetValues()
{
  return this->ContourFilter->GetValues();
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::GetValues(double *contourValues)
{
  this->ContourFilter->GetValues(contourValues);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::SetNumberOfContours(int number)
{
  this->ContourFilter->SetNumberOfContours(number);
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::GetNumberOfContours()
{
  return this->ContourFilter->GetNumberOfContours();
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::GenerateValues(int numContours, double range[2])
{
  this->ContourFilter->GenerateValues(numContours, range);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::GenerateValues(int numContours, double
                                             rangeStart, double rangeEnd)
{
  this->ContourFilter->GenerateValues(numContours, rangeStart, rangeEnd);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::SetComputeGradients(int s)
{
  this->ContourFilter->SetComputeGradients(s);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::SetComputeNormals(int s)
{
  this->ContourFilter->SetComputeNormals(s);
}

//----------------------------------------------------------------------------
void vtkKWEStencilContourFilter::SetComputeScalars(int s)
{
  this->ContourFilter->SetComputeScalars(s);
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::GetComputeGradients()
{
  return this->ContourFilter->GetComputeGradients();
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::GetComputeNormals()
{
  return this->ContourFilter->GetComputeNormals();
}

//----------------------------------------------------------------------------
int vtkKWEStencilContourFilter::GetComputeScalars()
{
  return this->ContourFilter->GetComputeScalars();
}

