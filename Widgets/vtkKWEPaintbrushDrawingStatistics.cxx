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
#include "vtkKWEPaintbrushDrawingStatistics.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkExecutive.h"
#include "vtkAlgorithmOutput.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkKWEPaintbrushDataStatistics.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushDrawingStatistics, "$Revision: 590 $");
vtkStandardNewMacro(vtkKWEPaintbrushDrawingStatistics);

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawingStatistics::vtkKWEPaintbrushDrawingStatistics()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(0);
  this->Volume = 0;
  this->Volumes.clear();
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawingStatistics::~vtkKWEPaintbrushDrawingStatistics()
{
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDrawingStatistics
::RequestData( vtkInformation *,
               vtkInformationVector** inputVector,
               vtkInformationVector* vtkNotUsed( outputVector ))
{
  vtkInformation *info = inputVector[0]->GetInformationObject(0);
  vtkKWEPaintbrushDrawing *inputDrawing = vtkKWEPaintbrushDrawing::SafeDownCast(  
                             info->Get(vtkDataObject::DATA_OBJECT()));

  this->Volume = 0;
  this->Volumes.clear();
  const int nSketches = inputDrawing->GetNumberOfItems();
  for (int n = 0; n < nSketches; n++)
    {
    vtkKWEPaintbrushSketch * sketch = inputDrawing->GetItem(n);
    vtkKWEPaintbrushData * data = sketch->GetPaintbrushData();
    vtkKWEPaintbrushDataStatistics * stats = vtkKWEPaintbrushDataStatistics::New();
    stats->SetInput(data);
    stats->Update();

    const double sketchVolume = stats->GetVolume();
    this->Volumes.push_back(sketchVolume);
    this->Volume += sketchVolume;
    
    stats->Delete();
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDrawingStatistics::
FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkKWEPaintbrushDrawing");
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDrawingStatistics::
ProcessRequest(vtkInformation* request,
               vtkInformationVector** inputVector,
               vtkInformationVector* outputVector)
{
  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    this->RequestData(request, inputVector, outputVector);
    return 1;
    }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDrawingStatistics::GetVolume()
{
  this->Update(); 
  return this->Volume;
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDrawingStatistics::GetVolume(int n)
{
  this->Update(); 
  return static_cast<size_t>(n) < this->Volumes.size() ? this->Volumes[n] : 0.0;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawingStatistics::SetInput(vtkKWEPaintbrushDrawing* input)
{
  this->SetInputConnection(0,input->GetProducerPort());
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawingStatistics::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

