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

// .NAME vtkKWEPaintbrushStroke - A stroke is an atomic unit of a paintbrush draw
// .SECTION Description
// The stroke is intended to represent a basic unit of a paintbrush draw. A
// stroke (can be a positive (draw) or a negative (erase) stroke) is one
// continuous sketch with the brush.
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushStroke_h
#define __vtkKWEPaintbrushStroke_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEPaintbrushEnums.h"
#include "vtkObject.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushOperation;
class vtkKWEPaintbrushData;
class vtkImageData;

//BTX
class vtkKWEPaintbrushStrokeNode
{
public:
  double WorldPosition[3];
};

class vtkKWEPaintbrushStrokeInternals
{
public:
  vtkKWEPaintbrushStrokeInternals() : State( vtkKWEPaintbrushEnums::Draw ) {}
  vtkstd::vector<vtkKWEPaintbrushStrokeNode * > Node;
  vtkKWEPaintbrushEnums::BrushType              State; // Erase or Draw ?
};
//ETX

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushStroke : public vtkObject
{
  //BTX
  friend class vtkKWEPaintbrushSketch;
  friend class vtkKWEPaintbrushDrawing;
  //ETX
public:

  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushStroke *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushStroke, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the template
  virtual void SetPaintbrushOperation( vtkKWEPaintbrushOperation * );
  vtkGetObjectMacro( PaintbrushOperation, vtkKWEPaintbrushOperation );

  // Description:
  // Get the stencil.
  // The stroke is represented by a binary stencil. This is updated every
  // time a template is added to the stroke.
  virtual void SetPaintbrushData(vtkKWEPaintbrushData *);
  vtkGetObjectMacro( PaintbrushData, vtkKWEPaintbrushData );

  // Description:
  // Set/Get the image data on which the sequence is drawn. This must
  // be set by the user or bad things will happen.
  virtual void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Description:
  // Stroke can be a draw or erase stroke
  //   vtkKWEPaintbrushEnums::Draw
  //   vtkKWEPaintbrushEnums::Erase
  virtual void SetStateToDraw();
  virtual void SetStateToErase();
  virtual void SetState( int );
  virtual int  GetState();

  // Description:
  // Set the representation of the stroke.
  // See vtkKWEPaintbrushEnums::LabelType.
  virtual void SetRepresentation( int representation );
  vtkGetMacro(Representation, int);
  virtual void SetRepresentationToGrayscale()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Grayscale); }
  virtual void SetRepresentationToBinary()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Binary); }

  // This is optional - USE WITH CARE
  // By default, each stroke has extents that match that of the canvas image.
  // Although there are efficient datastructures to more compactly represent
  // a stroke, it can induce some memory drag. If you are dead sure that your
  // stroke is going to have certain bounding extents, you can set this.
  // The widgets internally use this under certain conditions to speed things
  // up. Note that, if you set this method, you must do so before you set the
  // canvas image via SetImageData(..), since calling this method, by default
  // will allocate extents that are as large as the canvas image.
  virtual void SetExtent( int extent[6] );
  vtkGetVector6Macro( Extent, int );

protected:
  vtkKWEPaintbrushStroke();
  ~vtkKWEPaintbrushStroke();

  vtkKWEPaintbrushOperation          *PaintbrushOperation;
  vtkKWEPaintbrushStrokeInternals    *Internals;
  vtkKWEPaintbrushData               *PaintbrushData;
  vtkImageData                    *ImageData;
  int                              Extent[6];
  int                              Representation;
  double                           Tolerance;
  vtkKWEPaintbrushEnums::LabelType    Label;

  // Description:
  // Allocate the internal stencil. This must be called prior to use and after
  // the SetExtent or the SetImageData method has been called.
  virtual void Allocate();

  // Description:
  // Add template at position. If auxData is specified, the shape is added
  // not only to the stroke's PaintbrushData but also the auxillary data
  // specified.
  virtual int AddShapeAtPosition(double p[3],
            vtkKWEPaintbrushData *auxData1 = NULL,
            vtkKWEPaintbrushData *auxData2 = NULL );

  // Description:
  // Set the label of the stroke. Makes sense only when editing label maps.
  void SetLabel( vtkKWEPaintbrushEnums::LabelType l ) { this->Label = l; }
  vtkGetMacro( Label, vtkKWEPaintbrushEnums::LabelType );

  // Description:
  // The time the stroke was drawn.
  vtkSetMacro( DrawTime, unsigned long );
  vtkGetMacro( DrawTime, unsigned long );
  unsigned long DrawTime;

  // Is 'a' more recent than 'b' ?
  static bool IsRecent( const vtkKWEPaintbrushStroke *a,
                        const vtkKWEPaintbrushStroke *b );

private:
  vtkKWEPaintbrushStroke(const vtkKWEPaintbrushStroke&);  //Not implemented
  void operator=(const vtkKWEPaintbrushStroke&);  //Not implemented
};

#endif
