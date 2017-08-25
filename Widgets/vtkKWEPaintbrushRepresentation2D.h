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

// .NAME vtkKWEPaintbrushRepresentation2D - A widget representation that represents 2D paintbrushes
// .SECTION Description
// This is a concrete implementation of the abstract vtkKWEPaintbrushRepresentation
// class for paintbrush effects on a 2D scene.
//
// A typical use will be as follows:
//
// \code
// vtkKWEPaintbrushWidget * widget = vtkKWEPaintbrushWidget::New();
// widget->SetInteractor( RenderWindowInteractor );
// vtkKWEPaintbrushRepresentation2D * rep2D = vtkKWEPaintbrushRepresentation2D::
//    SafeDownCast( widget->GetRepresentation() );
// \endcode
//
// .SECTION Behaviour
// The paintbrush should behave like GIMP paintbrushes do. When you select a
// paintbrush, you will have the template outline along with an alpha
// transparency of the template moving with the cursor. You can click-drag-release
// to draw/erase
//
// The vtkKWEPaintbrushWidget will set this representation into one of three
// states: Draw, Erase or Interact. During the Interact state the brush
// outline just hovers around with the template. During the Draw / Erase
// state, the representation uses the stroke manager to add a stroke /
// erase a stroke.
//
// .SECTION ShapePlacer
// The default template uses a shape placer to validate the "validity" of the
// shape at a given point. The default shape placer is a
// \c vtkKWEVoxelAlignedImageActorPointPlacer. This placer snaps the tempate to
// a position on the image grid. See the vtkPointPlacer architecture for
// details. You can use this to restrict the shape to a given set of bounding
// planes.
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushRepresentation2D_h
#define __vtkKWEPaintbrushRepresentation2D_h

#include "vtkKWEPaintbrushRepresentation.h"

class vtkActor;
class vtkProperty;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkPoints;
class vtkCellArray;
class vtkKWEPaintbrushBlend;
class vtkImageActor;
class vtkImageData;
class vtkActor2D;
class vtkTextMapper;
class vtkTextProperty;
class vtkKWEPaintbrushHighlightActors;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushRepresentation2D
                              : public vtkKWEPaintbrushRepresentation
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushRepresentation2D *New();

  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushRepresentation2D,vtkKWEPaintbrushRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();
  virtual int  ComputeInteractionState(int X, int Y, int modified=0);
  virtual void StartWidgetInteraction(double e[2]);

  // Description:
  // Must be present to satisfy the vtkWidgetRepresentation's API.
  // This method will check the state of the representation. If the state is
  // "Draw" (states are set by the vtkKWEPaintbrushWidget); the representation
  // will enter this state during click-drags, the DrawFromLastEventPositionTo
  // method is invoked.
  virtual void WidgetInteraction(double eventPos[2]);

  // Description:
  // Methods required by vtkProp superclass.
  virtual void ReleaseGraphicsResources(vtkWindow *w);
  virtual int  RenderOverlay(vtkViewport *viewport);
  virtual int  RenderOpaqueGeometry(vtkViewport *viewport);

#if VTKEdge_VTK_VERSION_DATE > 20070305
  virtual int  RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual int  HasTranslucentPolygonalGeometry();
#else
  // Legacy method to support VTK source versions prior to 2007/03/05
  virtual int  RenderTranslucentGeometry(vtkViewport *viewport);
#endif

  // Description:
  // Set/Get the properties of the template outline. This should behave like
  // the GIMP paintbrushes do. When you select a paintbrush, you will have
  // the template outline along with an alpha transparency at the cursor
  // location. You can move around, place it somewhere, or drag with the
  // mouse button to create a stroke.
  void SetShapeOutlinePropertyInteract(vtkProperty*);
  vtkGetObjectMacro(ShapeOutlinePropertyInteract,vtkProperty);
  void SetShapeOutlinePropertyDraw(vtkProperty*);
  vtkGetObjectMacro(ShapeOutlinePropertyDraw,vtkProperty);
  void SetShapeOutlinePropertyErase(vtkProperty*);
  vtkGetObjectMacro(ShapeOutlinePropertyErase,vtkProperty);

  // Description:
  virtual int ActivateShapeOutline( int x, int y );

  // Description:
  // Create the template outline at location pos[3] (world coords)
  virtual void CreateShapeOutline( double * pos );

  // Description:
  // Get the actors maintained by the representation, that need to be rendered.
  virtual void GetActors(vtkPropCollection *);
  virtual void GetActors2D(vtkPropCollection *);

  // Description:
  // Set the Paintbrush representation state
  virtual void SetStateToDraw();
  virtual void SetStateToErase();
  virtual void SetStateToInteract();
  virtual void SetStateToDisabled();

  // Description:
  // Set the paintbrush operation. Here we override the superclass methodm so
  // that we can set some defaults of the operation based on the imageactor's
  // input.
  virtual void SetPaintbrushOperation( vtkKWEPaintbrushOperation * );

  // Description:
  // Set the image actor on which the paintbrush is drawn. We will need to
  // update the actor with the stencil's output etc..
  virtual void SetImageActor( vtkImageActor * );
  vtkGetObjectMacro ( ImageActor, vtkImageActor );

  // Description:
  // Set the underlying image data. While this is not always necessary, it
  // is required by some variants of the paintbrush widget such as region
  // growing widgets, etc that use features from the underlying image data.
  // At best, just specify the underlying image data always.. (In most
  // cases, its just the ImageActor->GetInput())
  //
  // NOTE: The canvas (PaintbrushDrawing) gets allocated when you invoke this
  //       method. You would do well to set the representation on the drawing
  //       prior to calling this method, so as to avoid unnecessary
  //       re-allocation later on.
  virtual void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Descirption:
  // Deep copy.. synchronizes states etc..
  virtual void DeepCopy(vtkWidgetRepresentation *r);

  // Description:
  // See superclass documentation
  virtual int  ResizeShape( double d[3], int resizeType );
  virtual void SetPaintbrushDrawing( vtkKWEPaintbrushDrawing * );
  virtual void SetShapeOutlineVisibility( int );
  virtual int  GetShapeOutlineVisibility( );

  // Description:
  // INTERNAL - Do not use.
  // Invoked by the widget in response to user interaction
  // Increase / Decrease the opacity of the drawing.
  virtual int IncreaseOpacity();
  virtual int DecreaseOpacity();

  // Description:
  // INTERNAL - Do not use
  // Sticks the stencil into the pipeline. (between the ImageActor and its
  // original input).
  virtual void InstallPipeline();
  virtual void UnInstallPipeline();

  // Description:
  // See docuementation of vtkKWEPaintbrushBlend::SetUseOverlay
  vtkSetMacro( UseOverlay, int );
  vtkGetMacro( UseOverlay, int );
  vtkBooleanMacro( UseOverlay, int );

  // Description:
  // Do a slice by slice painting, or paint in 3D using a 3D shape. If we do a
  // Single slice painting, the dimension of the shape orthogonal to the
  // currently displayed slice will be flattened implicitly. Default is 3D
  // painting.
  vtkSetMacro( SingleSliceThickBrush, int );
  vtkGetMacro( SingleSliceThickBrush, int );
  vtkBooleanMacro( SingleSliceThickBrush, int );

  // Description:
  // Copy a sketch from the currently displayed slice to the next/previous
  // slice. Returns 1 on success / 0 on failure.
  virtual int CopySketchToNextSlice( vtkKWEPaintbrushSketch *s );
  virtual int CopySketchToPreviousSlice( vtkKWEPaintbrushSketch *s );

  // Description:
  // INTERNAL - Do not use
  // This method is called from vtkKWEPaintbrushWidget whenever we draw using
  // a shape centered at the location "p". Here we override the superclass
  // method.
  virtual void AddShapeToCurrentStroke( double p[3] );

protected:
  vtkKWEPaintbrushRepresentation2D();
  ~vtkKWEPaintbrushRepresentation2D();

  // Description:
  // Create default representation. The default representation instantiates
  // a new sequencer, if one hasn't been set. It also sets default properties
  // for the ShapeOutline etc.
  virtual void CreateDefaultRepresentation();

  vtkPolyData                  *ShapeOutline;
  vtkPolyDataMapper            *ShapeOutlineMapper;
  vtkActor                     *ShapeOutlineActor;
  vtkProperty                  *ShapeOutlinePropertyInteract;
  vtkProperty                  *ShapeOutlinePropertyDraw;
  vtkProperty                  *ShapeOutlinePropertyErase;
  vtkImageActor                *ImageActor;
  vtkImageData                 *ImageData;
  vtkKWEPaintbrushHighlightActors *HighlightActors;
  double                        LastEventPosition[2];
  int                           SingleSliceThickBrush;

  // Description:
  // Get the extents of the current etch.
  virtual void GetEtchExtents( int extent[6] );

private:
  vtkKWEPaintbrushRepresentation2D(const vtkKWEPaintbrushRepresentation2D&);  //Not implemented
  void operator=(const vtkKWEPaintbrushRepresentation2D&);  //Not implemented

  vtkKWEPaintbrushBlend        *PaintbrushBlend;
  int                           UseOverlay;
};

#endif

