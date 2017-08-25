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
#include "vtkKWEPaintbrushMergeSketches.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkExecutive.h"
#include "vtkAlgorithmOutput.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushPropertyManager.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushMergeSketches, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushMergeSketches);

//----------------------------------------------------------------------------
vtkKWEPaintbrushMergeSketches::vtkKWEPaintbrushMergeSketches()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushMergeSketches::~vtkKWEPaintbrushMergeSketches()
{
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushMergeSketches::
PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushMergeSketches
::RequestData( vtkInformation *,
               vtkInformationVector** inputVector,
               vtkInformationVector* outputVector)
{
  vtkInformation *info = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkKWEPaintbrushDrawing *inputDrawing = vtkKWEPaintbrushDrawing::SafeDownCast(
                             info->Get(vtkDataObject::DATA_OBJECT()));
  vtkKWEPaintbrushDrawing *outputDrawing = vtkKWEPaintbrushDrawing::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));

  outputDrawing->SetRepresentation( inputDrawing->GetRepresentation() );
  outputDrawing->SetImageData( inputDrawing->GetImageData() );
  outputDrawing->SetPaintbrushOperation( inputDrawing->GetPaintbrushOperation() );
  outputDrawing->InitializeData();
  outputDrawing->RemoveAllItems();
  outputDrawing->GetPaintbrushPropertyManager()->SetHighlightType(
      inputDrawing->GetPaintbrushPropertyManager()->GetHighlightType() );

  // Sanity check.
  if (inputDrawing->GetNumberOfItems() == 0)
    {
    // No sketches.. just return.
    return 1;
    }

  if (inputDrawing->GetNumberOfItems() == 1)
    {
    // Just 1 sketch. Nothing to merge with.
    outputDrawing->AddItem( inputDrawing->GetItem(0) );
    return 1;
    }

  // The output data
  vtkKWEPaintbrushData * data = NULL;

  if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Binary)
    {
    vtkKWEPaintbrushStencilData *sdata = vtkKWEPaintbrushStencilData::New();
    data = sdata;
    }
  else if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Grayscale)
    {
    vtkKWEPaintbrushGrayscaleData *sdata = vtkKWEPaintbrushGrayscaleData::New();
    data = sdata;
    }

  data->SetExtent( inputDrawing->GetImageData()->GetExtent() );
  data->SetSpacing( inputDrawing->GetImageData()->GetSpacing() );
  data->SetOrigin( inputDrawing->GetImageData()->GetOrigin() );
  data->Allocate();

  for (int n = 0; n < inputDrawing->GetNumberOfItems(); n++)
    {
    data->Add( inputDrawing->GetItem(n)->GetPaintbrushData() );
    }

  vtkKWEPaintbrushSketch *sketch = outputDrawing->AddItem();
  sketch->AddNewStroke( vtkKWEPaintbrushEnums::Draw, data );
  data->Delete();

  // Copy over some default properties from the input drawing.
  vtkKWEPaintbrushProperty *inputProperty =
    inputDrawing->GetItem(0)->GetPaintbrushProperty();
  vtkKWEPaintbrushProperty *outputProperty = sketch->GetPaintbrushProperty();
  outputProperty->SetHighlightColor( inputProperty->GetHighlightColor() );
  outputProperty->SetHighlightType( inputProperty->GetHighlightType() );
  outputProperty->SetOpacity( inputProperty->GetOpacity() );

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushMergeSketches::
FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkKWEPaintbrushDrawing");
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushMergeSketches::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkKWEPaintbrushDrawing");
  return 1;
}

//----------------------------------------------------------------------------
// This method returns the largest region that can be generated.
void vtkKWEPaintbrushMergeSketches::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *info = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkKWEPaintbrushDrawing *inputDrawing = vtkKWEPaintbrushDrawing::SafeDownCast(
                             info->Get(vtkDataObject::DATA_OBJECT()));

  // set the extent
  int wExtent[6];
  inputDrawing->GetImageData()->GetExtent( wExtent );
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wExtent, 6);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushMergeSketches::
ProcessRequest(vtkInformation* request,
               vtkInformationVector** inputVector,
               vtkInformationVector* outputVector)
{
  // create the output
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
    {
    return this->RequestDataObject(request, inputVector, outputVector);
    }

  // generate the data
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
    {
    this->RequestData(request, inputVector, outputVector);
    return 1;
    }

  // execute information
  if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
    {
    this->RequestInformation(request, inputVector, outputVector);
    return 1;
    }

  return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushMergeSketches::RequestDataObject(
  vtkInformation*,
  vtkInformationVector** inputVector ,
  vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  if (!inInfo)
    {
    return 0;
    }
  vtkDataObject *input = inInfo->Get(vtkDataObject::DATA_OBJECT());

  if (input)
    {
    // for each output
    for(int i=0; i < this->GetNumberOfOutputPorts(); ++i)
      {
      vtkInformation* info = outputVector->GetInformationObject(i);
      vtkDataObject *output = info->Get(vtkDataObject::DATA_OBJECT());

      if (!output || !output->IsA(input->GetClassName()))
        {
        vtkDataObject* newOutput = input->NewInstance();
        newOutput->SetPipelineInformation(info);
        newOutput->Delete();
        this->GetOutputPortInformation(0)->Set(
          vtkDataObject::DATA_EXTENT_TYPE(), newOutput->GetExtentType());
        }
      }
    return 1;
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushMergeSketches::
SetInput(vtkKWEPaintbrushDrawing* input)
{
  this->SetInputConnection(0,input->GetProducerPort());
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawing* vtkKWEPaintbrushMergeSketches::GetOutput()
{
  return vtkKWEPaintbrushDrawing::SafeDownCast(this->GetOutputDataObject(0));
}


