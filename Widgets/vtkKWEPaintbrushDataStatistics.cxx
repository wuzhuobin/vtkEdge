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
#include "vtkKWEPaintbrushDataStatistics.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkExecutive.h"
#include "vtkAlgorithmOutput.h"
#include "vtkImageData.h"
#include "vtkImageStencilData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkContourFilter.h"
#include "vtkImageConstantPad.h"
#include "vtkTriangleFilter.h"
#include "vtkMassProperties.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushDataStatistics, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushDataStatistics);

//----------------------------------------------------------------------------
vtkKWEPaintbrushDataStatistics::vtkKWEPaintbrushDataStatistics()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(0);
  this->ContourFilter    = NULL;
  this->TriangleFilter   = NULL;
  this->PadFilter        = NULL;
  this->MassProperties   = NULL;
  this->Volume           = 0;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDataStatistics::~vtkKWEPaintbrushDataStatistics()
{
  if (this->ContourFilter)
    {
    this->ContourFilter->Delete();
    }
  if (this->PadFilter)
    {
    this->PadFilter->Delete();
    }
  if (this->TriangleFilter)
    {
    this->TriangleFilter->Delete();
    }
  if (this->MassProperties)
    {
    this->MassProperties->Delete();
    }
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDataStatistics
::RequestData( vtkInformation *,
               vtkInformationVector** inputVector,
               vtkInformationVector* vtkNotUsed( outputVector ))
{
  vtkInformation *info = inputVector[0]->GetInformationObject(0);
  vtkKWEPaintbrushData *data = vtkKWEPaintbrushData::SafeDownCast(
                             info->Get(vtkDataObject::DATA_OBJECT()));

  this->Volume = 0;


  if (vtkKWEPaintbrushStencilData *sdata
      = vtkKWEPaintbrushStencilData::SafeDownCast(data))
    {

    vtkImageStencilData *stencilData = sdata->GetImageStencilData();

    // For binary data, we will compute the volume simply by counting the
    // number of voxels.
    // Find the number of voxels that are ON in the stencil.
    unsigned long nVoxels = 0;
    int moreSubExtents;

    int iter, r1, r2, extent[6];
    stencilData->GetExtent(extent);

    for (int z=extent[4]; z <= extent[5]; z++)
      {
      for (int y=extent[2] ; y <= extent[3]; y++)
        {
        iter = 0;
        moreSubExtents = 1;
        while( moreSubExtents )
          {
          moreSubExtents = stencilData->GetNextExtent(
            r1, r2, extent[0], extent[1], y, z, iter);

          // sanity check
          if (r1 <= r2)
            {
            nVoxels += (r2 - r1 + 1);
            }
          } // end for each extent tuple
        } // end for each scan line
      } // end of each slice

    // Volume = number of voxels * volume of a voxel.
    double spacing[3];
    stencilData->GetSpacing(spacing);
    this->Volume = static_cast<double>(nVoxels) * spacing[0] * spacing[1]
      * spacing[2];
    }

  else if (vtkKWEPaintbrushGrayscaleData *gdata =
      vtkKWEPaintbrushGrayscaleData::SafeDownCast(data))
    {

    // The pipeline to compute volume for grayscale data is
    // image --> PadFilter --> ContourFilter --> TriangleFilter
    //      --> vtkMassProperties --> Volume
    //
    // The pad filter is necessary to add a border around the image, otherwise
    // the contour filter might generate non-closed surfaces and
    // vtkMassProperties will end up giving us bogus values.

    vtkImageData *image = gdata->GetImageData();

    if (!this->PadFilter)
      {
      this->PadFilter = vtkImageConstantPad::New();
      }
    this->PadFilter->SetInput(image);
    int extent[6];
    image->GetExtent(extent);
    extent[0] -= 1;
    extent[1] += 1;
    extent[2] -= 1;
    extent[3] += 1;
    extent[4] -= 1;
    extent[5] += 1;
    this->PadFilter->SetOutputWholeExtent(extent);

    if (!this->ContourFilter)
      {
      this->ContourFilter = vtkContourFilter::New();
      this->ContourFilter->SetValue( 0, 127.5 );
      }
    this->ContourFilter->SetInput( this->PadFilter->GetOutput() );
    this->ContourFilter->Update();

    // Make sure we have something to compute the volume of.
    if (this->ContourFilter->GetOutput()->GetNumberOfPoints())
      {
      if (!this->TriangleFilter)
        {
        this->TriangleFilter = vtkTriangleFilter::New();
        }
      this->TriangleFilter->SetInput( this->ContourFilter->GetOutput());

      if (!this->MassProperties)
        {
        this->MassProperties = vtkMassProperties::New();
        }
      this->MassProperties->SetInput( this->TriangleFilter->GetOutput() );
      this->Volume = this->MassProperties->GetVolume();
      }

    }

  return 1;
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDataStatistics::GetVolume()
{
  this->Update();
  return this->Volume;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDataStatistics::
FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkKWEPaintbrushData");
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDataStatistics::
ProcessRequest(vtkInformation* request,
               vtkInformationVector** inputVector,
               vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    return this->RequestData(request, inputVector, outputVector);
    }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDataStatistics::SetInput(vtkKWEPaintbrushData* input)
{
  this->SetInputConnection(0,input->GetProducerPort());
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDataStatistics::GetOverlapVolume(
    vtkKWEPaintbrushStencilData * s1,
    vtkKWEPaintbrushStencilData * s2 )
{
  unsigned long nVoxels = 0;

  int s1Extent[6], s2Extent[6], commonExtent[6];
  s1->GetExtent( s1Extent );
  s2->GetExtent( s2Extent );

  vtkImageStencilData *s1data = s1->GetImageStencilData();
  vtkImageStencilData *s2data = s2->GetImageStencilData();

  if (vtkKWEPaintbrushUtilities::GetIntersectingExtents(
                s1Extent, s2Extent, commonExtent) )

    {

    // Find the number of voxels that are ON in both
    int moreSubExtents1, moreSubExtents2;

    int iter_1, iter_2, r1_1, r2_1, r1_2, r2_2;

    for (int z=commonExtent[4]; z <= commonExtent[5]; z++)
      {
      for (int y=commonExtent[2] ; y <= commonExtent[3]; y++)
        {
        iter_1 = 0;
        moreSubExtents1 = 1;
        while( moreSubExtents1 )
          {
          moreSubExtents1 = s1data->GetNextExtent(
            r1_1, r2_1, commonExtent[0], commonExtent[1], y, z, iter_1);

          if (r1_1 <= r2_1)
            {
            moreSubExtents2 = 1;
            iter_2 = 0;
            while( moreSubExtents2 )
              {
              moreSubExtents2 = s2data->GetNextExtent(
                 r1_2, r2_2, r1_1, r2_1, y, z, iter_2);

              if (r1_2 <= r2_2)
                {
                // There is an intersection
                nVoxels += (r2_2 - r1_2 + 1);

                } // end if there is a finite intersectingextent in data1

              } // end foreach extent tuple of data2
                // within the extent extracted from data1

            } // end if there is a finite extent in data1
          } // end for each extent tuple in data1
        } // end for each scan line
      } // end of each slice
    }

  double spacing[3];
  s1data->GetSpacing(spacing);
  return static_cast<double>(nVoxels) * spacing[0] * spacing[1] * spacing[2];
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDataStatistics::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

