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

#include "vtkKWEImageFDKFilter.h"

#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkFastNumericConversion.h"
#include "vtkImageData.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageFFT.h"
#include "vtkImageFourierFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageRFFT.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"

#include <math.h>
#include <fstream>
#include <limits>


vtkCxxRevisionMacro(vtkKWEImageFDKFilter, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEImageFDKFilter);

//----------------------------------------------------------------------------
vtkKWEImageFDKFilter::vtkKWEImageFDKFilter()
{
  this->WeightFunction = vtkImageData::New();
  this->Radius = 1.0;
  this->Angles = vtkDoubleArray::New();
  this->Angles->SetNumberOfComponents(1);
}

//----------------------------------------------------------------------------
vtkKWEImageFDKFilter::~vtkKWEImageFDKFilter()
{
  this->WeightFunction->Delete();
  this->Angles->Delete();
}

//----------------------------------------------------------------------------
void vtkKWEImageFDKFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

int vtkKWEImageFDKFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector)
{
  // Get the information object.
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // Get the data objects.
  vtkImageData * inData = vtkImageData::SafeDownCast(
                                     inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData * outData = vtkImageData::SafeDownCast(
                                    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  //void *inPtr1;
  //void *inPtr2;
  float *outPtr;
  //int *tExt;

  int inExt[6], outExt[6] = {0, 128, 0, 128, 0 , 128};

  inData->GetExtent(inExt);
  outData->GetExtent(outExt);

  vtkImageData * inImage = inData;

  //inPtr1 = inData->GetScalarPointerForExtent(outExt);


  // this filter expects that input is the same type as output.
  if (inData->GetScalarType() != outData->GetScalarType())
    {
    vtkErrorMacro("Execute: input ScalarType, "
                  << inData->GetScalarType()
                  << ", must match out ScalarType "
                  << outData->GetScalarType());
    return 0;
    }



  // make the temporary regions to iterate over.
 /* vtkImageData *in = vtkImageData::New();
  in->SetExtent(inExt);
  in->SetNumberOfScalarComponents(inData[0][0]->GetNumberOfScalarComponents());
  in->SetScalarType(VTK_DOUBLE);
  in->CopyAndCastFrom(inData[0][0],inExt);*/

  vtkFastNumericConversion * numeric = vtkFastNumericConversion::New();

  // N = 2*floor( nc_p /(2*sqrt(2)) ); % take the default value
  int N = static_cast<int>(2 * floor( (187)/ (2*sqrt(2.0))));
  int N2 = static_cast<int>(ceil(N/2.0));
  int volExt[6] = { 0, N, 0, N, 0, N };

  vtkImageData *out = vtkImageData::New();
  out->SetExtent(volExt);
  out->SetNumberOfScalarComponents(
    inData->GetNumberOfScalarComponents());
  out->SetScalarType(VTK_FLOAT);
  out->AllocateScalars();

  outData->SetExtent(volExt);
  outData->SetNumberOfScalarComponents(
    inData->GetNumberOfScalarComponents());
  outData->SetScalarType(VTK_FLOAT);
  outData->AllocateScalars();

  outPtr = static_cast<float *>(outData->GetScalarPointer());

  for(int i = 0; i < N * N * N; i++)
    {
    *outPtr++ = 0.0;
    }

  float theta, x, y, z, s, t, ctrX, ctrY;
  int dimX, dimY, dimZ, u, u1, v, v1, outX, outY, outZ;
  dimX = inExt[1]-inExt[0] + 1;
  dimY = inExt[3]-inExt[2] + 1;
  dimZ = inExt[5]-inExt[4] + 1;
  outX = volExt[1]-volExt[0] + 1;
  outY = volExt[3]-volExt[2] + 1;
  outZ = volExt[5]-volExt[4] + 1;

  ctrX = static_cast<float>(ceil(dimX/2.0));
  ctrY = static_cast<float>(ceil(dimY/2.0));

  // Generate Weighting Function for the projections
  this->GenerateWeightingFunction(inExt);

  // Generate a vector filter

  int order = 1;

  while(order < dimX)
    {
    order <<= 1;
    }

  if(order < 64)
    {
    order = 64;
    }

  /*float ** H = NULL;
  H = new float*[6];
  for(int i = 0; i < 6; i++)
    {
    H[i] = new float[order];
    }*/

  float * H = new float[order];
  this->GenerateFilter(H,order);

  vtkImageMathematics * math = vtkImageMathematics::New();
  math->SetOperationToMultiply();

  vtkImageData * tempSlice = vtkImageData::New();
  tempSlice->SetNumberOfScalarComponents(1);
  tempSlice->SetScalarTypeToFloat();
  tempSlice->SetDimensions(order, 6, 1);
  tempSlice->Update();

  vtkImageFFT * fft = vtkImageFFT::New();
  vtkImageRFFT * ifft = vtkImageRFFT::New();
  //vtkStreamingDemandDrivenPipeline * fftPipe =
  //  vtkStreamingDemandDrivenPipeline::SafeDownCast(fft->GetExecutive());

  //vtkStreamingDemandDrivenPipeline * ifftPipe =
  //  vtkStreamingDemandDrivenPipeline::SafeDownCast(ifft->GetExecutive());

  vtkImageData * fftOutput = NULL;

  int fftExt[6] = {0, order, 0, dimY - 1, 0, dimZ - 1};
  //int ifftExt[6] = {0, order, 0, dimY - 1, 0, dimZ - 1};
  int projIdx = 0;
  float projValue = 0.0;
  float cosTheta = 0.0;
  float sinTheta = 0.0;
  float Radius2 = 0.0;

  float * fftPtr = NULL;
  float * ifftPtr = NULL;
  //float * volPtr = NULL;

  vtkTimerLog * timer = vtkTimerLog::New();

  timer->StartTimer();

  // Main execution loop
  for(int i = 0; i < 360; i++)
    {
    // for each row of tempRowS
    // Mutiply Projection by Weight
    this->ApplyWeightingFunction(64, i, inImage, tempSlice);

    fft->SetInput(tempSlice);

    fft->Update();
    fftOutput = fft->GetOutput();
    fftPtr = static_cast<float *>(fft->GetOutput()->GetScalarPointer());
    fftOutput->GetExtent(fftExt);

    // Filter each Projection row with H
    for(int j = 0; j < 6; j++)
      {
      // Multiply by Filter H.
      for(int k = 0; k < order; k++)
        {
        // We must multiply both real and imaginary parts.
        //*fftPtr++ *= H[k];
        //*fftPtr++ *= H[k];
        //fftPtr++;
        //fftPtr++;
        }
      }
    ifft->SetInput(fftOutput);
    // Inverse transform back to spatial domain.
    ifft->Update();

    theta = static_cast<float>(this->Angles->GetTuple1(i));
    ifftPtr = static_cast<float *>(ifft->GetOutput()->GetScalarPointer());

    vtkImageExtractComponents * real = vtkImageExtractComponents::New();
    real->SetInput(ifft->GetOutput());
    real->SetComponents(0);
    real->Update();

    // reset pointer
    outPtr = static_cast<float *>(outData->GetScalarPointer());
    cosTheta = cosf(theta);
    sinTheta = sinf(theta);

    for(x = static_cast<float>(-N2 + 1); x < N2; x++)
      {
      for(y = static_cast<float>(-N2 + 1); y < N2; y++)
        {
        s = x * cosTheta + y * sinTheta;
        t = -x * sinTheta + y * cosTheta;
        Radius2 = this->Radius - s;
        //u is the 1st projection pixel coordinate
        u = numeric->Round(this->Radius * t/Radius2 + ctrX);
        //fileOut << u << endl;
        if(u <= 0 ||u >= (dimX+1))
          {
          // this u is outside the projection, skip
          //cout << "u: " << u << endl;
          //outPtr++;
          continue;
          }

        for(z = static_cast<float>(-N2); z < 1; z++)
          {
          //v = int(this->Radius/Radius2 * z) + ctrY;
          v = numeric->Round(this->Radius/Radius2 * z + 3);
          /*
          v1=round( v0+nr_p/2 ) ;  % coord. correction
          idx = id_1_term + v1 ; % idx = sub2ind(szproj,u1, v1);
          tmp = proj(idx); % just in case
          tmp = tmp(:).*inv_R_s_quad; % back-projection ... from pixels to voxels ...!
          tmp1(good_u)=tmp; tmp1(lengoodu)=0;%
          tmp2(maskV)=tmp1; % prepare for image reconst. within the mask
          iposz=iposz_v(iz); % position of the current slice iz in the volume at iposz
          vol_tmp(:,iposz)=vol_tmp(:,iposz)+tmp2; % sum up back-projections without reshaping
            */
          projIdx = (u - 1) * 6 + v;
          u1 = projIdx / order;
          v1 = projIdx  - (u1 * order);
          u1 = 0;
          v1 = 0;
          //if(v > 0 && v < dimY)
          //if(y + N2 - 1 < dimY)
            {
            //cout << "x: " << x << "y: " << y << "v: " << v << endl;
            //projValue =  *static_cast<float *>(ifft->GetOutput()->GetScalarPointer(u, v, 0));
            //projValue =  *static_cast<float *>(tempSlice->GetScalarPointer(u, v + ctrY, 0));
            projValue =  *static_cast<float *>(tempSlice->GetScalarPointer(u1,v1,0));
            //projValue =  *static_cast<float *>(inImage->GetScalarPointer(x + N2 - 1, y + N2 - 1, 0));
            //if(projValue > std::numeric_limits<float>::min() && projValue < std::numeric_limits<float>::max())
              {
              *outPtr++ += projValue /(Radius2 * Radius2);
              //cout << projValue << "  ";
              }
            }
          }
        }
      }

    //fileOut.close();
    cout << "Projection " << i << " complete." << endl;
    }
    timer->StopTimer();
    cout << "Execution took " << timer->GetElapsedTime() << " seconds." << endl;

  numeric->Delete();
  out->Delete();
  math->Delete();
  ifft->Delete();
  fft->Delete();
  tempSlice->Delete();
  timer->Delete();
  delete [] H;

  return 1;
}

//----------------------------------------------------------------------------
// The output extent is the intersection.
int vtkKWEImageFDKFilter::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  int extent[6];
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
  //this->ComputeOutputWholeExtent(extent, this->HandleBoundaries);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

  return 1;
}

void vtkKWEImageFDKFilter::GenerateWeightingFunction(int projExt[6])
{
  int dimX, dimY;
  dimX = projExt[1]-projExt[0] + 1;
  dimY = projExt[3]-projExt[2] + 1;
  this->WeightFunction->SetNumberOfScalarComponents(1);
  this->WeightFunction->SetScalarTypeToDouble();
  this->WeightFunction->SetDimensions(dimX, dimY, 1);
  this->WeightFunction->Update();

  /*vtkImageData *in = vtkImageData::New();
  in->SetExtent(inExt);
  in->SetNumberOfScalarComponents(inData[0][0]->GetNumberOfScalarComponents());
  in->SetScalarType(VTK_DOUBLE);
  in->CopyAndCastFrom(inData[0][0],inExt);*/

  float * weightPtr = static_cast<float *>(this->WeightFunction->GetScalarPointer());

  // Generate a Weighting function.
  // The projection weights the pixels about the origin of the projection
  // which is the center of the projection.

  //[X,Y]=meshgrid([[floor(8/2):-1:0],[1:(8-floor(8/2)-1)]],...
//[[floor(8/2):-1:0],[1:(8-floor(8/2)-1)]]);
  //W=R.* ( (X.^2 + Y.^2 + R^2).^-0.5 );
  int dim2X, dim2Y, p, zeta;
  dim2X = static_cast<int>(floor(dimX/2.0));
  dim2Y = static_cast<int>(floor(dimY/2.0));
  double d = this->Radius;
  double d2 = d * d;
  double numerator = 0.0;

  for(zeta = -dim2Y; zeta < dim2Y; zeta++)
    {
    for(p = -dim2X; p < dim2X; p++)
      {
      numerator = d2 + pow(static_cast<double>(zeta), 2.0) + pow(static_cast<double>(p), 2.0);
      *weightPtr = static_cast<float>(d * pow(numerator,-0.5));
      weightPtr++;
      }
    }
}

void vtkKWEImageFDKFilter::ApplyWeightingFunction(
  int row,
  int slice,
  vtkImageData * projection,
  vtkImageData * output)
{
  int inExt[6], outExt[6];
  projection->GetExtent(inExt);
  output->GetExtent(outExt);


  int dimY, x , y, padX0, padX1;
  dimY = outExt[3] + 1;
  padX0 = static_cast<int>(floor((outExt[1] - (inExt[1] + 1))/2.0));
  padX1 = outExt[1] - padX0;

  float * weightPtr = NULL;
  float * projPtr = NULL;
  //const char * type = projection->GetScalarTypeAsString();
  float * outPtr = static_cast<float *>(output->GetScalarPointer());

  // Apply weight and also insert padding for FFT
  for(y = 0; y < dimY; y++)
    {
    weightPtr = static_cast<float *>(this->WeightFunction->GetScalarPointer(0, row, 0));
    projPtr = static_cast<float *>(projection->GetScalarPointer(0, row, slice));
    for(x = 0; x < padX0; x++)
      {
      *outPtr++ = 0.0;
      }

    for(x = padX0; x < padX1; x++)
      {
      //*outPtr++ = *weightPtr++ * *projPtr++;
      *outPtr++ = *projPtr++;
      }

    for(x = padX1; x < outExt[1] + 1; x++)
      {
      *outPtr++ = 0.0;
      }
    }
}

void vtkKWEImageFDKFilter::GenerateFilter(float * filter, int order)
{
  //filt = 2*( 0:(order/2) )./order;
  //w = 2*pi*(0:size(filt,2)-1)/order;   % frequency axis up to Nyquist
  //filt(w>pi*d) = 0;                      % Crop the frequency response
  //filt = [filt , filt(end-1:-1:2)];    % Symmetry of the filter
  int i = 0, ii = 0;
  int halfOrder = order/2;

  for(i = 0; i < halfOrder; i++)
    {
    filter[i] = static_cast<float>(2 * i/order);
    }

  float * w = new float[halfOrder];
  float PI = vtkMath::Pi();

  // ramp up the frequency to Nyquist
  for(i = 0; i < halfOrder; i++)
    {
    w[i] = 2 * PI * static_cast<float>(i/order);
    }

  // crop the frequency response
  for(i = 0; i < halfOrder; i++)
    {
    /*if(w[i] > PI * d)
      {
      filter[i] = 0;
      }*/
    }

  // Filter is symetric, mirror it to fill the rest of the array
  for(i = halfOrder, ii = halfOrder - 1; i < order; i++, ii--)
    {
    filter[i] = filter[ii];
    }
  delete [] w;
}
