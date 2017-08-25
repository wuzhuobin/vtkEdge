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
#include "vtkKWEITKConnectedThresholdPaintbrushOperation.h"
#include "vtkKWEITKConnectedThresholdImageFilter.h"
#include "vtkKWEPaintbrushStroke.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkImageStencilData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExtractComponents.h"
#include "vtkExtractVOI.h"
#include "vtkKWEPaintbrushUtilities.h"

vtkCxxRevisionMacro(vtkKWEITKConnectedThresholdPaintbrushOperation, "$Revision: 742 $");
vtkStandardNewMacro(vtkKWEITKConnectedThresholdPaintbrushOperation);

//----------------------------------------------------------------------
vtkKWEITKConnectedThresholdPaintbrushOperation::vtkKWEITKConnectedThresholdPaintbrushOperation()
{
}

//----------------------------------------------------------------------
vtkKWEITKConnectedThresholdPaintbrushOperation::~vtkKWEITKConnectedThresholdPaintbrushOperation()
{
}

template< class T > int 
PaintbrushRunner( vtkKWEITKConnectedThresholdPaintbrushOperation * self, 
                  double center[3], 
                  vtkImageStencilData *stencil,
                  T )
{
  self->InternalFilter = NULL;

  typedef vtkitk::vtkKWEITKConnectedThresholdImageFilter< T > PaintbrushFilterType;
  typename PaintbrushFilterType::Pointer filter = PaintbrushFilterType::New();
  self->InternalFilter = filter;

  // Set the update region to a certain region on either side of the center
  double spacing[3];
  double origin[3];
  int imageExtent[6];
  self->GetImageData()->GetOrigin(origin);
  self->GetImageData()->GetSpacing(spacing);
  self->GetImageData()->GetExtent(imageExtent);

  int xyz[3] = { (int)(center[0]/spacing[0] + 0.5),
                 (int)(center[1]/spacing[1] + 0.5),
                 (int)(center[2]/spacing[2] + 0.5) };

  int extent[6];
  if( self->GetFilterHalfWidth()[0] >= 0 && 
      self->GetFilterHalfWidth()[1] >= 0 && 
      self->GetFilterHalfWidth()[2] >= 0 )
    {
    extent[0] = (int)(xyz[0]-self->GetFilterHalfWidth()[0]);
    extent[1] = (int)(xyz[0]+self->GetFilterHalfWidth()[0]);
    extent[2] = (int)(xyz[1]-self->GetFilterHalfWidth()[1]);
    extent[3] = (int)(xyz[1]+self->GetFilterHalfWidth()[1]);
    extent[4] = (int)(xyz[2]-self->GetFilterHalfWidth()[2]);
    extent[5] = (int)(xyz[2]+self->GetFilterHalfWidth()[2]);
    vtkKWEPaintbrushUtilities::GetIntersectingExtents(extent, imageExtent, extent);
    vtkKWEPaintbrushUtilities::GetIntersectingExtents(extent, self->GetExtent(), extent);
    }
  else
    {
    vtkKWEPaintbrushUtilities::GetIntersectingExtents(self->GetExtent(), imageExtent, extent);
    }
  
  // Despite the fact that the FilterModule framework supports updates on 
  // requested extents, a lot of filters in ITK (such as the 
  // ConnectedThresholdImageFilter don't really support updating a subextent.
  // So in most cases, you will have to extract a VOI).
  vtkExtractVOI *extractVOI = vtkExtractVOI::New();
  extractVOI->SetInput( self->GetImageData());
  extractVOI->SetVOI(extent);

  // Extract the first component
  vtkImageExtractComponents * extractComponent = vtkImageExtractComponents::New();
  extractComponent->SetInput( extractVOI->GetOutput() );
  //extractComponent->GetOutput()->SetUpdateExtent( extent );
  extractComponent->SetComponents(0);
  extractComponent->Update();
  //filter->SetRequestedExtent(extent);
  
  // This is the filter that does most of the work. This is where most of the
  // time for this operation is spent
  filter->SetInput( extractComponent->GetOutput() );
  // Should add a seed for every point in the stencil
  filter->AddSeed( xyz );
  filter->SetRequestedExtent(extent);
  //filter->BoundWithRadiusOn();
  filter->SetCenter( xyz );
  filter->SetRadius( self->GetFilterHalfWidth() );
  filter->Update();
  filter->GetOutputAsStencil(stencil);

  extractComponent->Delete();
  extractVOI->Delete();
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEITKConnectedThresholdPaintbrushOperation::
DoOperationOnStencil(vtkImageStencilData *stencilData, double p[3] )
{
  this->PaintbrushShape->GetStencil( stencilData, p );

  vtkImageStencilData * stencil = vtkImageStencilData::New();
  switch( this->ImageData->GetScalarType() )
    {
    vtkitkTemplateMacro( PaintbrushRunner( this, p, stencil, 
                         static_cast< VTK_TT >(0)) );
    
    default:
      {
      vtkErrorMacro(<< 
          "vtkKWEITKConnectedThresholdPaintbrushOperation: Unknown ScalarType");
      break;
      }
    }
  
  stencilData->Add(stencil);
  stencil->Delete();
}

//----------------------------------------------------------------------
void vtkKWEITKConnectedThresholdPaintbrushOperation::
DoOperation( vtkKWEPaintbrushData *data, double p[3],
             vtkKWEPaintbrushEnums::OperationType & op )
{
  op = vtkKWEPaintbrushEnums::Add;

  // The paintbrush data can be binary or grayscale. Invoke the appropriate
  // filtering operation in each case.

  vtkKWEPaintbrushStencilData *sdata = 
      vtkKWEPaintbrushStencilData::SafeDownCast(data);
  if ( sdata )
    {
    this->DoOperationOnStencil( sdata->GetImageStencilData(), p ); 
    }
}

//----------------------------------------------------------------------
void vtkKWEITKConnectedThresholdPaintbrushOperation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

