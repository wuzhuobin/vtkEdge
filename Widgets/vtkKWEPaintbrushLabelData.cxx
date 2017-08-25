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
#include "vtkKWEPaintbrushLabelData.h"

#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushEnums.h"
#include "vtkImageStencilData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"

#include <math.h>

vtkCxxRevisionMacro(vtkKWEPaintbrushLabelData, "$Revision: 763 $");
vtkStandardNewMacro(vtkKWEPaintbrushLabelData);
vtkCxxSetObjectMacro(vtkKWEPaintbrushLabelData, LabelMap, vtkImageData);

//----------------------------------------------------------------------------
vtkKWEPaintbrushEnums::LabelType vtkKWEPaintbrushLabelData::NoLabelValue = 0;

//----------------------------------------------------------------------------
vtkKWEPaintbrushLabelData::vtkKWEPaintbrushLabelData()
{
  this->LabelMap = vtkImageData::New();
  this->LabelMap->SetScalarType( vtkKWEPaintbrushEnums::GetLabelType() );

  this->Information->Set( vtkDataObject::DATA_EXTENT_TYPE(), VTK_3D_EXTENT );
  this->Information->Set( vtkDataObject::DATA_EXTENT(), 
                          this->LabelMap->GetExtent(), 6);
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushLabelData::~vtkKWEPaintbrushLabelData()
{
  if (this->LabelMap)
    {
    this->LabelMap->Delete();
    this->LabelMap = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::Initialize()
{
  this->LabelMap->Initialize();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::CopyInformationToPipeline(vtkInformation* request,
                                                    vtkInformation* input,
                                                    vtkInformation* output,
                                                    int forceCopy)
{
  this->LabelMap->CopyInformationToPipeline( 
          request, input, output, forceCopy );
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::CopyInformationFromPipeline(vtkInformation* request)
{
  this->LabelMap->CopyInformationFromPipeline(request);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::CopyOriginAndSpacingFromPipeline()
{
  // Copy origin and spacing from pipeline information to the internal copies
  vtkInformation* info = this->PipelineInformation;
  if(info->Has(SPACING()))
    {
    this->SetSpacing(info->Get(SPACING()));
    }
  if(info->Has(ORIGIN()))
    {
    this->SetOrigin(info->Get(ORIGIN()));
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetExtent(int* extent)
{
  this->LabelMap->SetExtent(extent);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetExtent(int x1, int x2, int y1, int y2, int z1, int z2)
{
  int ext[6];
  ext[0] = x1;
  ext[1] = x2;
  ext[2] = y1;
  ext[3] = y2;
  ext[4] = z1;
  ext[5] = z2;
  this->LabelMap->SetExtent(ext);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::GetExtent(int e[6])
{
  this->LabelMap->GetExtent(e);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::ShallowCopy(vtkDataObject *o)
{
  vtkKWEPaintbrushLabelData *s = vtkKWEPaintbrushLabelData::SafeDownCast(o);
  if (s)
    {
    this->LabelMap->ShallowCopy(s->GetLabelMap());
    }

  vtkDataObject::ShallowCopy(o);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::DeepCopy(vtkDataObject *o)
{
  vtkKWEPaintbrushLabelData *s = vtkKWEPaintbrushLabelData::SafeDownCast(o);
  if (s)
    {
    this->LabelMap->DeepCopy(s->GetLabelMap());
    }

  vtkDataObject::DeepCopy(o);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::Allocate( double value )
{
  // Allocate
  this->LabelMap->AllocateScalars();

  // Fill with the unused label value.
  
  vtkDataArray * array = this->LabelMap->GetPointData()->GetScalars();
  vtkKWEPaintbrushEnums::LabelType *arrayPointer = 
    static_cast<vtkKWEPaintbrushEnums::LabelType *>(array->GetVoidPointer(0));
  unsigned long size = array->GetDataSize();

  for (unsigned long i = 0; i < size; i++)
    {
    *arrayPointer = static_cast<vtkKWEPaintbrushEnums::LabelType>(value);
    ++arrayPointer;
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::Clear( vtkKWEPaintbrushEnums::LabelType label )
{
  if (this->LabelMap->GetPointData()->GetScalars() == NULL)
    {
    // Data hasn't been allocated yet. Allocate and be done
    this->Allocate(0.0);
    }
  else
    {
    // Clear label
  
    vtkDataArray * array = this->LabelMap->GetPointData()->GetScalars();
    vtkKWEPaintbrushEnums::LabelType *arrayPointer = 
      static_cast<vtkKWEPaintbrushEnums::LabelType *>(array->GetVoidPointer(0));
    unsigned long size = array->GetDataSize();


    for (unsigned long i = 0; i < size; i++)
      {
      if (*arrayPointer == label)
        {
        *arrayPointer = vtkKWEPaintbrushLabelData::NoLabelValue;
        }
      ++arrayPointer;
      }

    this->LabelMap->Modified();
    }
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushLabelData* vtkKWEPaintbrushLabelData::GetData(vtkInformation* info)
{
  return info? vtkKWEPaintbrushLabelData::SafeDownCast(info->Get(DATA_OBJECT())) : 0;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushLabelData* vtkKWEPaintbrushLabelData::GetData(vtkInformationVector* v,
                                                  int i)
{
  return vtkKWEPaintbrushLabelData::GetData(v->GetInformationObject(i));
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushLabelData::Add( vtkKWEPaintbrushData * data, bool forceMutable )
{
  int extentToBeAdded[6], currentExtent[6], extent[6];
  data->GetExtent(extentToBeAdded);
  this->GetExtent(currentExtent);

  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents( 
             currentExtent, extentToBeAdded, extent ))
    {
    // The stuff we are adding is entirely outside our bounds.
    // Nothing to add. 
    return 0; 
    }

  // Handle Immubtability of sketches. 
  // For details, see vtkKWEPaintbrushProperty::SetMubtale
  const bool mutableLabelsPresent = (this->ImmutableLabels.size() > 0);
  vtkstd::set< vtkKWEPaintbrushEnums::LabelType >::const_iterator 
                    iterEnd = this->ImmutableLabels.end();

  
  if (vtkKWEPaintbrushLabelData *labelData = 
      vtkKWEPaintbrushLabelData::SafeDownCast(data))
    {
    // PaintbrushLabelData + PaintbrushLabelData

    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it2( 
                    labelData->GetLabelMap(), extent );
    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it1( 
                    this->LabelMap, extent );

    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    while( !it1.IsAtEnd() )
      { 
      vtkKWEPaintbrushEnums::LabelType *inSI    = it1.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSI2   = it2.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSIEnd = it1.EndSpan();
      if( mutableLabelsPresent && !forceMutable )
        {
        while (inSI != inSIEnd) 
          {
          vtkKWEPaintbrushEnums::LabelType l = *inSI2;
          if (l != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            if( lastLabel != *inSI )
              {
              lastLabel = *inSI;
              if( this->ImmutableLabels.find(*inSI) == iterEnd )
                {
                immutableLabel = false;
                }
              else
                {
                immutableLabel = true;
                }
              }
            if( !immutableLabel )
              {
              *inSI = l;
              }
            }
          ++inSI;
          ++inSI2;
          }      
        }
      else
        {
        while (inSI != inSIEnd) 
          {
          vtkKWEPaintbrushEnums::LabelType l = *inSI2;
          if (l != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            *inSI = l;
            }
          ++inSI;
          ++inSI2;
          }      
        }
      it1.NextSpan();
      it2.NextSpan();
      }
    }

  else if (vtkKWEPaintbrushStencilData *binaryPaintbrushData =
           vtkKWEPaintbrushStencilData::SafeDownCast(data))
    {
    // PaintbrushLabelData + PaintbrushStencilData
      
    vtkImageStencilData *stencilData 
      = binaryPaintbrushData->GetImageStencilData();

    vtkKWEPaintbrushEnums::LabelType l = binaryPaintbrushData->GetLabel();

    int r1, r2, moreSubExtents, iter;
    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    for (int z=extent[4]; z <= extent[5]; z++)
      {
      for (int y=extent[2]; y <= extent[3]; y++)
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
            vtkKWEPaintbrushEnums::LabelType *beginExtent =
              static_cast<vtkKWEPaintbrushEnums::LabelType *>(this->LabelMap->GetScalarPointer(r1, y, z));
            vtkKWEPaintbrushEnums::LabelType *endExtent   =
              static_cast<vtkKWEPaintbrushEnums::LabelType *>(this->LabelMap->GetScalarPointer(r2, y, z));
            if( mutableLabelsPresent && !forceMutable )
              {
              while (beginExtent <= endExtent)
                {
                // If this voxel is mutable..
                if( lastLabel != *beginExtent )
                  {
                  lastLabel = *beginExtent;
                  if( this->ImmutableLabels.find(*beginExtent) == iterEnd )
                    {
                    immutableLabel = false;
                    }
                  else
                    {
                    immutableLabel = true;
                    }
                  }
                if( !immutableLabel )
                  {
                  *beginExtent = l;
                  }
                ++beginExtent;
                }
              }
            else
              {
              while (beginExtent <= endExtent)
                {
                *beginExtent = l;
                ++beginExtent;
                }
              }
            }
          } // end for each extent tuple
        } // end for each scan line
      } // end of each slice 
    }

  this->LabelMap->Modified();
  this->Modified();
  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushLabelData::Subtract( vtkKWEPaintbrushData * data, bool forceMutable )
{
  int extentToBeSubtracted[6], currentExtent[6], extent[6];
  data->GetExtent(extentToBeSubtracted);
  this->GetExtent(currentExtent);

  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents( 
         currentExtent, extentToBeSubtracted, extent ))
    {
    // The stuff we are removing is entirely outside our bounds.
    // Nothing to add. 
    return 0; 
    }

  // Handle Immubtability of sketches. 
  // For details, see vtkKWEPaintbrushProperty::SetMubtale
  const bool mutableLabelsPresent = (this->ImmutableLabels.size() > 0);
  vtkstd::set< vtkKWEPaintbrushEnums::LabelType >::const_iterator 
                    iterEnd = this->ImmutableLabels.end();
  

  if (vtkKWEPaintbrushLabelData *labelData = 
      vtkKWEPaintbrushLabelData::SafeDownCast(data))
    {
    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it2( 
                    labelData->GetLabelMap(), extent );
    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it1( 
                    this->LabelMap, extent );
    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    while( !it1.IsAtEnd() )
      { 
      vtkKWEPaintbrushEnums::LabelType *inSI    = it1.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSI2   = it2.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSIEnd = it1.EndSpan();
      if( mutableLabelsPresent && !forceMutable )
        {
        while (inSI != inSIEnd) 
          {
          if (*inSI2 != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            // If this voxel is mutable..
            if( lastLabel != *inSI )
              {
              lastLabel = *inSI;
              if( this->ImmutableLabels.find(*inSI) == iterEnd )
                {
                immutableLabel = false;
                }
              else
                {
                immutableLabel = true;
                }
              }
            if( !immutableLabel )
              {
              *inSI = vtkKWEPaintbrushLabelData::NoLabelValue;
              }
            }
          ++inSI;
          ++inSI2;
          }      
        }
      else
        {
        while (inSI != inSIEnd) 
          {
          if (*inSI2 != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            // If this voxel is mutable..
            *inSI = vtkKWEPaintbrushLabelData::NoLabelValue;
            }
          ++inSI;
          ++inSI2;
          }      
        }
      it1.NextSpan();
      it2.NextSpan();
      }
    }

  else if (vtkKWEPaintbrushStencilData *binaryPaintbrushData =
           vtkKWEPaintbrushStencilData::SafeDownCast(data))
    {
    vtkImageStencilData *stencilData 
      = binaryPaintbrushData->GetImageStencilData();

    int r1, r2, moreSubExtents, iter;
    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    for (int z=extent[4]; z <= extent[5]; z++)
      {
      for (int y=extent[2]; y <= extent[3]; y++)
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
            vtkKWEPaintbrushEnums::LabelType *beginExtent =
              static_cast<vtkKWEPaintbrushEnums::LabelType *>(this->LabelMap->GetScalarPointer(r1, y, z));
            vtkKWEPaintbrushEnums::LabelType *endExtent   =
              static_cast<vtkKWEPaintbrushEnums::LabelType *>(this->LabelMap->GetScalarPointer(r2, y, z));
            if( mutableLabelsPresent && !forceMutable )
              {
              while (beginExtent <= endExtent)
                {
                if( lastLabel != *beginExtent )
                  {
                  lastLabel = *beginExtent;
                  if( this->ImmutableLabels.find(*beginExtent) == iterEnd )
                    {
                    immutableLabel = false;
                    }
                  else
                    {
                    immutableLabel = true;
                    }
                  }
                if( !immutableLabel )
                  {
                  *beginExtent = vtkKWEPaintbrushLabelData::NoLabelValue;
                  }
                ++beginExtent;
                }
              }
            else
              {
              while (beginExtent <= endExtent)
                {
                *beginExtent = vtkKWEPaintbrushLabelData::NoLabelValue;
                ++beginExtent;
                }
              }
            }
          } // end for each extent tuple
        } // end for each scan line
      } // end of each slice 
    }

  this->LabelMap->Modified();
  this->Modified();
  return 1;
} 

//----------------------------------------------------------------------------
int vtkKWEPaintbrushLabelData::Replace( vtkKWEPaintbrushData * data, bool forceMutable )
{
  int extentToBeReplaced[6], currentExtent[6], extent[6];
  data->GetExtent(extentToBeReplaced);
  this->GetExtent(currentExtent);

  if( !vtkKWEPaintbrushUtilities::GetIntersectingExtents( 
         currentExtent, extentToBeReplaced, extent ) )
    {
    // The stuff we are removing is entirely outside our bounds.
    // Nothing to add. 
    return 0; 
    }

  // Handle Immubtability of sketches. 
  // For details, see vtkKWEPaintbrushProperty::SetMubtale
  const bool mutableLabelsPresent = (this->ImmutableLabels.size() > 0);
  vtkstd::set< vtkKWEPaintbrushEnums::LabelType >::const_iterator 
                    iterEnd = this->ImmutableLabels.end();
  

  if (vtkKWEPaintbrushLabelData *labelData = 
      vtkKWEPaintbrushLabelData::SafeDownCast(data))
    {
    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it2( 
                    labelData->GetLabelMap(), extent );
    vtkImageIterator< vtkKWEPaintbrushEnums::LabelType > it1( 
                    this->LabelMap, extent );
    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    while( !it1.IsAtEnd() )
      { 
      vtkKWEPaintbrushEnums::LabelType *inSI    = it1.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSI2   = it2.BeginSpan();  
      vtkKWEPaintbrushEnums::LabelType *inSIEnd = it1.EndSpan();
      if( mutableLabelsPresent && !forceMutable )
        {
        while (inSI != inSIEnd) 
          {
          // If this voxel is mutable..
          if( lastLabel != *inSI )
            {
            lastLabel = *inSI;
            if( this->ImmutableLabels.find(*inSI) == iterEnd )
              {
              immutableLabel = false;
              }
            else
              {
              immutableLabel = true;
              }
            }
          if( !immutableLabel )
            {
            if( *inSI2 == this->GetLabel() )
              {
              *inSI = this->GetLabel();
              }
            else
              {
              if( *inSI == this->GetLabel() )
                {
                *inSI = vtkKWEPaintbrushLabelData::NoLabelValue;
                }
              }
            }
          ++inSI;
          ++inSI2;
          }      
        }
      else
        {
        while (inSI != inSIEnd) 
          {
          if( *inSI2 == this->GetLabel() )
            {
            *inSI = this->GetLabel();
            }
          else
            {
            if( *inSI == this->GetLabel() )
              {
              *inSI = vtkKWEPaintbrushLabelData::NoLabelValue;
              }
            }
          ++inSI;
          ++inSI2;
          }      
        }
      it1.NextSpan();
      it2.NextSpan();
      }
    }
  else if (vtkKWEPaintbrushStencilData *binaryPaintbrushData =
           vtkKWEPaintbrushStencilData::SafeDownCast(data))
    {
    vtkImageStencilData *stencilData 
      = binaryPaintbrushData->GetImageStencilData();

    int r1, r2, moreSubExtents, iter;
    bool immutableLabel = false;
    vtkKWEPaintbrushEnums::LabelType lastLabel = 
                                  vtkKWEPaintbrushLabelData::NoLabelValue;
    for (int z=extent[4]; z <= extent[5]; z++)
      {
      for (int y=extent[2]; y <= extent[3]; y++)
        {
        vtkKWEPaintbrushEnums::LabelType *beginExtent = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
                                                       this->LabelMap->GetScalarPointer(extent[0], y, z) );
        vtkKWEPaintbrushEnums::LabelType *endExtent   = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
                                                       this->LabelMap->GetScalarPointer(extent[1], y, z) );
        while( beginExtent < endExtent )
          {
          if( *beginExtent == this->GetLabel() )
            {
            *beginExtent = vtkKWEPaintbrushLabelData::NoLabelValue;
            }
          ++beginExtent;
          }
        iter = 0;
        moreSubExtents = 1;
        while( moreSubExtents )
          {
          moreSubExtents = stencilData->GetNextExtent(
            r1, r2, extent[0], extent[1], y, z, iter);

          // sanity check
          if (r1 <= r2)
            {
            beginExtent = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
                                this->LabelMap->GetScalarPointer(r1, y, z));
            endExtent   = static_cast<vtkKWEPaintbrushEnums::LabelType *>(
                                this->LabelMap->GetScalarPointer(r2, y, z));
            if( mutableLabelsPresent && !forceMutable )
              {
              while (beginExtent <= endExtent)
                {
                if( lastLabel != *beginExtent )
                  {
                  lastLabel = *beginExtent;
                  if( this->ImmutableLabels.find(*beginExtent) == iterEnd )
                    {
                    immutableLabel = false;
                    }
                  else
                    {
                    immutableLabel = true;
                    }
                  }
                if( !immutableLabel )
                  {
                  *beginExtent = this->GetLabel();
                  }
                ++beginExtent;
                }
              }
            else
              {
              while (beginExtent <= endExtent)
                {
                *beginExtent = this->GetLabel();
                ++beginExtent;
                }
              }
            }
          } // end for each extent tuple
        } // end for each scan line
      } // end of each slice 
    }

  this->LabelMap->Modified();
  this->Modified();
  return 1;
} 

//----------------------------------------------------------------------------
int vtkKWEPaintbrushLabelData::Clip( int extent[6] )
{
  int currentExtent[6], idy, idz, idx;
  this->Update();
  this->GetExtent( currentExtent );

  if (vtkMath::ExtentIsWithinOtherExtent( currentExtent, extent ))
    {
    // Nothing to do, we are already within the clipping extents.
    return 0;
    }


  // Does not resize the current allocation if the clipped region is smaller.
  // It simply blanks out data outside the extents specified.
  // TODO: Check if this has an impact on memory or performance.

  bool remove = false, removed = false;

  vtkKWEPaintbrushEnums::LabelType *ptr = 
    static_cast<vtkKWEPaintbrushEnums::LabelType *>(this->LabelMap->GetScalarPointer());

  for (idz=currentExtent[4]; idz<=currentExtent[5]; idz++)
    {
    remove = (idz < extent[4] || idz > extent[5]);
    for (idy = currentExtent[2]; idy <= currentExtent[3]; idy++)
      {
      remove |= (idy < extent[2] || idy > extent[3]);
      for (idx = currentExtent[4]; idx <= currentExtent[5]; idx++)
        {
        remove |= (idx < extent[4] || idx > extent[5]);
        if (remove)
          {
          *ptr = vtkKWEPaintbrushLabelData::NoLabelValue;
          }
        removed |= remove;
        ++ptr;
        }
      }
    }

  if (removed)
    {
    this->LabelMap->Modified();
    this->Modified();
    }

  return (removed ? 1 : 0);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushLabelData::IsInside(double p[3])
{
  int pixelPos[3], extent[6];
  double origin[3], spacing[3];
  this->LabelMap->GetSpacing(spacing);
  this->LabelMap->GetOrigin(origin);
  this->LabelMap->GetExtent(extent);

  for (int i = 0; i < 3; i++)
    {
    pixelPos[i] = static_cast<int>(((p[i] - origin[i])/spacing[i])+0.5);
    if (pixelPos[i] < extent[2*i] || pixelPos[i] > extent[2*i+1])
      {
      return 0;
      }
    }

  return static_cast<int>(*(static_cast<vtkKWEPaintbrushEnums::LabelType *>
                (this->LabelMap->GetScalarPointer(pixelPos))));
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetSpacing( double s[3] )
{
  this->LabelMap->SetSpacing(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::GetSpacing( double s[3] )
{
  this->LabelMap->GetSpacing(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetOrigin( double s[3] )
{
  this->LabelMap->SetOrigin(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::GetOrigin( double s[3] )
{
  this->LabelMap->GetOrigin(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::GetPaintbrushDataAsImageData(vtkImageData *image)
{
  image->ShallowCopy(this->LabelMap);
}

//----------------------------------------------------------------------------
unsigned long vtkKWEPaintbrushLabelData::GetMTime()
{
  unsigned long t = this->LabelMap->GetMTime();
  unsigned long mtime = vtkObject::GetMTime();
  return (mtime > t ? mtime : t);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetNoLabelValue( 
                  vtkKWEPaintbrushEnums::LabelType label )
{
  vtkKWEPaintbrushLabelData::NoLabelValue = label;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::SetMutable( 
    int isMutable, vtkKWEPaintbrushEnums::LabelType label )
{
  if (isMutable)
    {
    this->ImmutableLabels.insert(label);
    }
  else
    {
    this->ImmutableLabels.erase(label);
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushLabelData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "LabelMap:\n";
  if (this->LabelMap)
    {
    this->LabelMap->PrintSelf(os,indent.GetNextIndent());
    int extent[6];
    this->GetExtent(extent);

    os << indent << "Extent: (" 
       << extent[0] << ", "
       << extent[1] << ", "
       << extent[2] << ", "
       << extent[3] << ", "
       << extent[4] << ", "
       << extent[5] << ")\n";

    double spacing[3], origin[3];
    this->GetSpacing(spacing);
    this->GetOrigin(origin);
    os << indent << "Spacing: (" 
       << spacing[0] << ", "
       << spacing[1] << ", "
       << spacing[2] << ")\n";

    os << indent << "Origin: (" 
       << origin[0] << ", "
       << origin[1] << ", "
       << origin[2] << ")\n";
    }
}

