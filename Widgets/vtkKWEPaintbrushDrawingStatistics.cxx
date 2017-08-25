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
#include "vtkKWEPaintbrushDrawingStatistics.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkExecutive.h"
#include "vtkAlgorithmOutput.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushDataStatistics.h"
#include "vtkImageData.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushDrawingStatistics, "$Revision: 3258 $");
vtkStandardNewMacro(vtkKWEPaintbrushDrawingStatistics);

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawingStatistics::vtkKWEPaintbrushDrawingStatistics()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(0);
  this->Volume = 0;
  this->Volumes.clear();
  
  this->MaximumLabelValue = 1;
  for (int i = 0; i < sizeof( vtkKWEPaintbrushEnums::LabelType ); i++)
    {
    this->MaximumLabelValue *= 256;
    }
  this->VolumesArray = new unsigned long[this->MaximumLabelValue];
  
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawingStatistics::~vtkKWEPaintbrushDrawingStatistics()
{
  delete this->VolumesArray;
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

  if (inputDrawing->GetRepresentation() != vtkKWEPaintbrushEnums::Label)
    {
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
    }
  else
    {

    for (unsigned long i = 0; i < this->MaximumLabelValue; i++)
      {
      this->VolumesArray[i] = 0;
      }      

    vtkKWEPaintbrushLabelData *ldata = vtkKWEPaintbrushLabelData::SafeDownCast(
        inputDrawing->GetPaintbrushData());
    vtkImageData *labelImage = ldata->GetLabelMap();
    
    vtkDataArray * array = labelImage->GetPointData()->GetScalars();
    vtkKWEPaintbrushEnums::LabelType *arrayPointer =
      static_cast<vtkKWEPaintbrushEnums::LabelType *>(array->GetVoidPointer(0));

    const unsigned long size = array->GetDataSize();
    for (unsigned long i = 0; i < size; ++i, ++arrayPointer)
      {
      vtkKWEPaintbrushEnums::LabelType l = *arrayPointer;
      if (l != vtkKWEPaintbrushLabelData::NoLabelValue)
        {
        ++(this->VolumesArray[l]);
        }
      }

    double spacing[3];
    ldata->GetSpacing(spacing);
    const double voxelVolume = spacing[0] * spacing[1] * spacing[2];

    for (unsigned long i = 0; i < this->MaximumLabelValue; i++)
      {
      this->Volume += ((double)(this->VolumesArray[i]) * voxelVolume);
      }
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
vtkKWEPaintbrushDrawing * vtkKWEPaintbrushDrawingStatistics::GetDrawing()
{
  return vtkKWEPaintbrushDrawing::SafeDownCast(
    this->GetExecutive()->GetInputData(0, 0));
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDrawingStatistics::GetVolume(int n)
{
  this->Update();

  if (this->GetDrawing()->GetRepresentation() != vtkKWEPaintbrushEnums::Label)
    {
    return static_cast<size_t>(n) < this->Volumes.size() ? this->Volumes[n] : 0.0;
    }
  else
    {
    double spacing[3];
    vtkKWEPaintbrushLabelData *ldata = vtkKWEPaintbrushLabelData::SafeDownCast(
        this->GetDrawing()->GetPaintbrushData());    
    ldata->GetSpacing(spacing);
    const double voxelVolume = spacing[0] * spacing[1] * spacing[2];
    return voxelVolume * (double)(this->VolumesArray[
          this->GetDrawing()->GetItem(n)->GetLabel()]);
    }
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDrawingStatistics::GetVolume( vtkKWEPaintbrushSketch *s )
{
  return this->GetVolume(this->GetDrawing()->GetIndexOfItem(s));
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushDrawingStatistics
::GetLabelVolume( vtkKWEPaintbrushEnums::LabelType l )
{
  this->Update();
  double spacing[3];
  vtkKWEPaintbrushLabelData *ldata = vtkKWEPaintbrushLabelData::SafeDownCast(
      this->GetDrawing()->GetPaintbrushData());    
  ldata->GetSpacing(spacing);
  const double voxelVolume = spacing[0] * spacing[1] * spacing[2];
  return voxelVolume * (double)(this->VolumesArray[l]);
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

