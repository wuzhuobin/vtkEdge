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
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushUtilities, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushUtilities);

vtkKWEPaintbrushUtilities::vtkKWEPaintbrushUtilities()
{
}

vtkKWEPaintbrushUtilities::~vtkKWEPaintbrushUtilities()
{
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushUtilities::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushUtilities::GetIntersectingExtents( int extent1[6],
                                                    int extent2[6],
                                                    int extent[6])
{
  if ((extent1[0] > extent2[1]) || (extent1[1] < extent2[0]) ||
      (extent1[2] > extent2[3]) || (extent1[3] < extent2[2]) ||
      (extent1[4] > extent2[5]) || (extent1[5] < extent2[4]))
    {
    // The extents don't intersect
    extent[0] = extent[1] = extent[2] = extent[3]
              = extent[4] = extent[5] = -1;
    return 0;
    }

  extent[0] = (extent1[0] < extent2[0]) ? extent2[0] : extent1[0];
  extent[1] = (extent1[1] > extent2[1]) ? extent2[1] : extent1[1];
  extent[2] = (extent1[2] < extent2[2]) ? extent2[2] : extent1[2];
  extent[3] = (extent1[3] > extent2[3]) ? extent2[3] : extent1[3];
  extent[4] = (extent1[4] < extent2[4]) ? extent2[4] : extent1[4];
  extent[5] = (extent1[5] > extent2[5]) ? extent2[5] : extent1[5];
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushUtilities::GetImageFromStencil(
                          vtkImageData *image,
                          vtkImageStencilData *stencilData,
                          unsigned char inVal, unsigned char outVal,
                          bool useImageExtent )
{
  int extent[6];
  stencilData->GetExtent( extent );
  if (!useImageExtent)
    {
    // We will take care of initializing the image.
    image->SetExtent( extent );
    image->SetScalarTypeToUnsignedChar();
    image->SetNumberOfScalarComponents(1);
    image->AllocateScalars();
    }
  else
    {
    // extent = intersection of ImageExtent and StencilExtent.
    int imageExtent[6];
    image->GetExtent(imageExtent);
    extent[0] = (extent[0] < imageExtent[0] ? imageExtent[0] : extent[0]);
    extent[1] = (extent[1] > imageExtent[1] ? imageExtent[1] : extent[1]);
    extent[2] = (extent[2] < imageExtent[2] ? imageExtent[2] : extent[2]);
    extent[3] = (extent[3] > imageExtent[3] ? imageExtent[3] : extent[3]);
    extent[4] = (extent[4] < imageExtent[4] ? imageExtent[4] : extent[4]);
    extent[5] = (extent[5] > imageExtent[5] ? imageExtent[5] : extent[5]);
    }

  // Fill image with zeroes

  vtkImageIterator< unsigned char > it(image, image->GetExtent());
  while( !it.IsAtEnd() )
    {
    unsigned char *inSI    = it.BeginSpan();
    unsigned char *inSIEnd = it.EndSpan();
    while (inSI != inSIEnd)
      {
      *inSI = outVal;
      ++inSI;
      }
    it.NextSpan();
    }

  // Now populate image with inVal wherever the stencil exists.

  int r1, r2, moreSubExtents, iter;
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
          unsigned char *beginExtent =
            static_cast<unsigned char *>(image->GetScalarPointer(r1, y, z));
          unsigned char *endExtent   =
            static_cast<unsigned char *>(image->GetScalarPointer(r2, y, z));
          while (beginExtent <= endExtent)
            {
            *beginExtent = inVal;
            ++beginExtent;
            }
          }
        } // end for each extent tuple
      } // end for each scan line
    } // end of each slice

  image->Modified();
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushUtilities
::ExtentIsEqualToExtent( int extent1[6], int extent2[6] )
{
  if (!extent1 || !extent2)
    {
    return 0;
    }

  return (extent1[0] == extent2[0] &&
          extent1[1] == extent2[1] &&
          extent1[2] == extent2[2] &&
          extent1[3] == extent2[3] &&
          extent1[4] == extent2[4] &&
          extent1[5] == extent2[5]) ? 1 : 0;
}


