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

// .NAME vtkKWEPaintbrushRepresentation - An abstract base class for 2D/3D paintbrush widget representations
// .SECTION Description
// See vtkKWEPaintbrushRepresentation2D for more details.
// The ShapePlacer must be set prior to use.

#ifndef __vtkKWEPaintbrushRepresentation_h
#define __vtkKWEPaintbrushRepresentation_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkWidgetRepresentation.h"

class vtkImageStencil;
class vtkImageStencilData;
class vtkTimeStamp;
class vtkKWEPaintbrushDrawing;
class vtkKWEPaintbrushOperation;
class vtkKWEPaintbrushAnnotationRepresentation;
class vtkKWEPaintbrushSelectionRepresentation;
class vtkKWEPaintbrushSketch;
class vtkPointPlacer;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushRepresentation : public vtkWidgetRepresentation
{
  //BTX
  friend class vtkKWEPaintbrushAnnotationRepresentation;
  //ETX
public:
  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushRepresentation,vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX -- The representation can be set in two states by the widget. Draw/Erase
  enum
    {
    PaintbrushInteract,
    PaintbrushDraw,
    PaintbrushErase,
    PaintbrushResize,
    PaintbrushIsotropicResize,
    PaintbrushDisabled
    };
  //ETX

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation()=0;
  virtual int  ComputeInteractionState(int X, int Y, int modified=0)=0;
  virtual void StartWidgetInteraction(double e[2])=0;
  virtual void WidgetInteraction(double e[2])=0;

  // Description:
  // Set the Paintbrush sequencer
  virtual void SetPaintbrushDrawing( vtkKWEPaintbrushDrawing * );
  vtkGetObjectMacro( PaintbrushDrawing, vtkKWEPaintbrushDrawing );

  // Description:
  // Set the paintbrush operation.
  virtual void SetPaintbrushOperation( vtkKWEPaintbrushOperation * );
  vtkGetObjectMacro( PaintbrushOperation, vtkKWEPaintbrushOperation );

  // Description:
  // Set the Paintbrush representation state
  virtual void SetStateToDraw()     { this->InteractionState = PaintbrushDraw;     }
  virtual void SetStateToErase()    { this->InteractionState = PaintbrushErase;    }
  virtual void SetStateToInteract() { this->InteractionState = PaintbrushInteract; }
  virtual void SetStateToResize()   { this->InteractionState = PaintbrushResize;   }
  virtual void SetStateToDisabled() { this->InteractionState = PaintbrushDisabled; }
  virtual void SetStateToIsotropicResize()
    { this->InteractionState = PaintbrushIsotropicResize; }

  // Description:
  // Start a new sketch.
  virtual int BeginNewSketch();

  // Description:
  // Methods to jump to a certain sketch.
  // Returns 0 if the jump was unsuccessful, (if you try to jump outside the
  // available sketches etc).
  virtual int GoToSketch( int n );
  virtual int IncrementSketch();
  virtual int DecrementSketch();

  // Description:
  // Get the sketch being currently edited.
  int GetSketchIndex();

  // Description:
  // Start a new stroke.
  virtual int BeginNewStroke();

  // Description:
  // Remove the last stroke. Will work only if there is a stroke in the list
  virtual int UndoStroke();
  virtual int RemoveLastStroke();
  virtual int RedoStroke();

  // Description:
  // Remove an entire sketch from the drawing. Returns 1 if successful. Note
  // that this may cause the SketchIndex to decrement, if the index of the
  // currently active sketch is higher than the one being deleted. You can
  // query this value by calling GetSketchIndex().
  virtual int RemoveSketch( vtkKWEPaintbrushSketch * s );

  // Descirption:
  // Set / get the Point Placer. The point placer is responsible for
  // converting display coordinates into world coordinates according
  // to some constraints, and for validating world positions.
  // By writing an appopriate point placer, you can toggle the visibiliy
  // of the templates based on display position, have it snap to the
  // image grid etc..
  void SetShapePlacer( vtkPointPlacer * );
  vtkGetObjectMacro( ShapePlacer, vtkPointPlacer );

  // Descirption:
  // This method is meant to be invoked from vtkKWEPaintbrushWidget during user
  // interaction.
  // Resize the shape. The value passed is a factor that determines how much
  // to resize. Its up to the shape to use this factor appropriately.
  // See vtkKWEPaintbrushShape::PaintbrushResizeEnum for possible resizeTypes.
  // Returns 1 if a resize happened (you need to re-render), 0 otherwise..
  virtual int ResizeShape( double d[3], int resizeType ) = 0;

  // Descirption:
  // Get the current position of the center of the shape.
  vtkGetVector3Macro(CurrentShapePosition, double);

  // Descirption:
  // Deep copy.. synchronizes states etc..
  virtual void DeepCopy(vtkWidgetRepresentation *);

  // Description:
  // Check if display point (x,y) is inside the paintable region of the canvas.
  // The ShapePlacer can be used to dictate which location in the canvas the
  // shape can etch on. You can very easily restrict drawing to certain
  // region simply by writing an appropriate point placer. (See vtkPointPlacer)
  virtual int IsInsideCanvas( int displayPos[2] );

  // Description:
  // Get the annotation representation. This displays timer based informative
  // annotations
  vtkGetObjectMacro( Annotation, vtkKWEPaintbrushAnnotationRepresentation );

  // Description:
  // Get the selection widget's representation. This is used if the paintbrush
  // is in selection mode.
  vtkGetObjectMacro( SelectionRepresentation,
                     vtkKWEPaintbrushSelectionRepresentation );

  // Description:
  // INTERNAL - Do not use.
  // Add a shape centered at "p" to the currently active sketch's current
  // stroke. This method is called by the widget whenever we interactively
  // trace.
  virtual void AddShapeToCurrentStroke( double p[3] );

  // Description:
  // INTERNAL - Do not use
  // Invoked by the widget in response to user interaction
  // Activate the template outline and the supplied coordinate
  // according to the constraints of the placer ?
  virtual int ActivateShapeOutline( int X, int Y ) = 0;

  // Description:
  // INTERNAL - Do not use
  // Invoked by the widget in response to user interaction
  // Create a shape outline at the current shape position. Does not check the
  // placer for constraints.
  virtual void CreateShapeOutline(double *pos) = 0;

  // Description:
  // INTERNAL - Do not use.
  // Invoked by the widget in response to user interaction
  // Turn on/off the hovering shape outline visibility
  virtual void SetShapeOutlineVisibility( int ) = 0;
  virtual int  GetShapeOutlineVisibility( ) = 0;

  // Description:
  // INTERNAL - Do not use.
  // Invoked by the widget in response to user interaction
  // Increase / Decrease the opacity of the drawing.
  virtual int IncreaseOpacity() { return 0; }
  virtual int DecreaseOpacity() { return 0; }

  // Description:
  // INTERNAL - Do not use.
  virtual void InstallPipeline();
  virtual void UnInstallPipeline();

protected:
  vtkKWEPaintbrushRepresentation();
  ~vtkKWEPaintbrushRepresentation();

  vtkKWEPaintbrushOperation      *PaintbrushOperation;
  vtkKWEPaintbrushDrawing        *PaintbrushDrawing;

  // Description:
  // The point placer determines the world coordinate based on screen
  // coordinate and allows you to set constraints too.. It can also
  // be used to toggle the template visibility depending on the position
  // if you write an appropriate subclass of the point placer. One
  // such subclass snaps the contour to regular positions on the image
  // grid.
  vtkPointPlacer             * ShapePlacer;

  // Description:
  // Displays timer based informative annotations
  vtkKWEPaintbrushAnnotationRepresentation * Annotation;

  // Description:
  // The selection widget's representation. This is used if the paintbrush
  // is in selection mode.
  vtkKWEPaintbrushSelectionRepresentation  * SelectionRepresentation;

  // Description:
  // Bookkeeping of the last display position.
  int    LastDisplayPosition[2];
  double CurrentShapePosition[3];
  int    SketchIndex; // We can manage multiple sketches.
  int    ShapeOutlineVisibility;
  int    PipelineInstalled;

  // Description:
  // Get the extents of the current etch.
  virtual void GetEtchExtents( int extent[6] );

private:
  vtkKWEPaintbrushRepresentation(const vtkKWEPaintbrushRepresentation&);  //Not implemented
  void operator=(const vtkKWEPaintbrushRepresentation&);  //Not implemented
};

#endif

