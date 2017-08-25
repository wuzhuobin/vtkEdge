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

#include "vtkKWEITKFilterModuleStencilOutput.h"

namespace vtkitk
{

template < class TFilterType >
FilterModuleStencilOutput< TFilterType >::FilterModuleStencilOutput()
{
}

template < class TFilterType >
FilterModuleStencilOutput< TFilterType >::~FilterModuleStencilOutput()
{
}

template < class TFilterType >
int FilterModuleStencilOutput< TFilterType >
::GetOutputAsStencil( vtkImageStencilData * stencilData )
{
  if ( this->m_ExportFilter->GetOutput()->GetNumberOfScalarComponents() != 1
       || this->m_ExportFilter->GetOutput()->GetScalarType()
            != VTK_UNSIGNED_CHAR )
    {
    return 0;
    }

  // Convert the output to a stencil.

  vtkImageData * image = this->m_ExportFilter->GetOutput();

  int extent[6];
  double spacing[3], origin[3];
  image->GetExtent(extent);
  image->GetSpacing(spacing);
  image->GetOrigin(origin);

  // Allocate stencil extents
  stencilData->SetExtent(extent);
  stencilData->SetSpacing(spacing);
  stencilData->SetOrigin(origin);
  stencilData->AllocateExtents();

  vtkImageIterator< unsigned char > it(image, extent);

  int ends[2], index[3];
  index[1] = extent[2];
  index[2] = extent[4];

  while( !it.IsAtEnd() )
    {
    unsigned char *inSI    = it.BeginSpan();
    unsigned char *inSIEnd = it.EndSpan();

    index[0] = extent[0];

    // for each row

    while ( inSI < inSIEnd )
      {
      // Skip until find a valid pixel
      while( inSI < inSIEnd && *inSI == 0)
        {
        ++index[0];
        ++inSI;
        }

      if( inSI < inSIEnd )
        {
        ends[0] = index[0];   // first valid pixel

        ++index[0];
        ++inSI;

        // Skip until find an invalid pixel
        while( inSI < inSIEnd && *inSI != 0)
          {
          ++index[0];
          ++inSI;
          }

        // Only use valid pixels
        ends[1] = index[0]-1;
        stencilData->InsertNextExtent(ends[0], ends[1], index[1], index[2]);

        if( inSI < inSIEnd )
          {
          ++index[0];
          ++inSI;
          }
        }
      }

    it.NextSpan();

    if (index[1] == extent[3])
      {
      ++index[2];
      index[1] = extent[2];
      }
    else
      {
      ++index[1];
      }
    }

  return 1;
}

template < class TFilterType >
int FilterModuleStencilOutput< TFilterType >
::GetSphereBoundedOutputAsStencil( vtkImageStencilData * stencilData,
                                   int center[3], double radius[3] )
{
  if ( this->m_ExportFilter->GetOutput()->GetNumberOfScalarComponents() != 1
       || this->m_ExportFilter->GetOutput()->GetScalarType()
            != VTK_UNSIGNED_CHAR )
    {
    return 0;
    }

  // Convert the output to a stencil.

  vtkImageData * image = this->m_ExportFilter->GetOutput();

  int extent[6];
  double spacing[3], origin[3];
  image->GetExtent(extent);
  image->GetSpacing(spacing);
  image->GetOrigin(origin);

  // Allocate stencil extents
  stencilData->SetExtent(extent);
  stencilData->SetSpacing(spacing);
  stencilData->SetOrigin(origin);
  stencilData->AllocateExtents();

  vtkImageIterator< unsigned char > it(image, extent);

  int ends[2], index[3];
  index[1] = extent[2];
  index[2] = extent[4];

  double lengthSq[3];
  // BUG: Center of extent is not necessarily the brush center.
  //   At edges, extent is clipped.

  lengthSq[1] = (double)((index[1]-center[1]) * (index[1]-center[1]))
                        /(radius[1] * radius[1]);
  lengthSq[2] = (double)((index[2]-center[2]) * (index[2]-center[2]))
                        /(radius[2] * radius[2]);

  while( !it.IsAtEnd() )
    {
    unsigned char *inSI    = it.BeginSpan();
    unsigned char *inSIEnd = it.EndSpan();

    index[0] = extent[0];

    // for each row

    double len2;
    while ( inSI < inSIEnd )
      {
      lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                             /(radius[0] * radius[0]);
      len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);

      // Skip until inside the circle
      while( inSI < inSIEnd && len2 > 1.0 )
        {
        ++index[0];
        ++inSI;
        lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                               /(radius[0] * radius[0]);
        len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);
        }

      // Skip until find a valid pixel
      while( inSI < inSIEnd && *inSI == 0 && len2 <= 1.0 )
        {
        ++index[0];
        ++inSI;
        lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                               /(radius[0] * radius[0]);
        len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);
        }

      if( inSI < inSIEnd && len2 <= 1.0 )
        {
        ends[0] = index[0];   // first valid pixel

        ++index[0];
        ++inSI;
        lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                               /(radius[0] * radius[0]);
        len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);

        // Skip until find an invalid pixel
        while( inSI < inSIEnd && *inSI != 0 && len2 <= 1.0 )
          {
          ++index[0];
          ++inSI;
          lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                                 /(radius[0] * radius[0]);
          len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);
          }

        // Only use valid pixels
        ends[1] = index[0]-1;
        stencilData->InsertNextExtent(ends[0], ends[1], index[1], index[2]);

        if( inSI < inSIEnd && len2 <= 1.0 )
          {
          ++index[0];
          ++inSI;
          lengthSq[0] = (double)((index[0]-center[0]) * (index[0]-center[0]))
                                 /(radius[0] * radius[0]);
          len2 = (lengthSq[0]+lengthSq[1]+lengthSq[2]);
          }
        }
      }

    it.NextSpan();

    if (index[1] == extent[3])
      {
      ++index[2];
      lengthSq[2] = (double)((index[2]-center[2]) * (index[2]-center[2]))
                            /(radius[2]*radius[2]);
      index[1] = extent[2];
      lengthSq[1] = (double)((index[1]-center[1]) * (index[1]-center[1]))
                            /(radius[1]*radius[1]);
      }
    else
      {
      ++index[1];
      lengthSq[1] = (double)((index[1]-center[1]) * (index[1]-center[1]))
                            /(radius[1]*radius[1]);
      }
    }

  return 1;
}


} // end namespace vtkitk

