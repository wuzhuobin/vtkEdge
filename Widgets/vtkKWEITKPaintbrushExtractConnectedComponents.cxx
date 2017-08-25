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
#include "vtkKWEITKPaintbrushExtractConnectedComponents.h"

#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkExecutive.h"
#include "vtkAlgorithmOutput.h"
#include "vtkImageData.h"
#include "vtkImageIterator.h"
#include "vtkImageThreshold.h"
#include "vtkSmartPointer.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushPropertyManager.h"

#include "itkConnectedComponentImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"

vtkCxxRevisionMacro(vtkKWEITKPaintbrushExtractConnectedComponents, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEITKPaintbrushExtractConnectedComponents);

//----------------------------------------------------------------------------
vtkKWEITKPaintbrushExtractConnectedComponents::
vtkKWEITKPaintbrushExtractConnectedComponents()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  this->SketchIndex = 0;
}

//----------------------------------------------------------------------------
vtkKWEITKPaintbrushExtractConnectedComponents::
~vtkKWEITKPaintbrushExtractConnectedComponents()
{
}

//----------------------------------------------------------------------------
void vtkKWEITKPaintbrushExtractConnectedComponents::
PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkKWEITKPaintbrushExtractConnectedComponents
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

  // Sanity check.
  if (this->SketchIndex >= inputDrawing->GetNumberOfItems())
    {
    vtkErrorMacro( << "sketch"
        << this->SketchIndex << " isnt present in the drawing.");
    return 0;
    }

  outputDrawing->SetRepresentation( inputDrawing->GetRepresentation() );
  outputDrawing->SetImageData( inputDrawing->GetImageData() );
  outputDrawing->SetPaintbrushOperation( inputDrawing->GetPaintbrushOperation() );
  outputDrawing->GetPaintbrushPropertyManager()->SetHighlightType(
      inputDrawing->GetPaintbrushPropertyManager()->GetHighlightType() );

  // This is the image on which we will run the connected components algorithm
  vtkSmartPointer< vtkImageData > inputImage
    = vtkSmartPointer< vtkImageData >::New();
  inputDrawing->GetItem(this->SketchIndex)->GetPaintbrushData()->
      GetPaintbrushDataAsImageData( inputImage );

  vtkSmartPointer< vtkImageData > inputBinaryImage
          = vtkSmartPointer< vtkImageData >::New();

  if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Grayscale)
    {
    // Threshold the image at 127.5 if it came from a grayscale representation.
    // We want to make sure that the connected components algorithm runs on a
    // binary image.
    vtkSmartPointer< vtkImageThreshold > threshold =
      vtkSmartPointer< vtkImageThreshold >::New();
    threshold->SetInput( inputImage );
    threshold->SetInValue(255.0);
    threshold->SetOutValue(0.0);
    threshold->ThresholdByUpper( 127.5 );
    threshold->Update();
    inputBinaryImage->ShallowCopy( threshold->GetOutput() );
    }
  else
    {
    inputBinaryImage->ShallowCopy( inputImage );
    }

  typedef itk::Image< unsigned char, 3 > ImageType;
  typedef itk::ConnectedComponentImageFilter< ImageType, ImageType > FilterType;

  FilterType::Pointer connectedComponentsFilter = FilterType::New();

  // Use face and edge connectivity. (26 connectivity in 3D).
  connectedComponentsFilter->FullyConnectedOn();

  // Connect the pipeline.
  typedef itk::VTKImageToImageFilter< ImageType >  VTK2ITKConverter;
  typedef itk::ImageToVTKImageFilter< ImageType >  ITK2VTKConverter;
  VTK2ITKConverter::Pointer inputConverter = VTK2ITKConverter::New();
  ITK2VTKConverter::Pointer outputConverter = ITK2VTKConverter::New();
  inputConverter->SetInput( inputBinaryImage );
  connectedComponentsFilter->SetInput( inputConverter->GetOutput() );

  // Run the connected components algorithm.
  try
    {
    inputConverter->Update();
    connectedComponentsFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return 0;
    }

  // Covert the segmented object label map to VTK land.
  outputConverter->SetInput( connectedComponentsFilter->GetOutput() );
  outputConverter->Update();

  int nSegments = static_cast< int >(connectedComponentsFilter->GetObjectCount());

  // Find the bounding box of each of the segments. We will do that so as to
  // save some memory allocating N sketches of the whole extent.
  int *sketchExtents = new int[6*nSegments];
  this->ComputeSegmentExtents( nSegments,
      sketchExtents, outputConverter->GetOutput() );

  // Now extract each connected component out into a vtkKWEPaintbrushSketch.
  for (int n = 1; n <= nSegments; n++)
    {

    int extent[6];
    inputDrawing->GetImageData()->GetExtent(extent);

    // Extract segmentN into an imageData.
    vtkSmartPointer< vtkImageData > segmentN
      = vtkSmartPointer< vtkImageData >::New();
    segmentN->SetExtent( sketchExtents + 6*(n-1) );
    segmentN->SetScalarTypeToUnsignedChar();
    segmentN->SetOrigin( inputDrawing->GetImageData()->GetOrigin() );
    segmentN->SetSpacing( inputDrawing->GetImageData()->GetSpacing() );
    segmentN->AllocateScalars();

    if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Grayscale)
      {

      // Extract object N from the output of the connected components algo
      ImageType::Pointer image = ImageType::New();
      image->SetRegions(connectedComponentsFilter->GetOutput()
          ->GetLargestPossibleRegion());
      image->Allocate();
      typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;
      typedef itk::ImageRegionIterator< ImageType > IteratorType;
      ConstIteratorType cit( connectedComponentsFilter->GetOutput(),
              connectedComponentsFilter->GetOutput()->GetBufferedRegion());
      IteratorType it( image, image->GetBufferedRegion());
      while (!it.IsAtEnd())
        {
        it.Set( (cit.Get() == n) ? 255 : 0 );
        ++cit;
        ++it;
        }


      // Dilate the object, so we can copy over the transition region as well.
      // We will dilate with a 5x5x5 structuring element.
      typedef itk::BinaryBallStructuringElement<
                      unsigned char, 3 > StructuringElementType;
      typedef itk::BinaryDilateImageFilter<
        ImageType, ImageType, StructuringElementType > DilateFilterType;
      DilateFilterType::Pointer dilate = DilateFilterType::New();
      dilate->SetInput( image );

      StructuringElementType  structuringElement;
      structuringElement.SetRadius( 2 );  // 5x5 structuring element
      structuringElement.CreateStructuringElement();
      dilate->SetKernel( structuringElement );
      dilate->SetDilateValue(255);
      dilate->Update();

      // Covert the dilated ITK image to VTK
      ITK2VTKConverter::Pointer dilatedConverter = ITK2VTKConverter::New();
      dilatedConverter->SetInput( dilate->GetOutput() );

      cout << "Start dilating object " << n << " of " << nSegments << endl;
      dilatedConverter->Update();
      cout << "Finished dilating" << endl;

      // Now do the copy of both the object and the transition region.
      // For the object, we need to look in the extracted segment, for the
      // transition region, we need to look in the dilation. Within these
      // regions we will copy the grayscale values from the original image.
      // Outside these we will set the values to 0.
      //
      // So we need 4 iterators, to iterate over
      //  * Extracted object
      //  * Dilation of above
      //  * Input drawing
      //  * Output drawing.
      //
      vtkImageIterator< unsigned char > it1(
          outputConverter->GetOutput(), segmentN->GetExtent());
      vtkImageIterator< unsigned char > it2( segmentN, segmentN->GetExtent() );
      vtkImageIterator< unsigned char > it3( inputImage, segmentN->GetExtent() );
      vtkImageIterator< unsigned char > it4(
          dilatedConverter->GetOutput(), segmentN->GetExtent() );

      while( !it2.IsAtEnd() )
        {
        unsigned char *inSI    = it1.BeginSpan();
        unsigned char *inSIEnd = it1.EndSpan();
        unsigned char *inSI2   = it2.BeginSpan();
        unsigned char *inSI3   = it3.BeginSpan();
        unsigned char *inSI4   = it3.BeginSpan();
        while (inSI != inSIEnd)
          {
          unsigned char inputDrawingVal = *inSI3;
          if (*inSI == (unsigned char)n)
            {
            *inSI2 = inputDrawingVal; // Within the object
            }
          else if (*inSI4 && inputDrawingVal <= 127.5)
            {
            *inSI2 = inputDrawingVal; // Within the transition region
            }
          else
            {
            *inSI2 = 0; // Outside.
            }

          ++inSI;
          ++inSI2;
          ++inSI3;
          ++inSI4;
          }

        it1.NextSpan();
        it2.NextSpan();
        it3.NextSpan();
        it4.NextSpan();
        }
      }
    else // Binary case
      {
      vtkImageIterator< unsigned char > it1(
          outputConverter->GetOutput(), segmentN->GetExtent());
      vtkImageIterator< unsigned char > it2( segmentN, segmentN->GetExtent() );

      while( !it2.IsAtEnd() )
        {
        unsigned char *inSI    = it1.BeginSpan();
        unsigned char *inSIEnd = it1.EndSpan();
        unsigned char *inSI2   = it2.BeginSpan();
        while (inSI != inSIEnd)
          {
          *inSI2 = ((*inSI == (unsigned char)n) ? 255 : 0);
          ++inSI;
          ++inSI2;
          }
        it1.NextSpan();
        it2.NextSpan();
        }
      }

    // Populate a sketch that encapsulates this segment and add it to the
    // drawing.
    vtkKWEPaintbrushSketch *sketchN = outputDrawing->AddItem();
    vtkKWEPaintbrushData * data;

    // Convert this vtkImageData that represents the segment into
    // a vtkKWEPaintbrushData.
    if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Binary)
      {
      vtkKWEPaintbrushStencilData *sdata = vtkKWEPaintbrushStencilData::New();
      vtkKWEPaintbrushUtilities::GetStencilFromImage<
        vtkKWEPaintbrushUtilities::vtkFunctorGreaterThan >(
          segmentN, sdata->GetImageStencilData(), 127.5 );
      data = sdata;
      }
    else if (inputDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Grayscale)
      {
      vtkKWEPaintbrushGrayscaleData *sdata = vtkKWEPaintbrushGrayscaleData::New();
      sdata->SetImageData( segmentN );
      data = sdata;
      }
    else
      {
      std::cerr << "ERROR: Unsupported paintbrush representation" << std::endl;
      return 0;
      }

    // Copy over some default properties from the input drawing.
    vtkKWEPaintbrushProperty *inputProperty =
      inputDrawing->GetItem(this->SketchIndex)->GetPaintbrushProperty();
    vtkKWEPaintbrushProperty *sketchNProperty = sketchN->GetPaintbrushProperty();
    sketchNProperty->SetHighlightColor( inputProperty->GetHighlightColor() );
    sketchNProperty->SetHighlightType( inputProperty->GetHighlightType() );
    sketchNProperty->SetOpacity( inputProperty->GetOpacity() );

    sketchN->SetPaintbrushData( data );
    data->Delete();
    }

  delete [] sketchExtents;
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEITKPaintbrushExtractConnectedComponents::
FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkKWEPaintbrushDrawing");
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEITKPaintbrushExtractConnectedComponents::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkKWEPaintbrushDrawing");
  return 1;
}

//----------------------------------------------------------------------------
// This method returns the largest region that can be generated.
void vtkKWEITKPaintbrushExtractConnectedComponents::RequestInformation (
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
int vtkKWEITKPaintbrushExtractConnectedComponents::
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
int vtkKWEITKPaintbrushExtractConnectedComponents::RequestDataObject(
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
void vtkKWEITKPaintbrushExtractConnectedComponents::
SetInput(vtkKWEPaintbrushDrawing* input)
{
  this->SetInputConnection(0,input->GetProducerPort());
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawing* vtkKWEITKPaintbrushExtractConnectedComponents::GetOutput()
{
  return vtkKWEPaintbrushDrawing::SafeDownCast(this->GetOutputDataObject(0));
}

//----------------------------------------------------------------------------
void vtkKWEITKPaintbrushExtractConnectedComponents
::ComputeSegmentExtents( int nSegments, int *e, vtkImageData *image )
{
  vtkImageIterator< unsigned char > it(image, image->GetExtent());
  for (int i = 0; i < nSegments; i++)
    {
    const int offset = i*6;
    e[offset]   = e[offset + 2] = e[offset + 4] = VTK_INT_MAX;
    e[offset+1] = e[offset + 3] = e[offset + 5] = VTK_INT_MIN;
    }

  int extent[6];
  image->GetExtent(extent);
  int ijk[3] = { extent[0], extent[2], extent[4] };

  while( !it.IsAtEnd() )
    {
    unsigned char *inSI    = it.BeginSpan();
    unsigned char *inSIEnd = it.EndSpan();
    while (inSI != inSIEnd)
      {
      for (int i = 1; i <= nSegments; i++)
        {
        if (*inSI == i)
          {
          const int offset = (i-1)*6;
          for (int j = 0; j < 3; j++)
            {
            if (e[offset + 2*j]    > ijk[j]) { e[offset + 2*j]   = ijk[j]; }
            if (e[offset + 2*j +1] < ijk[j]) { e[offset + 2*j+1] = ijk[j]; }
            }
          }
        }

      ++inSI;
      ++ijk[0];

      // Find out our structured coordinate in the image.
      }
    it.NextSpan();

    if (ijk[0] > extent[1])
      {
      ijk[0] = extent[0];
      ++ijk[1];
      if (ijk[1] > extent[3])
        {
        ijk[1] = extent[2];
        ++ijk[2];
        }
      }
    }

  for (int i = 0; i < nSegments; i++)
    {
    const int offset = i*6;
    cout << e[offset] <<" " <<
      e[offset + 1] << " " << e[offset + 2] << " " << e[offset + 3] << " " << e[offset + 4] << " " << e[offset + 5] << endl;
    }

}

