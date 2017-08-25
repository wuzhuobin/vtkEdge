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
// .NAME vtkKWEPaintbrushStencilData - An abstract class used to support segmentations
// .SECTION Description
//
// .SECTION see also
// vtkImageStencilSource vtkImageStencil

#ifndef __vtkKWEPaintbrushStencilData_h
#define __vtkKWEPaintbrushStencilData_h

#include "vtkKWEPaintbrushData.h"

class vtkImageStencilData;
class vtkImageData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushStencilData
                                 : public vtkKWEPaintbrushData
{
public:
  static vtkKWEPaintbrushStencilData *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushStencilData, vtkKWEPaintbrushData);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DeepCopy(vtkDataObject *o);
  void ShallowCopy(vtkDataObject *f);

  // Description:
  virtual void SetImageStencilData( vtkImageStencilData * );
  vtkGetObjectMacro( ImageStencilData, vtkImageStencilData );

  // Description:
  // Minkowski operations
  virtual int  Add(      vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Subtract( vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Replace( vtkKWEPaintbrushData *, bool forceMutable=false );

  virtual int  Add(      vtkImageStencilData *, bool forceMutable=false );
  virtual int  Subtract( vtkImageStencilData *, bool forceMutable=false );
  virtual int  Replace( vtkImageStencilData *, bool forceMutable=false );

  // Description:
  // Clip self with supplied extents. Return 1 if something changed
  virtual int Clip( int extent[6] );

  // Description:
  // Allocate and fill. This will wipe out any existing data.
  virtual void Allocate(double fillValue = 0.0 );

  // Description:
  // Resize. Unlike allocate, this will allocate to conform to the new
  // extents, while preserving existing data. If you are calling
  // Resize with the extents for the first time, this is the same as
  // calling
  //   SetExtent(..) followed by Allocate(..).
  virtual void Resize( int extent[6], double fillValue = 0.0 );

  // Description:
  // Set the metadata.
  virtual void SetSpacing(double spacing[3]);
  virtual void SetOrigin( double origin[3] );

  // Description:
  // Set/Get extents.
  virtual void SetExtent( int extent[6] );
  virtual void GetExtent( int extent[6] );

  // Description:
  // Is the point "p" inside ?
  virtual int IsInside( double p[3] );

  // Description:
  // Get the binary brush stencil data as an image data
  virtual void GetPaintbrushDataAsImageData( vtkImageData * );

  // Description:
  // See vtkObject for doc
  virtual unsigned long GetMTime();

protected:
  vtkKWEPaintbrushStencilData();
  ~vtkKWEPaintbrushStencilData();

  vtkImageStencilData          * ImageStencilData;

private:
  vtkKWEPaintbrushStencilData(const vtkKWEPaintbrushStencilData&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushStencilData&);  // Not implemented.
};

#endif

