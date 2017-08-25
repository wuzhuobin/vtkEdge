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
// .NAME vtkKWEPaintbrushBlend - blend images together using alpha or opacity
// .SECTION Description

#ifndef __vtkKWEPaintbrushBlend_h
#define __vtkKWEPaintbrushBlend_h

#include "VTKEdgeConfigure.h"
#include "vtkThreadedImageAlgorithm.h"
#include <vtkstd/map>

class vtkImageStencilData;
class vtkKWEPaintbrushDrawing;
class vtkKWEPaintbrushBlendColors;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushBlend : public vtkThreadedImageAlgorithm
{
public:
  static vtkKWEPaintbrushBlend *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushBlend,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the opacity of an stencil.
  // Defaults to 0.5
  vtkSetClampMacro(Opacity,double,0.0,1.0);
  vtkGetMacro(Opacity,double);

  // Description:
  // Restrict the blending to be performed on a set of extents.. does not
  // care about stuff outside the extents.. this will work only if the input and
  // the output are of the same

  // Description:
  // Set stencils to apply when blending the data.
  virtual void SetPaintbrushDrawing(vtkKWEPaintbrushDrawing *);
  vtkKWEPaintbrushDrawing * GetPaintbrushDrawing();

  // Description:
  // Use an overlay ? Defaults to ON.
  // If ON, this class will produce an output image that represents a blending
  // of the input and the drawing, passed through a color lookup table.
  // If OFF, this class will produce an output image that is the drawing passed
  // through a color lookup table.
  vtkSetMacro( UseOverlay, int );
  vtkGetMacro( UseOverlay, int );
  vtkBooleanMacro( UseOverlay, int );

protected:
  vtkKWEPaintbrushBlend();
  ~vtkKWEPaintbrushBlend();

  virtual int RequestUpdateExtent(vtkInformation *,
                                  vtkInformationVector **,
                                  vtkInformationVector *);

  void ThreadedRequestData (vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int ext[6], int id);

  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);

  // see vtkAlgorithm for docs.
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  double            Opacity;
  int               UseOverlay;

private:
  vtkKWEPaintbrushBlend(const vtkKWEPaintbrushBlend&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushBlend&);  // Not implemented.

};

#endif

