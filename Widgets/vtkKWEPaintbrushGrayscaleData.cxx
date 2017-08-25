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
#include "vtkKWEPaintbrushGrayscaleData.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkImageIterator.h"
#include "vtkImageData.h"
#include "vtkKWEPaintbrushUtilities.h"

#include <math.h>

#ifndef max
#define max(x,y) ((x>y) ? (x) : (y))
#endif
#ifndef min
#define min(x,y) ((x<y) ? (x) : (y))
#endif

vtkCxxRevisionMacro(vtkKWEPaintbrushGrayscaleData, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushGrayscaleData);
vtkCxxSetObjectMacro(vtkKWEPaintbrushGrayscaleData, ImageData, vtkImageData);

//----------------------------------------------------------------------------
vtkKWEPaintbrushGrayscaleData::vtkKWEPaintbrushGrayscaleData()
{
  this->ImageData = vtkImageData::New();
  this->ImageData->SetScalarTypeToUnsignedChar();
  this->Information->Set(vtkDataObject::DATA_EXTENT_TYPE(), VTK_3D_EXTENT);
  this->Information->Set(vtkDataObject::DATA_EXTENT(),
                         this->ImageData->GetExtent(), 6);

  this->OutsideValue           = 0.0;
  this->OutsideValueTolerance = 0.01;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushGrayscaleData::~vtkKWEPaintbrushGrayscaleData()
{
  this->SetImageData(NULL);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ImageData: " << this->ImageData << "\n";
  if (this->ImageData)
    {
    this->ImageData->PrintSelf(os,indent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::ShallowCopy(vtkDataObject *o)
{
  vtkKWEPaintbrushGrayscaleData *s=
    vtkKWEPaintbrushGrayscaleData::SafeDownCast(o);

  if (s)
    {
    this->ImageData->ShallowCopy(s->GetImageData());
    }

  vtkDataObject::ShallowCopy(o);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::DeepCopy(vtkDataObject *o)
{
  vtkKWEPaintbrushGrayscaleData *s=
    vtkKWEPaintbrushGrayscaleData::SafeDownCast(o);

  if (s)
    {
    this->ImageData->DeepCopy(s->GetImageData());
    }

  vtkDataObject::DeepCopy(o);
}

//----------------------------------------------------------------------------
// Returns 0 if nothing was added
// Implements minkowski add and subtract operators (max and min)
template< class T1, class T2 >
int vtkKWEPaintbrushGrayscaleDataAdd( vtkKWEPaintbrushGrayscaleData * self,
                                        vtkImageData *s, T1, T2 )
{
  const double outsideValue = self->GetOutsideValue();
  const double outsideValueTolerance = self->GetOutsideValueTolerance();

  // Not using vtkImageMathematics there cause we want to use self as the
  // output and do a quick inplace add.
  int extent[6];
  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents(
        self->GetImageData()->GetExtent(), s->GetExtent(), extent))
    {
    return 0;
    }

  vtkImageIterator< T1 > it1(self->GetImageData(), extent);
  vtkImageIterator< T2 > it2(s, extent);

  while( !it2.IsAtEnd() )
    {
    T1 *inSI    = it1.BeginSpan();
    T1 *inSIEnd = it1.EndSpan();
    T2 *inSI2   = it2.BeginSpan();
    while (inSI != inSIEnd)
      {
      if ((static_cast< double >(*inSI2) - outsideValue)
                                  > outsideValueTolerance)
        {
        *inSI = max(static_cast< T1 >(*inSI2), static_cast< T1 >(*inSI));
        }
      ++inSI;
      ++inSI2;
      }
    it1.NextSpan();
    it2.NextSpan();
    }

  self->GetImageData()->Modified();
  self->Modified();

  return 1;
}

//----------------------------------------------------------------------------
// Returns 0 if nothing was added
template< class T1, class T2 >
int vtkKWEPaintbrushGrayscaleDataSubtract(vtkKWEPaintbrushGrayscaleData *self,
                                          vtkImageData *s, T1, T2 )
{
  const double outsideValue = self->GetOutsideValue();
  const double outsideValueTolerance = self->GetOutsideValueTolerance();

  // Not using vtkImageMathematics there cause we want to use self as the
  // output and do a quick inplace add.
  int extent[6];
  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents(
        self->GetImageData()->GetExtent(), s->GetExtent(), extent))
    {
    return 0;
    }

  vtkImageIterator< T1 > it1(self->GetImageData(), extent);
  vtkImageIterator< T2 > it2(s, extent);

  while( !it1.IsAtEnd() )
    {
    T1 *inSI    = it1.BeginSpan();
    T1 *inSIEnd = it1.EndSpan();
    T2 *inSI2 = it2.BeginSpan();
    while (inSI != inSIEnd)
      {
      if ((static_cast< double >(*inSI2) - outsideValue)
                                  > outsideValueTolerance)
        {
        *inSI = min(static_cast< T1 >(*inSI2), static_cast< T1 >(*inSI));
        }
      ++inSI;
      ++inSI2;
      }
    it1.NextSpan();
    it2.NextSpan();
    }

  self->GetImageData()->Modified();
  self->Modified();

  return 1;
}

//----------------------------------------------------------------------------
// Returns 0 if nothing was added
template< class T1, class T2 >
int vtkKWEPaintbrushGrayscaleDataReplace(vtkKWEPaintbrushGrayscaleData *self,
                                          vtkImageData *s, T1, T2 )
{
  const double outsideValue = self->GetOutsideValue();
  const double outsideValueTolerance = self->GetOutsideValueTolerance();

  // Not using vtkImageMathematics there cause we want to use self as the
  // output and do a quick inplace add.
  int extent[6];
  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents(
        self->GetImageData()->GetExtent(), s->GetExtent(), extent))
    {
    return 0;
    }

  vtkImageIterator< T1 > it1(self->GetImageData(), extent);
  vtkImageIterator< T2 > it2(s, extent);

  while( !it1.IsAtEnd() )
    {
    T1 *inSI    = it1.BeginSpan();
    T1 *inSIEnd = it1.EndSpan();
    T2 *inSI2 = it2.BeginSpan();
    while (inSI != inSIEnd)
      {
      if ((static_cast< double >(*inSI2) - outsideValue)
                                  > outsideValueTolerance)
        {
        *inSI = static_cast< T1 >(*inSI2);
        }
      ++inSI;
      ++inSI2;
      }
    it1.NextSpan();
    it2.NextSpan();
    }

  self->GetImageData()->Modified();
  self->Modified();

  return 1;
}

//----------------------------------------------------------------------------
template< class T1 >
int vtkKWEPaintbrushGrayscaleDataOper(vtkKWEPaintbrushGrayscaleData *self,
                                      vtkImageData *s, T1, int oper)
{
 int ret = 0;
 if (oper == 0) // add
   {
   switch (s->GetScalarType())
     {
     vtkTemplateMacro(
       ret=vtkKWEPaintbrushGrayscaleDataAdd(self,s,
                                            static_cast<T1>(0),
                                            static_cast< VTK_TT >(0) ));
     }
   }
 else if (oper == 1) // subtract
   {
   switch (s->GetScalarType())
     {
     vtkTemplateMacro(
       ret=vtkKWEPaintbrushGrayscaleDataSubtract(self,s,
                                                 static_cast<T1>(0),
                                                 static_cast< VTK_TT >(0) ));
     }
   }
 else if (oper == 2) // replace
   {
   switch (s->GetScalarType())
     {
     vtkTemplateMacro(
       ret=vtkKWEPaintbrushGrayscaleDataReplace(self,s,
                                                 static_cast<T1>(0),
                                                 static_cast< VTK_TT >(0) ));
     }
   }

 return ret;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Add(vtkImageData *s,
                                       bool vtkNotUsed(forceMutable))
{
  int ret = 0;
  switch(this->ImageData->GetScalarType())
    {
    vtkTemplateMacro( ret = vtkKWEPaintbrushGrayscaleDataOper(this,s,
          static_cast<VTK_TT>(0), 0 ));
     default:
       {
       vtkErrorMacro(<< "vtkKWEPaintbrushGrayscaleData: Unknown ScalarType");
       return 0;
       }
    }
  return ret;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Subtract(vtkImageData *s,
                                            bool vtkNotUsed(forceMutable))
{
  int ret = 0;
  switch(this->ImageData->GetScalarType())
    {
    vtkTemplateMacro( ret = vtkKWEPaintbrushGrayscaleDataOper( this, s,
          static_cast< VTK_TT >(0), 1 ));
     default:
       {
       vtkErrorMacro(<< "vtkKWEPaintbrushGrayscaleData: Unknown ScalarType");
       return 0;
       }
    }
  return ret;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Replace(vtkImageData *s,
                                            bool vtkNotUsed(forceMutable))
{
  int ret = 0;
  switch(this->ImageData->GetScalarType())
    {
    vtkTemplateMacro( ret = vtkKWEPaintbrushGrayscaleDataOper( this, s,
          static_cast< VTK_TT >(0), 2 ));
     default:
       {
       vtkErrorMacro(<< "vtkKWEPaintbrushGrayscaleData: Unknown ScalarType");
       return 0;
       }
    }
  return ret;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Add(vtkKWEPaintbrushData *d,
                                       bool forceMutable)
{
  vtkKWEPaintbrushGrayscaleData *s=
    vtkKWEPaintbrushGrayscaleData::SafeDownCast(d);
  if (s)
    {
    return this->Add(s->GetImageData(), forceMutable);
    }
  return 0;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Subtract(vtkKWEPaintbrushData *d,
                                            bool forceMutable)
{
  vtkKWEPaintbrushGrayscaleData *s=
    vtkKWEPaintbrushGrayscaleData::SafeDownCast(d);
  if (s)
    {
    return this->Subtract(s->GetImageData(), forceMutable);
    }
  return 0;
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Replace(vtkKWEPaintbrushData *d,
                                            bool forceMutable)
{
  vtkKWEPaintbrushGrayscaleData *s=
    vtkKWEPaintbrushGrayscaleData::SafeDownCast(d);
  if (s)
    {
    return this->Replace(s->GetImageData(), forceMutable);
    }
  return 0;
}

//----------------------------------------------------------------------------
template< class T > void vtkKWEPaintbrushGrayscaleDataFillBufferInside(
                                  vtkImageData *image, T fillValue )
{
  vtkImageIterator< T > it(image, image->GetExtent());
  while( !it.IsAtEnd() )
    {
    T *inSI    = it.BeginSpan();
    T *inSIEnd = it.EndSpan();
    while (inSI != inSIEnd)
      {
      *inSI = static_cast< T >(fillValue);
      ++inSI;
      }
    it.NextSpan();
    }
}

//----------------------------------------------------------------------------
template< class T > void vtkKWEPaintbrushGrayscaleDataFillBufferOutside(
                    vtkImageData *image, int extent[6], T fillValue )
{
  int imageExtent[6];
  image->GetExtent(imageExtent);
  int y = imageExtent[2], z = imageExtent[4], e[6];

  e[0] = (extent[0] < imageExtent[0] ? imageExtent[0] : extent[0]);
  e[1] = (extent[1] > imageExtent[1] ? imageExtent[1] : extent[1]);
  e[2] = (extent[2] < imageExtent[2] ? imageExtent[2] : extent[2]);
  e[3] = (extent[3] > imageExtent[3] ? imageExtent[3] : extent[3]);
  e[4] = (extent[4] < imageExtent[4] ? imageExtent[4] : extent[4]);
  e[5] = (extent[5] > imageExtent[5] ? imageExtent[5] : extent[5]);

  // Handle 4 cases with extent clippers
  //
  // Image                      -------------------
  // ClipExtent Case 0                -------
  // ClipExtent Case 1      ------------
  // ClipExtent Case 2                    --------------
  // ClipExtent Case 3      ---------------------------

  int type  = (extent[0] <= imageExtent[0]) ? 0x01 : 0x00;
      type |= (extent[1] >= imageExtent[1]) ? 0x10 : 0x00;

  int incr0 = 0;
  int incr1 = 0;
  switch (type)
    {
    case 0:
      incr0 = extent[0] - imageExtent[0] + 1;
      incr1 = imageExtent[1] - extent[0];
      break;
    case 1:
      incr0 = extent[1] - imageExtent[0] + 1;
      break;
    case 2:
      incr0 = extent[0] - imageExtent[0];
      break;
    }

  vtkImageIterator< T > it(image, e);
  while( !it.IsAtEnd() )
    {
    T *inSI    = it.BeginSpan();
    T *inSIEnd = it.EndSpan();

    if (z < extent[4] || z > extent[5] || y < extent[2] || y > extent[3])
      {
      while (inSI != inSIEnd)
        {
        *inSI = static_cast< T >(fillValue);
        ++inSI;
        }
      }

    else if (type == 0)
      {
      T* inSIEnd0 = inSI + incr0;
      T* inSIBegin1 = inSI + incr1;
      while (inSI != inSIEnd0)
        {
        *inSI = static_cast< T >(fillValue);
        ++inSI;
        }
      while (inSIBegin1 != inSIEnd)
        {
        *inSIBegin1 = static_cast< T >(fillValue);
        ++inSIBegin1;
        }
      }

    else if (type == 1)
      {
      inSI += incr0;
      while (inSI != inSIEnd)
        {
        *inSI = static_cast< T >(fillValue);
        ++inSI;
        }
      }

    else if (type == 2)
      {
      inSIEnd = inSI + incr0;
      while (inSI != inSIEnd)
        {
        *inSI = static_cast< T >(fillValue);
        ++inSI;
        }
      }

    it.NextSpan();
    if (y == extent[3])
      {
      ++z;
      y = extent[2];
      }
    else
      {
      ++y;
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::Allocate(double fillValue)
{
  this->ImageData->AllocateScalars();

  switch(this->ImageData->GetScalarType())
    {
    vtkTemplateMacro(
      vtkKWEPaintbrushGrayscaleDataFillBufferInside(this->ImageData,
                                                    static_cast< VTK_TT >(fillValue) ));
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::SetSpacing( double s[3] )
{
  this->ImageData->SetSpacing(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::SetOrigin( double s[3] )
{
  this->ImageData->SetOrigin(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::SetExtent( int extent[6] )
{
  this->ImageData->SetExtent(extent);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::GetExtent( int extent[6] )
{
  this->ImageData->GetExtent(extent);
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::Clip( int extent[6] )
{
  // This will not re-allocate extents. In other words, this is not a means
  // of saving memory. It will simply set to 0, any values in the image
  // outside the extents.

  int intersectingExtent[6], currentExtent[6];
  this->ImageData->GetExtent( currentExtent );

  if (vtkMath::ExtentIsWithinOtherExtent( currentExtent, extent ))
    {
    return 0;
    }

  if (!vtkKWEPaintbrushUtilities::GetIntersectingExtents(
          currentExtent, extent, intersectingExtent ))
    {
    // Nothing is inside.. blank image..
    switch (this->ImageData->GetScalarType())
      {
      vtkTemplateMacro( vtkKWEPaintbrushGrayscaleDataFillBufferInside(
            this->ImageData, static_cast< VTK_TT >(this->OutsideValue) ));
      }
    }
  else
    {
    // Nothing is inside.. blank image..
    switch (this->ImageData->GetScalarType())
      {
      vtkTemplateMacro( vtkKWEPaintbrushGrayscaleDataFillBufferOutside(
        this->ImageData, extent, static_cast< VTK_TT >(this->OutsideValue)));
      }
    }

  return 1;
}

//----------------------------------------------------------------------------
template < class T > void vtkKWEPaintbrushGrayscaleDataGetValue(
  int coords[3],
  double &value,
  vtkImageData *image,
  T )
{
  value = static_cast<double>(
    *(static_cast<T *>(image->GetScalarPointer(coords))));
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushGrayscaleData::IsInside( double p[3] )
{
  // Check if point "p" (p is in world coordinates) is inside the painting.

  double pcoords[3];
  double value = 0;
  int ijk[3];
  if (this->ImageData->ComputeStructuredCoordinates( p, ijk, pcoords ))
    {
    switch (this->ImageData->GetScalarType())
      {
      vtkTemplateMacro( vtkKWEPaintbrushGrayscaleDataGetValue(
                      ijk, value, this->ImageData, static_cast<VTK_TT>(0) ));
      }
    return (fabs(value - this->OutsideValue)
              < this->OutsideValueTolerance ? 0 : 1);
    }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long vtkKWEPaintbrushGrayscaleData::GetMTime()
{
  unsigned long t = this->ImageData->GetMTime();
  unsigned long mtime = vtkObject::GetMTime();
  return (mtime > t ? mtime : t);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::GetPaintbrushDataAsImageData(
                                        vtkImageData *image)
{
  image->ShallowCopy(this->ImageData);
}

//----------------------------------------------------------------------------
template< class T > void vtkKWEPaintbrushGrayscaleDataResize(
    vtkImageData *imageResized, vtkImageData *imageOld, int oldExtent[6], T )
{
  vtkImageIterator< T > it1(imageResized, oldExtent);
  vtkImageIterator< T > it2(imageOld, oldExtent);

  while( !it2.IsAtEnd() )
    {
    T *inSI    = it1.BeginSpan();
    T *inSIEnd = it1.EndSpan();
    T *inSI2   = it2.BeginSpan();
    while (inSI != inSIEnd)
      {
      *inSI = *inSI2;
      ++inSI;
      ++inSI2;
      }
    it1.NextSpan();
    it2.NextSpan();
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushGrayscaleData::Resize(int extent[6], double f)
{
  int oldExtent[6];
  this->ImageData->GetExtent(oldExtent);
  if (oldExtent[1] < oldExtent[0])
    {
    // We haven't been allocated yet. Just allocate and return.
    this->SetExtent(extent);
    this->Allocate(f);
    return;
    }

  if (oldExtent[0] != extent[0] ||
      oldExtent[1] != extent[1] ||
      oldExtent[2] != extent[2] ||
      oldExtent[3] != extent[3] ||
      oldExtent[4] != extent[4] ||
      oldExtent[5] != extent[5])
    {
    vtkImageData * oldData = vtkImageData::New();
    oldData->DeepCopy(this->ImageData);

    this->SetExtent(extent);
    this->Allocate(f);

    switch (this->ImageData->GetScalarType())
      {
      vtkTemplateMacro( vtkKWEPaintbrushGrayscaleDataResize(
        this->ImageData, oldData, extent, static_cast<VTK_TT>(0) ));
      }

    oldData->Delete();
    }

  this->ImageData->Modified();
  this->Modified();
}


