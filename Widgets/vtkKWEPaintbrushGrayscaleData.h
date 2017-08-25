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

#ifndef __vtkKWEPaintbrushGrayscaleData_h
#define __vtkKWEPaintbrushGrayscaleData_h

#include "vtkKWEPaintbrushData.h"
#include "VTKEdgeConfigure.h" // Needed for export symbols directives

class vtkImageData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushGrayscaleData : public vtkKWEPaintbrushData
{
public:
  static vtkKWEPaintbrushGrayscaleData *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushGrayscaleData, vtkKWEPaintbrushData);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DeepCopy(vtkDataObject *o);
  void ShallowCopy(vtkDataObject *f);

  // Description:
  // Set/Get the image data externally.. usually the image data is created
  // internally during every stroke.. This method allows you to load an
  // external segmentation and initialize the PaintbrushData with that
  // segmentation.
  virtual void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Description:
  // These operators are minkowski Add/Subtract operators. The Add operator
  // implements a pixelwise max function. The subtract operator is a
  // pixelwise min function.
  virtual int  Add(      vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Subtract( vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Replace( vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Add(      vtkImageData *, bool forceMutable=false );
  virtual int  Subtract( vtkImageData *, bool forceMutable=false );
  virtual int  Replace( vtkImageData *, bool forceMutable=false );

  // Description:
  // Clip self with supplied extents. Return 1 if something changed
  virtual int Clip( int extent[6] );

  // Description:
  // Allocate the grayscale image. Destroys existing data, if any
  virtual void Allocate( double fillValue = 0.0 );

  // Description:
  // Resize. Unlike allocate, this will allocate to conform to the new
  // extents, while preserving existing data. If you are calling
  // Resize with the extents for the first time, this is the same as
  // calling
  //   SetExtent(..) followed by Allocate(..).
  virtual void Resize( int extent[6], double fillValue = 0.0 );

  virtual void SetSpacing(double spacing[3]);
  virtual void SetOrigin( double origin[3] );

  // Description:
  // Set/Get extents. This will be the same
  virtual void SetExtent( int extent[6] );
  virtual void GetExtent( int extent[6] );

  // Description:
  // You can set a certain value in the grayscale data as the outside value.
  // Pixels with a value equal (within the specified tolerance) to the
  // OutsideValue will not affect the image when adding or subtract.
  vtkSetMacro(OutsideValue, double);
  vtkGetMacro(OutsideValue, double);
  vtkSetMacro(OutsideValueTolerance, double);
  vtkGetMacro(OutsideValueTolerance, double);

  // Description:
  // Is the point "p" inside ?
  virtual int IsInside( double p[3] );

  // Description:
  // Get the paintbrush data as an image data.. Note.. shallow copy here of
  // the existing image data.
  virtual void GetPaintbrushDataAsImageData( vtkImageData * );

  // Description:
  // See vtkObject for doc
  virtual unsigned long GetMTime();

protected:
  vtkKWEPaintbrushGrayscaleData();
  ~vtkKWEPaintbrushGrayscaleData();

  // Description:
  // Stores the bulk data
  vtkImageData *ImageData;

  double OutsideValue;
  double OutsideValueTolerance;

private:
  vtkKWEPaintbrushGrayscaleData(const vtkKWEPaintbrushGrayscaleData&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushGrayscaleData&);  // Not implemented.
};

#endif

