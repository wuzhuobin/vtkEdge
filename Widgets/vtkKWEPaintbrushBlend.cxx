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
#include "vtkKWEPaintbrushBlend.h"

#include "vtkAlgorithmOutput.h"
#include "vtkImageData.h"
#include "vtkImageStencilData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkCollection.h"
#include "vtkProperty.h"
#include "vtkImageIterator.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushPropertyManager.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushEnums.h"

typedef unsigned char VTKColorType [3];

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEPaintbrushBlend, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushBlend);

//----------------------------------------------------------------------------
vtkKWEPaintbrushBlend::vtkKWEPaintbrushBlend()
{
  this->SetNumberOfInputPorts(2);
  this->Opacity = 0.5;
  this->UseOverlay=1;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushBlend::~vtkKWEPaintbrushBlend()
{
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawing *vtkKWEPaintbrushBlend::GetPaintbrushDrawing()
{
  if (this->GetNumberOfInputConnections(1) < 1)
    {
    return NULL;
    }
  return vtkKWEPaintbrushDrawing::SafeDownCast(
      this->GetExecutive()->GetInputData(1,0));
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushBlend::SetPaintbrushDrawing(
                          vtkKWEPaintbrushDrawing *c)
{
  // if stencil is null, then set the input port to null
  this->SetInput(1, c);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushBlend::RequestUpdateExtent(
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo0 = inputVector[0]->GetInformationObject(0);
  vtkInformation *inInfo1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // default input extents will be that of output extent
  int inExt[6];
  outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),inExt);
  inInfo0->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),inExt,6);
  inInfo1->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),inExt,6);

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushBlend::RequestData(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

  vtkImageData *input = vtkImageData::SafeDownCast(
          inInfo->Get(vtkDataObject::DATA_OBJECT()));
  if (!input || ! this->GetPaintbrushDrawing())
    {
    return 1;
    }

  vtkInformation *info = inputVector[1]->GetInformationObject(0);
  vtkKWEPaintbrushDrawing *drawing = vtkKWEPaintbrushDrawing::SafeDownCast(
                     info->Get(vtkDataObject::DATA_OBJECT()));

  // Attempt to assign a (possibly unique) color for each sketch in the drawing
  drawing->GetPaintbrushPropertyManager()->Update();

  if (drawing->GetRepresentation() == vtkKWEPaintbrushEnums::Label)
    {
    // Preprocess and store all the labels.
    }

  return this->Superclass::RequestData(request,inputVector,outputVector);
}

//----------------------------------------------------------------------------
// This templated function blends a stencil that represents a sketch from a
// drawing into the overlaid image. This method is specific to the case when
// the overlaid image has data type of CHAR
template <class T>
void vtkKWEPaintbrushBlendExecuteChar( vtkImageStencilData * stencil,
                                    vtkKWEPaintbrushBlend *vtkNotUsed(self),
                                    int extent[6],
                                    vtkImageData *inData, T *,
                                    vtkImageData *outData, T *,
                                    double opacity, unsigned char color[3])
{
  int inC, outC;
  unsigned short r, f, o;
  int v0, v1, v2;
  int stencilExtent[6];

  // round opacity to a value in the range [0,256], because division
  // by 256 can be efficiently achieved by bit-shifting by 8 bits
  o = static_cast<unsigned short>(256*opacity + 0.5);
  r = o;
  f = static_cast<unsigned short>(256 - o);

  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the stencil extents
  stencil->GetExtent(stencilExtent);

  stencilExtent[0] = (stencilExtent[0] < extent[0]) ? extent[0] : stencilExtent[0];
  stencilExtent[1] = (stencilExtent[1] > extent[1]) ? extent[1] : stencilExtent[1];
  stencilExtent[2] = (stencilExtent[2] < extent[2]) ? extent[2] : stencilExtent[2];
  stencilExtent[3] = (stencilExtent[3] > extent[3]) ? extent[3] : stencilExtent[3];
  stencilExtent[4] = (stencilExtent[4] < extent[4]) ? extent[4] : stencilExtent[4];
  stencilExtent[5] = (stencilExtent[5] > extent[5]) ? extent[5] : stencilExtent[5];

  // Loop through ouput pixels
  for (int idxZ = stencilExtent[4]; idxZ <= stencilExtent[5]; idxZ++)
    {
    for (int idxY = stencilExtent[2]; idxY <= stencilExtent[3]; idxY++)
      {
      int iter = 0, moreSubExtents = 1, r1, r2;
      while (moreSubExtents)
        {
        moreSubExtents = stencil->GetNextExtent( r1, r2,
            stencilExtent[0], stencilExtent[1], idxY, idxZ, iter );

        r1 = (r1 < stencilExtent[0]) ? stencilExtent[0] : r1;
        r2 = (r2 > stencilExtent[1]) ? stencilExtent[1] : r2;

        if (r1 <= r2 )  // sanity check
          {
          T *beginInPtr  =
            static_cast<T *>(inData->GetScalarPointer(r1, idxY, idxZ));
          T *beginOutPtr =
            static_cast<T *>(outData->GetScalarPointer(r1, idxY, idxZ));
          T *endOutPtr   =
            static_cast<T *>(outData->GetScalarPointer(r2, idxY, idxZ));

          while (beginOutPtr <= endOutPtr)
            {

            if (outC >= 3 && inC >= 4)
              {
              // RGB(A) blended with RGBA

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of beginInPtr[3] * range of o
              r = static_cast<unsigned short>(beginInPtr[3]*o);
              f = static_cast<unsigned short>(65280 - r);

              v0 = beginInPtr[0]*f + color[0]*r;
              v1 = beginInPtr[1]*f + color[1]*r;
              v2 = beginInPtr[2]*f + color[2]*r;


              // do some math tricks to achieve division by 65280:
              // this is not an approximation, it gives exactly the
              // same result as an integer division by 65280
              beginOutPtr[0] =
                static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
              beginOutPtr[1] =
                static_cast<T>((v1 + (v1 >> 8) + (v1 >> 16) + 1) >> 16);
              beginOutPtr[2] =
                static_cast<T>((v2 + (v2 >> 8) + (v2 >> 16) + 1) >> 16);
              }

            else if (outC >= 3 && inC == 3)
              {
              // RGB(A) blended with RGB

              // the bit-shift achieves a division by 256
              beginOutPtr[0] =
                static_cast<T>((beginInPtr[0]*f + color[0]*r) >> 8);
              beginOutPtr[1] =
                static_cast<T>((beginInPtr[1]*f + color[1]*r) >> 8);
              beginOutPtr[2] =
                static_cast<T>((beginInPtr[2]*f + color[2]*r) >> 8);
              }

            else if (inC == 2)
              {
              // luminance(+alpha) blended with luminance+alpha

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of beginInPtr[1] * range of o
              r = static_cast<unsigned short>(beginInPtr[1]*o);
              f = static_cast<unsigned short>(65280 - r);
              v0 = beginOutPtr[0]*f + beginInPtr[0]*r;
              // do some math tricks to achieve division by 65280:
              // this is not an approximation, it gives exactly the
              // same result as an integer division by 65280
              beginOutPtr[0] =
                static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
              }

            else
              {
              // luminance(+alpha) blended with luminance

              // the bit-shift achieves a division by 256
              beginOutPtr[0] = static_cast<T>((beginInPtr[0]*f) >> 8);
              }

            beginInPtr += inC;
            beginOutPtr += outC;
            }
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
// This templated function blends an image that represents a label map into
// the overlay image. This function is specific to the case when the overlay
// image has data type of CHAR
template <class T>
void vtkKWEPaintbrushBlendExecuteChar(
     vtkKWEPaintbrushLabelData *labelMap,
     vtkKWEPaintbrushBlend *self,
     int extent[6],
     vtkImageData *inData, T *,
     vtkImageData *outData, T *,
     vtkKWEPaintbrushPropertyManager::LabelToColorMapType &labelToColorMap)
{
  // Is an overlay present ? If absent, the label map, passed through the
  // color lookup table is displayed as is. If not, it is blended with
  // the overlay and displayed.
  bool useOverlay = self->GetUseOverlay() ? true : false;

  int inC, outC;
  unsigned short r = 0, f = 0, o = 0;
  int v0, v1, v2;
  int labelMapExtent[6];


  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the label map's extents
  labelMap->GetExtent(labelMapExtent);

  // Find the intersection of the labelmap's extents and the overlaid image's
  // extents. This is the region we need to be blending.
  labelMapExtent[0] = (labelMapExtent[0] < extent[0]) ? extent[0] : labelMapExtent[0];
  labelMapExtent[1] = (labelMapExtent[1] > extent[1]) ? extent[1] : labelMapExtent[1];
  labelMapExtent[2] = (labelMapExtent[2] < extent[2]) ? extent[2] : labelMapExtent[2];
  labelMapExtent[3] = (labelMapExtent[3] > extent[3]) ? extent[3] : labelMapExtent[3];
  labelMapExtent[4] = (labelMapExtent[4] < extent[4]) ? extent[4] : labelMapExtent[4];
  labelMapExtent[5] = (labelMapExtent[5] > extent[5]) ? extent[5] : labelMapExtent[5];

  vtkImageData *labelMapData = labelMap->GetLabelMap();

  // Lookup table of label to color map.
  unsigned char color[3];

  // To avoid gcc 4.4 warning "color[x] may be used uninitialized in this
  // function [-Wuninitialized]"
  color[0]=0;
  color[1]=0;
  color[2]=0;

  vtkKWEPaintbrushPropertyManager::vtkKWEPaintbrushLabelMapColor colorClass;

  // Loop through pixels
  T *beginInPtr = NULL, *beginOutPtr;
  vtkKWEPaintbrushEnums::LabelType * labelPtr;

  // Type of compositing..
  int compositeType = 3; // luminance(+alpha) blended with luminance
  if (outC >= 3 && inC >= 4)
    {
    // RGB(A) blended with RGBA
    compositeType = 0;
    }
  else if (outC >= 3 && inC == 3)
    {
    // RGB(A) blended with RGB
    compositeType = 1;
    }
  else if (inC == 2)
    {
    // luminance(+alpha) blended with luminance+alpha
    compositeType = 2;
    }


  vtkKWEPaintbrushEnums::LabelType lastLabel =
                                 vtkKWEPaintbrushLabelData::NoLabelValue;
  // Loop through ouput pixels
  for (int idxZ = labelMapExtent[4]; idxZ <= labelMapExtent[5]; idxZ++)
    {
    for (int idxY = labelMapExtent[2]; idxY <= labelMapExtent[3]; idxY++)
      {

      if (useOverlay)
        {
        beginInPtr  = static_cast<T *>(inData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));
        }
      beginOutPtr = static_cast<T *>(outData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));
      labelPtr = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
          labelMapData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));

      for (int idxX = labelMapExtent[0]; idxX <= labelMapExtent[1]; idxX++)
        {

        const vtkKWEPaintbrushEnums::LabelType label = *labelPtr;

        if (useOverlay)
          {
          if (label != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            if( label != lastLabel )
              {
              colorClass = labelToColorMap[label];
              lastLabel = label;
              color[0] = colorClass.Color[0];
              color[1] = colorClass.Color[1];
              color[2] = colorClass.Color[2];

              // round opacity to a value in the range [0,256], because division
              // by 256 can be efficiently achieved by bit-shifting by 8 bits
              o = static_cast<unsigned short>(256*colorClass.Opacity + 0.5);
              r = o;
              f = static_cast<unsigned short>(256 - o);
              }

            if (compositeType == 0)
              {
              // RGB(A) blended with RGBA

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of inSI[3] * range of o
              r = static_cast<unsigned short>(beginInPtr[3]*o);
              f = static_cast<unsigned short>(65280 - r);
              v0 = beginInPtr[0]*f + color[0]*r;
              v1 = beginInPtr[1]*f + color[1]*r;
              v2 = beginInPtr[2]*f + color[2]*r;

              // do some math tricks to achieve division by 65280:
              // this is not an approximation, it gives exactly the
              // same result as an integer division by 65280
              beginOutPtr[0] =
                static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
              beginOutPtr[1] =
                static_cast<T>((v1 + (v1 >> 8) + (v1 >> 16) + 1) >> 16);
              beginOutPtr[2] =
                static_cast<T>((v2 + (v2 >> 8) + (v2 >> 16) + 1) >> 16);
              }

            else if (compositeType == 1)
              {
              // the bit-shift achieves a division by 256
              beginOutPtr[0] =
                static_cast<T>((beginInPtr[0]*f + color[0]*r) >> 8);
              beginOutPtr[1] =
                static_cast<T>((beginInPtr[1]*f + color[1]*r) >> 8);
              beginOutPtr[2] =
                static_cast<T>((beginInPtr[2]*f + color[2]*r) >> 8);
              }

            else if (compositeType == 2)
              {
              // luminance(+alpha) blended with luminance+alpha

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of beginInPtr[1] * range of o
              r = static_cast<unsigned short>(beginInPtr[1]*o);
              f = static_cast<unsigned short>(65280 - r);
              v0 = beginOutPtr[0]*f + beginInPtr[0]*r;

              // do some math tricks to achieve division by 65280:
              // this is not an approximation, it gives exactly the
              // same result as an integer division by 65280
              beginOutPtr[0] =
                static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
              }

            else
              {
              // luminance(+alpha) blended with luminance

              // the bit-shift achieves a division by 256
              beginOutPtr[0] = static_cast<T>((beginInPtr[0]*f) >> 8);
              }
            }

          beginInPtr += inC;
          beginOutPtr += outC;
          ++labelPtr;
          }
        else // No overlays.
          {
          if( label != lastLabel )
            {
            colorClass = labelToColorMap[label];
            lastLabel = label;
            }

          if (compositeType == 0)
            {
            beginOutPtr[0] = colorClass.Color[0];
            beginOutPtr[1] = colorClass.Color[1];
            beginOutPtr[2] = colorClass.Color[2];
            if (outC > 3)
              {
              beginOutPtr[3] = 255;
              }
            }
          else if (compositeType == 1)
            {
            beginOutPtr[0] = colorClass.Color[0];
            beginOutPtr[1] = colorClass.Color[1];
            beginOutPtr[2] = colorClass.Color[2];
            }
          else if (compositeType == 2)
            {
            beginOutPtr[0] = colorClass.Color[0];
            }
          else
            {
            // luminance(+alpha) blended with luminance
            // the bit-shift achieves a division by 256
            beginOutPtr[0] = colorClass.Color[0];
            }

          beginOutPtr += outC;
          ++labelPtr;
          }
        }
      }
    }

}

//----------------------------------------------------------------------------
// This templated function blends an image that represents a label map into
// the overlay image. This function is specific to the case when the overlay
// (canvas) image as well as the label map have data type of CHAR
template <class T>
void vtkKWEPaintbrushBlendExecuteCharLabelMapTypeUC(
                        vtkKWEPaintbrushLabelData * labelMap,
                        vtkKWEPaintbrushBlend *self,
                        int extent[6],
                        vtkImageData *inData, T *,
                        vtkImageData *outData, T *,
                        VTKColorType *labelToColorMapUC,
                        double       *labelToOpacityMapUC)
{
  // Is an overlay present ? If absent, the label map, passed through the
  // color lookup table is displayed as is. If not, it is blended with
  // the overlay and displayed.
  bool useOverlay = self->GetUseOverlay() ? true : false;

  int inC, outC;
  unsigned short r, f, o;
  int v0, v1, v2;
  int labelMapExtent[6];
  double opacity = 1.0;


  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the label map's extents
  labelMap->GetExtent(labelMapExtent);

  // Find the intersection of the labelmap's extents and the overlaid image's
  // extents. This is the region we need to be blending.
  labelMapExtent[0] = (labelMapExtent[0] < extent[0]) ? extent[0] : labelMapExtent[0];
  labelMapExtent[1] = (labelMapExtent[1] > extent[1]) ? extent[1] : labelMapExtent[1];
  labelMapExtent[2] = (labelMapExtent[2] < extent[2]) ? extent[2] : labelMapExtent[2];
  labelMapExtent[3] = (labelMapExtent[3] > extent[3]) ? extent[3] : labelMapExtent[3];
  labelMapExtent[4] = (labelMapExtent[4] < extent[4]) ? extent[4] : labelMapExtent[4];
  labelMapExtent[5] = (labelMapExtent[5] > extent[5]) ? extent[5] : labelMapExtent[5];

  vtkImageData *labelMapData = labelMap->GetLabelMap();

  vtkImageIterator< T > inIt( inData, labelMapExtent );
  vtkImageIterator< T > outIt( outData, labelMapExtent );
  vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > labelMapIt(
                                 labelMapData, labelMapExtent );

  unsigned char *color;

  int compositeType = 3;
  if (outC >= 3 && inC >= 4)
    {
    compositeType = 0;
    }
  else if (outC >= 3 && inC == 3)
    {
    compositeType = 1;
    }
  else if (inC == 2)
    {
    compositeType = 2;
    }

  T *beginInPtr = 0, *beginOutPtr;
  vtkKWEPaintbrushEnums::LabelType * labelPtr;

  // Loop through ouput pixels
  for (int idxZ = labelMapExtent[4]; idxZ <= labelMapExtent[5]; idxZ++)
    {
    for (int idxY = labelMapExtent[2]; idxY <= labelMapExtent[3]; idxY++)
      {
      if (useOverlay)
        {
        beginInPtr  = static_cast<T *>(inData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));
        }
      beginOutPtr = static_cast<T *>(outData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));
      labelPtr = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
          labelMapData->GetScalarPointer(labelMapExtent[0], idxY, idxZ));

      for (int idxX = labelMapExtent[0]; idxX <= labelMapExtent[1]; idxX++)
        {

        const vtkKWEPaintbrushEnums::LabelType label = *labelPtr;

        if (useOverlay)
          {

          if (label != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            // Get the color for this label in the label map.
            color = labelToColorMapUC[label];
            opacity = labelToOpacityMapUC[label];

            // round opacity to a value in the range [0,256], because division
            // by 256 can be efficiently achieved by bit-shifting by 8 bits
            o = static_cast<unsigned short>(256*opacity + 0.5);
            r = o;
            f = static_cast<unsigned short>(256 - o);

            if (compositeType == 0)
              {
              if (useOverlay)
                {
                // RGB(A) blended with RGBA

                // multiply to get a number in the range [0,65280]
                // where 65280 = 255*256 = range of beginInPtr[3] * range of o
                r = static_cast<unsigned short>(beginInPtr[3]*o);
                f = static_cast<unsigned short>(65280 - r);

                v0 = beginInPtr[0]*f + color[0]*r;
                v1 = beginInPtr[1]*f + color[1]*r;
                v2 = beginInPtr[2]*f + color[2]*r;


                // do some math tricks to achieve division by 65280:
                // this is not an approximation, it gives exactly the
                // same result as an integer division by 65280
                beginOutPtr[0] =
                  static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
                beginOutPtr[1] =
                  static_cast<T>((v1 + (v1 >> 8) + (v1 >> 16) + 1) >> 16);
                beginOutPtr[2] =
                  static_cast<T>((v2 + (v2 >> 8) + (v2 >> 16) + 1) >> 16);
                }
              else
                {
                beginOutPtr[0] = color[0];
                beginOutPtr[1] = color[1];
                beginOutPtr[2] = color[2];
                }
              }

            else if (compositeType == 1)
              {
              if (useOverlay)
                {
                // RGB(A) blended with RGB

                // the bit-shift achieves a division by 256
                beginOutPtr[0] =
                  static_cast<T>((beginInPtr[0]*f + color[0]*r) >> 8);
                beginOutPtr[1] =
                  static_cast<T>((beginInPtr[1]*f + color[1]*r) >> 8);
                beginOutPtr[2] =
                  static_cast<T>((beginInPtr[2]*f + color[2]*r) >> 8);
                }
              else
                {
                beginOutPtr[0] = color[0];
                beginOutPtr[1] = color[1];
                beginOutPtr[2] = color[2];
                }
              }

            else if (compositeType == 2)
              {
              // luminance(+alpha) blended with luminance+alpha

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of beginInPtr[1] * range of o
              r = static_cast<unsigned short>(beginInPtr[1]*o);
              f = static_cast<unsigned short>(65280 - r);
              v0 = beginOutPtr[0]*f + beginInPtr[0]*r;
              // do some math tricks to achieve division by 65280:
              // this is not an approximation, it gives exactly the
              // same result as an integer division by 65280
              beginOutPtr[0] =
                static_cast<T>((v0 + (v0 >> 8) + (v0 >> 16) + 1) >> 16);
              }

            else
              {
              // luminance(+alpha) blended with luminance

              // the bit-shift achieves a division by 256
              beginOutPtr[0] = static_cast<T>((beginInPtr[0]*f) >> 8);
              }
            }
          beginInPtr += inC;
          beginOutPtr += outC;
          ++labelPtr;
          }

        else // no overlay
          {
          // Get the color for this label in the label map.
          color = labelToColorMapUC[label];

          if (compositeType == 0)
            {
            beginOutPtr[0] = color[0];
            beginOutPtr[1] = color[1];
            beginOutPtr[2] = color[2];
            if (outC > 3) { beginOutPtr[3] = 255; }
            }

          else if (compositeType == 1)
            {
            beginOutPtr[0] = color[0];
            beginOutPtr[1] = color[1];
            beginOutPtr[2] = color[2];
            }

          else if (compositeType == 2)
            {
            beginOutPtr[0] = color[0];
            }

          else
            {
            beginOutPtr[0] = color[0];
            }
          beginOutPtr += outC;
          ++labelPtr;
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
template <class T>
void vtkKWEPaintbrushBlendExecute( vtkImageStencilData *stencil,
                                   vtkKWEPaintbrushBlend *vtkNotUsed(self),
                                   int extent[6],
                                   vtkImageData *inData, T *,
                                   vtkImageData *outData, T *,
                                   double opacity )
{
  int inC, outC;
  double r, f;
  double minA, maxA;
  int stencilExtent[6];

  if (inData->GetScalarType() == VTK_DOUBLE ||
      inData->GetScalarType() == VTK_FLOAT)
    {
    minA = 0.0;
    maxA = 1.0;
    }
  else
    {
    minA = inData->GetScalarTypeMin();
    maxA = inData->GetScalarTypeMax();
    }

  r = opacity;
  f = 1.0 - r;

  opacity = opacity/(maxA-minA);

  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the stencil extents
  stencil->GetExtent(stencilExtent);
  stencilExtent[0] = (stencilExtent[0] < extent[0]) ? extent[0] : stencilExtent[0];
  stencilExtent[1] = (stencilExtent[1] > extent[1]) ? extent[1] : stencilExtent[1];
  stencilExtent[2] = (stencilExtent[2] < extent[2]) ? extent[2] : stencilExtent[2];
  stencilExtent[3] = (stencilExtent[3] > extent[3]) ? extent[3] : stencilExtent[3];
  stencilExtent[4] = (stencilExtent[4] < extent[4]) ? extent[4] : stencilExtent[4];
  stencilExtent[5] = (stencilExtent[5] > extent[5]) ? extent[5] : stencilExtent[5];

  // Loop through ouput pixels
  for (int idxZ = stencilExtent[4]; idxZ <= stencilExtent[5]; idxZ++)
    {
    for (int idxY = stencilExtent[2]; idxY <= stencilExtent[3]; idxY++)
      {
      int iter = 0, moreSubExtents = 1, r1, r2;
      while (moreSubExtents)
        {
        moreSubExtents = stencil->GetNextExtent( r1, r2,
            stencilExtent[0], stencilExtent[1], idxY, idxZ, iter );

        if (r1 <= r2 )  // sanity check
          {
          T *beginOutPtr=
            static_cast<T *>(outData->GetScalarPointer(r1, idxY, idxZ));
          T *endOutPtr=
            static_cast<T *>(outData->GetScalarPointer(r2, idxY, idxZ));
          T *beginInPtr=
            static_cast<T *>(inData->GetScalarPointer(r1, idxY, idxZ));

          while (beginOutPtr <= endOutPtr)
            {

            if (outC >= 3 && inC >= 4)
              {
              // RGB(A) blended with RGBA

              // multiply to get a number in the range [0,65280]
              // where 65280 = 255*256 = range of beginInPtr[3] * range of o
              r = opacity*(static_cast<double>(beginInPtr[3])-minA);
              f = 1.0-r;
              beginOutPtr[0] = T(static_cast<double>(beginOutPtr[0])*f
                                 + static_cast<double>(beginInPtr[0])*r);
              beginOutPtr[1] = T(static_cast<double>(beginOutPtr[1])*f
                                 + static_cast<double>(beginInPtr[1])*r);
              beginOutPtr[2] = T(static_cast<double>(beginOutPtr[2])*f
                                 + static_cast<double>(beginInPtr[2])*r);
              beginOutPtr += outC;
              beginInPtr += inC;
              }

            else if (outC >= 3 && inC == 3)
              {
              // RGB(A) blended with RGB

              // the bit-shift achieves a division by 256
              beginOutPtr[0] = T(static_cast<double>(beginOutPtr[0])*f
                                 + static_cast<double>(beginInPtr[0])*r);
              beginOutPtr[1] = T(static_cast<double>(beginOutPtr[1])*f
                                 + static_cast<double>(beginInPtr[1])*r);
              beginOutPtr[2] = T(static_cast<double>(beginOutPtr[2])*f
                                 + static_cast<double>(beginInPtr[2])*r);
              beginOutPtr += outC;
              beginInPtr += inC;
              }

            else if (inC == 2)
              {
              r = opacity*(static_cast<double>(beginInPtr[1])-minA);
              f = 1.0-r;
              *beginOutPtr = T(static_cast<double>(*beginOutPtr)*f
                               + static_cast<double>(*beginInPtr)*r);
              beginOutPtr += outC;
              beginInPtr += 2;
              }

            else
              {
              // luminance(+alpha) blended with luminance

              // the bit-shift achieves a division by 256
              *beginOutPtr = T(static_cast<double>(*beginOutPtr)*f + static_cast<double>(*beginInPtr)*r);
              beginOutPtr += outC;
              beginInPtr++;
              }
            }
          }
        }
      }
    }
}

//----------------------------------------------------------------------------
// Label map blend method for general overlay data (not unsigned char overlays)
template <class T>
void vtkKWEPaintbrushBlendExecute(
  vtkKWEPaintbrushLabelData *labelMap,
  vtkKWEPaintbrushBlend *vtkNotUsed(self),
  int extent[6],
  vtkImageData *inData, T *,
  vtkImageData *outData, T *,
  vtkKWEPaintbrushPropertyManager::LabelToColorMapType &labelToColorMap )
{
  int inC, outC;
  double r, f, opacity;
  double minA, maxA;
  int labelMapExtent[6];


  if (inData->GetScalarType() == VTK_DOUBLE ||
      inData->GetScalarType() == VTK_FLOAT)
    {
    minA = 0.0;
    maxA = 1.0;
    }
  else
    {
    minA = inData->GetScalarTypeMin();
    maxA = inData->GetScalarTypeMax();
    }

  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the label map's extents
  labelMap->GetExtent(labelMapExtent);

  // Find the intersection of the labelmap's extents and the overlaid image's
  // extents. This is the region we need to be blending.
  labelMapExtent[0] = (labelMapExtent[0] < extent[0]) ? extent[0] : labelMapExtent[0];
  labelMapExtent[1] = (labelMapExtent[1] > extent[1]) ? extent[1] : labelMapExtent[1];
  labelMapExtent[2] = (labelMapExtent[2] < extent[2]) ? extent[2] : labelMapExtent[2];
  labelMapExtent[3] = (labelMapExtent[3] > extent[3]) ? extent[3] : labelMapExtent[3];
  labelMapExtent[4] = (labelMapExtent[4] < extent[4]) ? extent[4] : labelMapExtent[4];
  labelMapExtent[5] = (labelMapExtent[5] > extent[5]) ? extent[5] : labelMapExtent[5];

  vtkImageIterator< T > inIt( inData, labelMapExtent );
  vtkImageIterator< T > outIt( outData, labelMapExtent );
  vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > labelMapIt(
                                     labelMap->GetLabelMap(), labelMapExtent );

  // Lookup table of label to color map.
  unsigned char color[3];
  vtkKWEPaintbrushPropertyManager::vtkKWEPaintbrushLabelMapColor colorClass;

  // Loop through pixels
  while (!labelMapIt.IsAtEnd())
    {
    vtkKWEPaintbrushEnums::LabelType *labelMapSI    = labelMapIt.BeginSpan();
    vtkKWEPaintbrushEnums::LabelType *labelMapSIEnd = labelMapIt.EndSpan();
    T                             *inSI          = inIt.BeginSpan();
    T                             *outSI         = outIt.BeginSpan();

    while (labelMapSI != labelMapSIEnd)
      {

      const vtkKWEPaintbrushEnums::LabelType label = *labelMapSI;
      colorClass = labelToColorMap[label];
      color[0] = colorClass.Color[0];
      color[1] = colorClass.Color[1];
      color[2] = colorClass.Color[2];

      opacity = colorClass.Opacity;
      r = opacity;
      f = 1.0 - r;
      opacity = opacity/(maxA-minA);

      if (outC >= 3 && inC >= 4)
        {
        // RGB(A) blended with RGBA

        // multiply to get a number in the range [0,65280]
        // where 65280 = 255*256 = range of inSI[3] * range of o
        r = opacity*(static_cast<double>(inSI[3])-minA);
        f = 1.0-r;
        outSI[0] = T(static_cast<double>(outSI[0])*f +
                     static_cast<double>(inSI[0])*r);
        outSI[1] = T(static_cast<double>(outSI[1])*f +
                     static_cast<double>(inSI[1])*r);
        outSI[2] = T(static_cast<double>(outSI[2])*f +
                     static_cast<double>(inSI[2])*r);
        outSI += outC;
        inSI += inC;
        }

      else if (outC >= 3 && inC == 3)
        {
        // RGB(A) blended with RGB

        // the bit-shift achieves a division by 256
        outSI[0] = T(static_cast<double>(outSI[0])*f +
                     static_cast<double>(inSI[0])*r);
        outSI[1] = T(static_cast<double>(outSI[1])*f +
                     static_cast<double>(inSI[1])*r);
        outSI[2] = T(static_cast<double>(outSI[2])*f +
                     static_cast<double>(inSI[2])*r);
        outSI += outC;
        inSI += inC;
        }

      else if (inC == 2)
        {
        r = opacity*(static_cast<double>(inSI[1])-minA);
        f = 1.0-r;
        *outSI = T(static_cast<double>(*outSI)*f +
                   static_cast<double>(*inSI)*r);
        outSI += outC;
        inSI += 2;
        }

      else
        {
        // luminance(+alpha) blended with luminance

        // the bit-shift achieves a division by 256
        *outSI = T(static_cast<double>(*outSI)*f +
                   static_cast<double>(*inSI)*r);
        outSI += outC;
        inSI++;
        }

      ++labelMapSI;
      inSI  += inC;
      outSI += outC;
      }

    labelMapIt.NextSpan();
    inIt.NextSpan();
    outIt.NextSpan();
    }
}

//----------------------------------------------------------------------------
// This templated function blends an image that represents a label map into
// the overlay image. This function is specific to the case when the overlay
// image has data type of CHAR
template <class T>
void vtkKWEPaintbrushBlendExecuteLabelMapTypeUC(
                vtkKWEPaintbrushLabelData *labelMap,
                vtkKWEPaintbrushBlend *vtkNotUsed(self),
                int extent[6],
                vtkImageData *inData, T *,
                vtkImageData *outData, T *,
                VTKColorType *labelToColorMapUC,
                double       *labelToOpacityMapUC)
{
  int inC, outC;
  double r, f, opacity;
  double minA, maxA;
  int labelMapExtent[6];
  unsigned char *color;

  if (inData->GetScalarType() == VTK_DOUBLE ||
      inData->GetScalarType() == VTK_FLOAT)
    {
    minA = 0.0;
    maxA = 1.0;
    }
  else
    {
    minA = inData->GetScalarTypeMin();
    maxA = inData->GetScalarTypeMax();
    }

  inC = inData->GetNumberOfScalarComponents();
  outC = outData->GetNumberOfScalarComponents();

  // Check the label map's extents
  labelMap->GetExtent(labelMapExtent);

  // Find the intersection of the labelmap's extents and the overlaid image's
  // extents. This is the region we need to be blending.
  labelMapExtent[0] = (labelMapExtent[0] < extent[0]) ? extent[0] : labelMapExtent[0];
  labelMapExtent[1] = (labelMapExtent[1] > extent[1]) ? extent[1] : labelMapExtent[1];
  labelMapExtent[2] = (labelMapExtent[2] < extent[2]) ? extent[2] : labelMapExtent[2];
  labelMapExtent[3] = (labelMapExtent[3] > extent[3]) ? extent[3] : labelMapExtent[3];
  labelMapExtent[4] = (labelMapExtent[4] < extent[4]) ? extent[4] : labelMapExtent[4];
  labelMapExtent[5] = (labelMapExtent[5] > extent[5]) ? extent[5] : labelMapExtent[5];

  vtkImageIterator< T > inIt( inData, labelMapExtent );
  vtkImageIterator< T > outIt( outData, labelMapExtent );
  vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > labelMapIt(
                                     labelMap->GetLabelMap(), labelMapExtent );

  // Loop through pixels
  while (!labelMapIt.IsAtEnd())
    {
    vtkKWEPaintbrushEnums::LabelType *labelMapSI    = labelMapIt.BeginSpan();
    vtkKWEPaintbrushEnums::LabelType *labelMapSIEnd = labelMapIt.EndSpan();
    T                             *inSI          = inIt.BeginSpan();
    T                             *outSI         = outIt.BeginSpan();

    while (labelMapSI != labelMapSIEnd)
      {

      // Fast retrieve from the lookup table to fetch the color for this label.
      color = labelToColorMapUC[*labelMapSI];

      // Blend based on opacity
      opacity = labelToOpacityMapUC[*labelMapSI];
      r = opacity;
      f = 1.0 - r;
      opacity = opacity/(maxA-minA);

      if (outC >= 3 && inC >= 4)
        {
        // RGB(A) blended with RGBA

        // multiply to get a number in the range [0,65280]
        // where 65280 = 255*256 = range of inSI[3] * range of o
        r = opacity*(static_cast<double>(inSI[3])-minA);
        f = 1.0-r;
        outSI[0] = T(static_cast<double>(outSI[0])*f +
                     static_cast<double>(inSI[0])*r);
        outSI[1] = T(static_cast<double>(outSI[1])*f +
                     static_cast<double>(inSI[1])*r);
        outSI[2] = T(static_cast<double>(outSI[2])*f +
                     static_cast<double>(inSI[2])*r);
        outSI += outC;
        inSI += inC;
        }

      else if (outC >= 3 && inC == 3)
        {
        // RGB(A) blended with RGB

        // the bit-shift achieves a division by 256
        outSI[0] = T(static_cast<double>(outSI[0])*f +
                     static_cast<double>(inSI[0])*r);
        outSI[1] = T(static_cast<double>(outSI[1])*f +
                     static_cast<double>(inSI[1])*r);
        outSI[2] = T(static_cast<double>(outSI[2])*f +
                     static_cast<double>(inSI[2])*r);
        outSI += outC;
        inSI += inC;
        }

      else if (inC == 2)
        {
        r = opacity*(static_cast<double>(inSI[1])-minA);
        f = 1.0-r;
        *outSI = T(static_cast<double>(*outSI)*f +
                   static_cast<double>(*inSI)*r);
        outSI += outC;
        inSI += 2;
        }

      else
        {
        // luminance(+alpha) blended with luminance

        // the bit-shift achieves a division by 256
        *outSI = T(static_cast<double>(*outSI)*f +
                   static_cast<double>(*inSI)*r);
        outSI += outC;
        inSI++;
        }

      ++labelMapSI;
      inSI  += inC;
      outSI += outC;
      }

    labelMapIt.NextSpan();
    inIt.NextSpan();
    outIt.NextSpan();
    }
}

//----------------------------------------------------------------------------
// This function simply does a copy (for the first input)
//----------------------------------------------------------------------------
void vtkKWEPaintbrushBlendCopyData(vtkImageData *inData, vtkImageData *outData,
                           int *ext)
{
  int idxY, idxZ, maxY, maxZ;
  vtkIdType inIncX, inIncY, inIncZ;
  vtkIdType rowLength;
  unsigned char *inPtr, *inPtr1, *outPtr;

  inPtr = static_cast<unsigned char *>(inData->GetScalarPointerForExtent(ext));

  // Get increments to march through inData
  inData->GetIncrements(inIncX, inIncY, inIncZ);

  // find the region to loop over
  rowLength = (ext[1] - ext[0]+1)*inIncX*inData->GetScalarSize();
  maxY = ext[3] - ext[2];
  maxZ = ext[5] - ext[4];

  outPtr=static_cast<unsigned char *>(outData->GetScalarPointerForExtent(ext));

  inData->GetIncrements(inIncX, inIncY, inIncZ);
  inIncY *= inData->GetScalarSize();
  inIncZ *= inData->GetScalarSize();

  // Loop through outData pixels
  for (idxZ = 0; idxZ <= maxZ; idxZ++)
    {
    inPtr1 = inPtr + idxZ*inIncZ;
    for (idxY = 0; idxY <= maxY; idxY++)
      {
      memcpy(outPtr,inPtr1,rowLength);
      inPtr1 += inIncY;
      outPtr += rowLength;
      }
    }
}

//----------------------------------------------------------------------------
// This method is passed a input and output regions, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the regions data types.
void vtkKWEPaintbrushBlend::ThreadedRequestData (
  vtkInformation * vtkNotUsed( request ),
  vtkInformationVector** inputVector,
  vtkInformationVector * vtkNotUsed( outputVector ),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6],
  int vtkNotUsed(id))
{
  void *inPtr;
  void *outPtr;

  // check
  if (inData[0][0]->GetNumberOfScalarComponents() > 4)
    {
    vtkErrorMacro("The first input can have a maximum of four components");
    return;
    }

  vtkInformation *info = inputVector[1]->GetInformationObject(0);
  vtkKWEPaintbrushDrawing *drawing = vtkKWEPaintbrushDrawing::SafeDownCast(
                     info->Get(vtkDataObject::DATA_OBJECT()));

  if (this->GetUseOverlay())
    {
    // copy the first image directly to the output
    vtkKWEPaintbrushBlendCopyData(inData[0][0], outData[0], outExt);
    }

  // this filter expects that input is the same type as output.
  if (inData[0][0]->GetScalarType() != outData[0]->GetScalarType())
    {
    vtkErrorMacro(
      << "Execute: input ScalarType (" << inData[0][0]->GetScalarType()
      << "), must match output ScalarType (" << outData[0]->GetScalarType()
      << ")" );
    return;
    }

  inPtr = inData[0][0]->GetScalarPointerForExtent(outExt);
  outPtr = outData[0]->GetScalarPointerForExtent(outExt);

  if (drawing->GetRepresentation() == vtkKWEPaintbrushEnums::Binary)
    {

    // Loop over each stencil and blend it.
    for (int n = 0; n < drawing->GetNumberOfItems(); n++)
      {
      vtkKWEPaintbrushSketch *sketch = drawing->GetItem(n);
      vtkImageStencilData *stencil = (vtkKWEPaintbrushStencilData::
        SafeDownCast(sketch->GetPaintbrushData()))->GetImageStencilData();

      // The color of the blend.
      unsigned char color[3];
      double colorD[3];
      sketch->GetPaintbrushProperty()->GetColor(colorD);
      color[0] = static_cast< unsigned char >(colorD[0] * 255.0);
      color[1] = static_cast< unsigned char >(colorD[1] * 255.0);
      color[2] = static_cast< unsigned char >(colorD[2] * 255.0);
      double opacity = sketch->GetPaintbrushProperty()->GetOpacity();

      // for performance reasons, use a special method for unsigned char
      if (inData[0][0]->GetScalarType() == VTK_UNSIGNED_CHAR)
        {
        vtkKWEPaintbrushBlendExecuteChar(
              stencil, this, outExt,
              inData[0][0], static_cast<unsigned char *>(inPtr),
              outData[0], static_cast<unsigned char *>(outPtr),
              opacity, color );
        }
      else
        {
        switch (inData[0][0]->GetScalarType())
          {
          vtkTemplateMacro(
            vtkKWEPaintbrushBlendExecute(stencil, this, outExt,
                                 inData[0][0], static_cast<VTK_TT *>(inPtr),
                                 outData[0], static_cast<VTK_TT *>(outPtr),
                                 opacity ));
          default:
            vtkErrorMacro(<< "Execute: Unknown ScalarType");
            return;
          }
        }
      }

    }
  else if (drawing->GetRepresentation() == vtkKWEPaintbrushEnums::Label)
    {

    // the label map we need to blend onto our overlay image.
    vtkKWEPaintbrushLabelData *labelData =
      vtkKWEPaintbrushLabelData::SafeDownCast(
              drawing->GetPaintbrushData() );

    vtkKWEPaintbrushPropertyManager *propertyManager
          = drawing->GetPaintbrushPropertyManager();


    // for performance reasons, use a special method for unsigned char
    if (inData[0][0]->GetScalarType() == VTK_UNSIGNED_CHAR)
      {
      if (vtkKWEPaintbrushEnums::GetLabelType() == VTK_UNSIGNED_CHAR)
        {

        // Fast Lookup table of label to color map.
        VTKColorType *labelToColorMapUC   = propertyManager->LabelToColorMapUC;
        double       *labelToOpacityMapUC = propertyManager->LabelToOpacityMapUC;

        vtkKWEPaintbrushBlendExecuteCharLabelMapTypeUC(
            labelData, this, outExt,
            inData[0][0], static_cast<unsigned char *>(inPtr),
            outData[0], static_cast<unsigned char *>(outPtr),
            labelToColorMapUC, labelToOpacityMapUC );
        }
      else
        {
        vtkKWEPaintbrushBlendExecuteChar(
            labelData, this, outExt,
            inData[0][0], static_cast<unsigned char *>(inPtr),
            outData[0], static_cast<unsigned char *>(outPtr),
            propertyManager->LabelToColorMap );
        }
      }
    else
      {
      if (vtkKWEPaintbrushEnums::GetLabelType() == VTK_UNSIGNED_CHAR)
        {
        // Fast Lookup table of label to color map.
        VTKColorType *labelToColorMapUC   = propertyManager->LabelToColorMapUC;
        double       *labelToOpacityMapUC = propertyManager->LabelToOpacityMapUC;

        switch (inData[0][0]->GetScalarType())
          {
          vtkTemplateMacro(
            vtkKWEPaintbrushBlendExecuteLabelMapTypeUC(
                                 labelData, this, outExt,
                                 inData[0][0], static_cast<VTK_TT *>(inPtr),
                                 outData[0], static_cast<VTK_TT *>(outPtr),
                                 labelToColorMapUC, labelToOpacityMapUC ));
          default:
            vtkErrorMacro(<< "Execute: Unknown ScalarType");
            return;
          }
        }
      else
        {
        switch (inData[0][0]->GetScalarType())
          {
          vtkTemplateMacro(
            vtkKWEPaintbrushBlendExecute(
                                 labelData, this, outExt,
                                 inData[0][0], static_cast<VTK_TT *>(inPtr),
                                 outData[0], static_cast<VTK_TT *>(outPtr),
                                 propertyManager->LabelToColorMap ));
          default:
            vtkErrorMacro(<< "Execute: Unknown ScalarType");
            return;
          }
        }
      } // Endif LabelType is UCHAR or everything else
    } // Endif Drawing type (LabelMap or Stencil)
}


//----------------------------------------------------------------------------
int vtkKWEPaintbrushBlend::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    return 1;
    }
  else if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkKWEPaintbrushDrawing");
    return 1;
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushBlend::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Opacity:      " << this->Opacity    << endl;
  os << indent << "UseOverlay:   " << this->UseOverlay << endl;
}

