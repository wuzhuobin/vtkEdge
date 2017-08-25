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
// .NAME vtkKWEPaintbrushUtilities - helper class to cram unrelated functions.
// .SECTION Description
// Helper class to cram unclassifiable functions..
//
// All methods here are of course static

#ifndef __vtkKWEPaintbrushUtilities_h
#define __vtkKWEPaintbrushUtilities_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkObject.h"
#include "vtkImageData.h"
#include "vtkImageStencilData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkImageIterator.h"
#include "vtkSmartPointer.h"
#include <vtkstd/map>
#include <vtkstd/vector>

class vtkImageData;
class vtkImageStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushUtilities: public vtkObject
{
public:
  static vtkKWEPaintbrushUtilities * New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushUtilities, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Returns 0 if the extents don't intersect, 1 if they do interesect
  // extent contains the intersection of extent1 and extent2.
  static int GetIntersectingExtents( int extent1[6], int extent2[6],
                                     int extent[6]);

  //BTX
  // Description:
  // These functors provide the ability to do boolean operations on input data
  // so as to convert imagedata to segmentations. They can be used to create
  // multiple stencils from a label-map, by saying
  //   vtkImageStencilData *s1 = vtkImageStencilData::New();
  //   vtkImageStencilData *s2 = vtkImageStencilData::New();
  //   vtkImageStencilData *s3 = vtkImageStencilData::New();
  //   GetStencilFromImage< vtkFunctorEqualTo >( labelMapImage, s1, 64.0 );
  //   GetStencilFromImage< vtkFunctorEqualTo >( labelMapImage, s2, 128.0 );
  //   GetStencilFromImage< vtkFunctorEqualTo >( labelMapImage, s3, 192.0 );

  class vtkFunctorEqualTo
    {
    public:
    inline bool operator()( const double &a, const double &b )
      { return (a == b); }
    };

  class vtkFunctorGreaterThanEqualTo
    {
    public:
    inline bool operator()( const double &a, const double &b )
      { return (a >= b); }
    };

  class vtkFunctorGreaterThan
    {
    public:
    inline bool operator()( const double &a, const double &b )
      { return (a > b); }
    };

  class vtkFunctorLessThan
    {
    public:
    inline bool operator()( const double &a, const double &b )
      { return (a < b); }
    };

  class vtkFunctorLessThanEqualTo
    {
    public:
    inline bool operator()( const double &a, const double &b )
      { return (a < b); }
    };

  template < class TFunctor, class T >
  static void vtkKWEPaintbrushUtilitiesGetStencilFromImage( vtkImageData *image,
                          vtkImageStencilData *stencilData, T threshold)
    {
    TFunctor f;

    int extent[6];
    double spacing[3], origin[3];
    image->GetExtent(extent);
    image->GetSpacing(spacing);
    image->GetOrigin(origin);

    stencilData->SetExtent(extent);
    stencilData->SetSpacing(spacing);
    stencilData->SetOrigin(origin);
    stencilData->AllocateExtents();

    vtkImageIterator< T > it(image, extent);

    int ends[2], index[3];
    index[1] = extent[2];
    index[2] = extent[4];

    while( !it.IsAtEnd() )
      {
      T *inSI    = it.BeginSpan();
      T *inSIEnd = it.EndSpan();

      index[0] = extent[0];
      ends[0] = -1;
      ends[1] = -1;
      if (f(static_cast<double>(*inSI), static_cast<double>(threshold)))
        {
        ends[0] = extent[0];
        }

      // for each row

      while (inSI != inSIEnd)
        {

        if (ends[0] == -1 &&
            f(static_cast<double>(*inSI), static_cast<double>(threshold)))
          {
          // look for start
          ends[0] = index[0];
          ++index[0];
          ++inSI;
          continue;
          }

        if (ends[0] != -1 && ends[1] == -1 &&
            !f(static_cast<double>(*inSI), static_cast<double>(threshold)))
          {
          ends[1] = index[0];
          stencilData->InsertNextExtent(ends[0], ends[1]-1, index[1], index[2]);
          ends[0] = ends[1] = -1;
          }

        ++index[0];
        ++inSI;
        }

      if (f(static_cast<double>(*(inSI-1)),
            static_cast<double>(threshold))) // ends[0] can't be -1, ends[1] has to be -1
        {
        stencilData->InsertNextExtent(ends[0], extent[1], index[1], index[2]);
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
    }

  template< class T >
  static vtkstd::map< vtkKWEPaintbrushEnums::LabelType,
                 vtkSmartPointer< vtkKWEPaintbrushStencilData > >
    vtkKWEPaintbrushUtilitiesGetStencilsFromImage(
        vtkImageData *image,
        vtkstd::vector< vtkKWEPaintbrushEnums::LabelType > labels,
        T )
    {
    int extent[6];
    double spacing[3], origin[3];
    image->GetExtent(extent);
    image->GetSpacing(spacing);
    image->GetOrigin(origin);

    vtkstd::map< vtkKWEPaintbrushEnums::LabelType,
                 vtkSmartPointer< vtkKWEPaintbrushStencilData > > pstrokeDatas;
    vtkstd::map< vtkKWEPaintbrushEnums::LabelType,
                 vtkSmartPointer< vtkImageStencilData > > strokeDatas;
    for (vtkstd::vector< vtkKWEPaintbrushEnums::LabelType >::const_iterator lit = labels.begin();
         lit != labels.end(); ++lit)
      {
      vtkKWEPaintbrushEnums::LabelType l = *lit;
      pstrokeDatas[l] = vtkSmartPointer< vtkKWEPaintbrushStencilData >::New();
      pstrokeDatas[l]->SetLabel(*lit);
      pstrokeDatas[l]->SetExtent(extent);
      pstrokeDatas[l]->SetSpacing(spacing);
      pstrokeDatas[l]->SetOrigin(origin);
      pstrokeDatas[l]->Allocate();
      strokeDatas[l] = pstrokeDatas[l]->GetImageStencilData();
      }

    vtkImageIterator< T > it(image, extent);

    int ends[2], index[3];
    index[1] = extent[2];
    index[2] = extent[4];
    vtkKWEPaintbrushEnums::LabelType label = vtkKWEPaintbrushLabelData::NoLabelValue;

    while( !it.IsAtEnd() )
      {
      T *inSI    = it.BeginSpan();
      T *inSIEnd = it.EndSpan();

      index[0] = extent[0];
      ends[0] = -1;
      ends[1] = -1;

      // for each row

      while (inSI != inSIEnd)
        {

        if (ends[0] == -1 && *inSI != vtkKWEPaintbrushLabelData::NoLabelValue)
          {
          // look for start
          ends[0] = index[0];
          ++index[0];
          label = static_cast< vtkKWEPaintbrushEnums::LabelType >(*inSI);
          ++inSI;
          continue;
          }

        if (ends[0] != -1 && ends[1] == -1 && *inSI != label)
          {
          ends[1] = index[0];
          strokeDatas[label]->InsertNextExtent(ends[0], ends[1]-1, index[1], index[2]);
          ends[0] = ends[1] = -1;

          if (*inSI != vtkKWEPaintbrushLabelData::NoLabelValue)
            {
            ends[0] = index[0];
            label = static_cast< vtkKWEPaintbrushEnums::LabelType >(*inSI);
            }
          }

        ++index[0];
        ++inSI;
        }

      if (ends[0] != -1 && ends[1] == -1) // ends[0] can't be -1, ends[1] has to be -1
        {
        strokeDatas[label]->InsertNextExtent(ends[0], extent[1], index[1], index[2]);
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

    return pstrokeDatas;
    }

  // Description:
  // Get a binary stencil from an image. All pixels <FUNCTOR> threshold are
  // considered within the stencil.
  //
  // TFunctor may be any of the above defined class templates or your own.
  // It may be any one of <= == >= > < operators.
  // It should be a templated class that will provide a mechanism of
  // comparing with the threshold and return a boolean. Common templates
  // for greater-than / less-than etc are already provided.
  template < class TFunctor >
  static void GetStencilFromImage( vtkImageData        * image,
                                   vtkImageStencilData * stencilData,
                                   double                threshold )
    {
    switch (image->GetScalarType())
      {
      vtkTemplateMacro( vtkKWEPaintbrushUtilitiesGetStencilFromImage< TFunctor >(
                image, stencilData, static_cast<VTK_TT>(threshold)));
      }
    }

  static vtkstd::map< vtkKWEPaintbrushEnums::LabelType,
                 vtkSmartPointer< vtkKWEPaintbrushStencilData > >
        GetStencilsFromImage( vtkImageData * image,
           vtkstd::vector< vtkKWEPaintbrushEnums::LabelType > labels)
    {
    typedef vtkstd::map< vtkKWEPaintbrushEnums::LabelType,
               vtkSmartPointer< vtkKWEPaintbrushStencilData > > StrokeToLabelMapType;
    StrokeToLabelMapType r;
    switch (image->GetScalarType())
      {
      vtkTemplateMacro( (r =
          vtkKWEPaintbrushUtilitiesGetStencilsFromImage(
                            image, labels, static_cast<VTK_TT>(0))));
      }
    return r;
    }

  // Description:
  // Populate a vtkImageData from a binary stencil.
  // "inVal" and "outVal" define the values assigned to the image inside and
  // outside the object. If "UseImageExtent" is true, the image extents are
  // preserved. If not, the image acquires its metadata from the stencil.
  static void GetImageFromStencil( vtkImageData *,
                                   vtkImageStencilData *,
                                   unsigned char inVal,
                                   unsigned char outVal,
                                   bool UseImageExtent = false );
  //ETX

  // Description:
  // Check if one extent is equal to the other extent. Returns 1 if true.
  static int ExtentIsEqualToExtent( int e1[6], int e2[6] );

protected:
  vtkKWEPaintbrushUtilities();
  ~vtkKWEPaintbrushUtilities();

private:
  vtkKWEPaintbrushUtilities(const vtkKWEPaintbrushUtilities &); // Not implemented
  void      operator=(const vtkKWEPaintbrushUtilities &); // Not implemented
};

#endif

