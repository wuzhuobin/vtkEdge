//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2008 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the GNU General Public License 
//   version 3 as published by the Free Software Foundation and appearing in 
//   the file LICENSE.txt included in the top level directory of this source
//   code distribution. Alternatively you may (at your option) use any later 
//   version of the GNU General Public License if such license has been 
//   publicly approved by Kitware, Inc. (or its successors, if any).
//
//   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
//   PURPOSE. See LICENSE.txt for additional details.
//
//   VTKEdge is available under alternative license terms. Please visit
//   vtkedge.org or contact us at kitware@kitware.com for further information.
//
//=============================================================================
// .NAME vtkKWEPaintbrushWidget - Paintbrush widget.
//
// .SECTION Description
// The vtkKWEPaintbrushWidget is intended to edit and create segmentations. The
// widget operates in two modes. An "edit" mode and a "select" mode. In the 
// "edit" mode, the widget can be used to edit and create segmentations. In 
// the "select" mode, the widget allows you to select one or more seperate
// segmentations and then merge them into a single segmentation, do one click
// deletion of selected segmentations etc.
//
// .SECTION Framework
// The key to understanding brushes is to understand the datastructures, the
// model and role of the following classes. 
//   (1) vtkKWEPaintbrushDrawing (2) vtkKWEPaintbrushSketch (3) vtkKWEPaintbrushStroke
//   (4) vtkKWEPaintbrushData (5) vtkKWEPaintbrushShape (6) vtkKWEPaintbrushProperty
//   (7) vtkKWEPaintbrushOperation (8) Binary and Grayscale representations
//   (9) vtkPointPlacer (10) Widget Groups
//
// .SECTION Drawing
// A PaintbrushWidget is used to edit a "Drawing" (vtkKWEPaintbrushDrawing).
// The drawing contains several "sketches" (vtkKWEPaintbrushSketch).
//
// .SECTION Sketch
// The drawing contains several "sketches" (vtkKWEPaintbrushSketch). Think of a 
// each sketch as a segmentation. Each "sketch" is composed of one or more
// "strokes". Keeping the strokes around allows for undo-redo operations. 
//
// .SECTION Stroke
// Each stroke (vtkKWEPaintbrushStroke) represents a swish of the brush. It is 
// created interactively by dragging a "shape" (vtkKWEPaintbrushShape) around.
// It can also be created manually (to load predefined segmentations/edits).
// A stroke can be a positive stroke (draw) or a negative stroke (erase).
//
// .SECTION Shape
// Shapes (vtkKWEPaintbrushShape) modes a brush. It allows you to have your 
// brush take a variety of forms. The output of the shape is filtered through
// an "operation" (vtkKWEPaintbrushOperation) before being composed into its 
// stroke. Concrete shapes can be created by deriving from vtkKWEPaintbrushShape. 
// Two examples, an ellipsoid and a box are provided. A shape maintains a 
// polarity (correspoding to a draw and an erase brush).
//
// .SECTION Operation
// Operations (vtkKWEPaintbrushOperation) allow you to plug in filters that take
// effect as you draw. For instance, you can have a region growing operation 
// that does a flood fill on the image as you draw. A few operations are
// provided as examples. Bridges between ITK and VTK can also be built. Its
// customary to use ITK's powerful imaging filters to construct operations. 
// An example is vtkKWEITKConfidenceConnectedPaintbrushOperation.
//
// .SECTION PointPlacers
// The widget uses "point placers" (vtkPointPlacer) to dictate the placement
// of shapes. Point placers allow you to place constraints on the placement of
// shapes. For instance a vtkKWEVoxelAlignedImageActorPointPlacer constrains
// placement of shapes to pixel-grid boundaries. A vtkBoundedPlanePointPlacer
// can be used to constrain placement to an arbitrary region, bounded by a set
// of planes. A varirty of point placers exist in VTK and they lend themselves 
// to easy subclassing.
// 
// .SECTION vtkKWEPaintbrushData
// This is an abstract class that is used to store edits made using the 
// paintbrush. Each sketch maintains its own paintbrush data. Each stroke also
// maintaints its own paintbrush data. The sketch's data is merely a composition
// of the data from all its storkes. Two concrete implementations of 
// vtkKWEPaintbrushData exist. One for storing binary 1/0 strokes and another for 
// storing grayscale strokes. 
//<P> 
// vtkKWEPaintbrushData provides minkowski operators. You can add, subtract, 
// merge, intersect data.
//
// .SECTION Representations
// Two representations are supported, a binary and a grayscale representation.
// The Binary representation is rendered via an overlay. (see
// vtkKWEPaintbrushRepresentation2D) Its datastructures are modeled by 
// vtkKWEPaintbrushStencilData and are memory efficient (They maintain a run 
// length encoded structure internally).
//<P>
// The grayscale representation is rendered as contours. See
// vtkKWEPaintbrushRepresentationGrayscale2D. It stores the data internally using
// vtkKWEPaintbrushGrayscaleData, which maintains a vtkImageData under the hood. 
//
// .SECTION Property
// A drawing can contain many sketches. Each sketch is rendered on the screen. 
// A sketch's properties are encapsulated in vtkKWEPaintbrushProperty. A property
// manager automatically manages colors to ensure that each new sketch created 
// by the user has unique colors. However you may set your own colors. Apart
// from managing colors, the property manages highlight styles. These take 
// effect when a sketch is highlighted (interactively during select mode) or 
// programatically. The Property also has methods to change the mutability of
// sketches, a functionality supported only when editing label maps.
//
// .SECTION Widget Groups
// It is customary for the user working on his segmentation(s) to have an 
// axial, coronal and sagittal view side by side while editing. vtkKWEWidgetGroup 
// enables this. It links widgets that exist on multiple render windows 
// together, so that they behave in unison. It relieves the user of the burden 
// of managing these disparate widgets and their underlying datastructures.
//
// .SECTION Supporting classes
// Several supporting classes exist in the library. Note that 
// vtkKWEPaintbrushDrawing is a data-object. Algorithms can run on it that take
// each of the segmentations and perform operations. See for instance
// vtkKWEPaintbrushMergeSketches and vtkKWEITKPaintbrushExtractConnectedComponents.
// The former merges all sketches in a drawing. The latter extracts out each
// connected component in a sketch and populates them into seperate sketches.
//
// .SECTION Interaction
//<P>
// The follwing interactions are supported by the brush in Edit mode.
//   Left click and drag       - Draws a stroke
//   Ctrl-Left click and drag  - Erases the stroke
//   Backspace or Left key     - Undo the sketch (deletes the previously drawn stroke in a sketch)
//   Right key                 - Redo on the given sketch
//   Shift "+"                 - Start a new sketch.
//   Shift Right               - Traverse to the next sketch.
//   Shift Left                - Traverse to the previous sketch.
//   Shift Backspace           - Delete a sketch.
//   Right click and drag      - Resize the shape isotropically
//   Ctrl-Right click and drag - Resize the shape anisotropically
//   Shift "<"                 - Decrease the opacity of the overlaid drawing
//   Shift ">"                 - Increase the opacity of the overlaid drawing
//   "l" key                   - Render the current sketch immutable/mutable. (Supported only when editing labelmaps)
//   Escape                    - Enable/Disable interactions
//<P>
// The follwing interactions are supported by the brush in Select Mode
//   Left click                - Select/Unselect a sketch
//   Left click and drag       - Drag and drop merge of sketches.
//   Backspace key             - Delete selected sketches.
//   "m" key                   - Merge selected sketches.
//   Escape                    - Enable/Disable interactions
//
// .SECTION See Also
//
#ifndef __vtkKWEPaintbrushWidget_h
#define __vtkKWEPaintbrushWidget_h

#include "VTKEdgeConfigure.h" // needed for export symbol directives
#include "vtkKWEAbstractPaintbrushWidget.h"

class vtkKWEPaintbrushRepresentation;
class vtkKWEPaintbrushAnnotationWidget;
class vtkKWEPaintbrushSelectionWidget;
class vtkKWEWidgetGroup;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushWidget : public vtkKWEAbstractPaintbrushWidget
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushWidget *New();

  // Description:
  // Standard methods for a VTK class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushWidget,vtkKWEAbstractPaintbrushWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The method for activiating and deactiviating this widget.
  virtual void SetEnabled(int);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene.
  void SetRepresentation(vtkKWEPaintbrushRepresentation *r);

  // Description:
  // Create the default vtkKWEPaintbrushRepresentation if one is not set.
  virtual void CreateDefaultRepresentation();

  // Description:
  // Set the mode that the widget should operate in. The Paintbrush widget can
  // operate either in an "edit" mode or in a "select" mode. In the "edit"
  // mode, you can draw, erase, create and edit segmentations. The "select"
  // mode facilitates one/multi-click selection of sketches and allows you
  // merge and remove segmentations. Default mode is edit.
  virtual void SetPaintbrushMode( int );
  vtkGetMacro( PaintbrushMode, int );

  // Description:
  // Get the selection widget. This is relevant only if the PaintbrushMode
  // is set to "Select".
  vtkGetObjectMacro(PaintbrushSelectionWidget, vtkKWEPaintbrushSelectionWidget);

  // Description:
  // Get the annotation manager. This is used to display hover annotations
  vtkGetObjectMacro(PaintbrushAnnotationWidget, vtkKWEPaintbrushAnnotationWidget);

  // Description:
  // Events and states. 
  //BTX
  enum
  {
    BeginDrawStrokeEvent   = 10000,
    BeginEraseStrokeEvent,
    EndStrokeEvent,
    UndoStrokeEvent,
    RedoStrokeEvent,
    DeleteStrokeEvent,
    BeginResizeEvent,
    BeginIsotropicResizeEvent,
    BeginNewSketchEvent,
    EndResizeEvent,
    ToggleSelectStateEvent,
    DrawEvent,
    EraseEvent,
    IncrementSketchEvent,
    DecrementSketchEvent,
    IncreaseOpacityEvent,
    DecreaseOpacityEvent,
    SelectSketchEvent,
    SelectAllSketchesEvent,
    UnselectSketchEvent,
    UnSelectAllSketchesEvent,    
    ToggleSelectAllSketchesEvent,
    ToggleSketchMutabilityEvent,
    DeleteSelectionEvent,
    MergeSelectionEvent,
    EnterEvent,
    LeaveEvent
  };
  enum 
    {
    PaintbrushInteract, 
    PaintbrushDraw, 
    PaintbrushErase, 
    PaintbrushResize,
    PaintbrushIsotropicResize,
    PaintbrushDisabled
    };
  enum PaintbrushMode
    {
    Edit,
    Select
    };
  //ETX
  
  // Description:
  // Deep copy from another widget. Synchronizes states, representations, etc
  virtual void DeepCopy(vtkAbstractWidget *w);

  // Description:
  // Override method to propagate to the child vtkKWEPaintbrushSelection widget
  virtual void SetProcessEvents(int);

protected:
  vtkKWEPaintbrushWidget();
  ~vtkKWEPaintbrushWidget();
 
  // Description:
  // Callback interface to tie ends with the CallbackMapper for the events
  // invoked by this widget. Note the seperation of "Callbacks" and "Actions".
  // The callbacks are invoked in response to events from the 
  // RenderWindowInteractor. They are then dispatched to the WidgetGroup, who
  // inturn calls the corresponding "Actions" on each of the widgets in the
  // group.
  //
  // As a rule of thumb, the mapping of "Callbacks" is to "Events" from the
  // Callbackmapper. The mapping of "Actions" is meant to be one pertinent
  // to the geometry changes on the widget's representation.
  //    For instance a MouseMoveEvent should map to a "MoveCallback". This
  // can, depending on the state of the widget, trigger any of three Actions:
  // HoverAction, DrawAction, ResizeAction.
  //    This is done, so that you can have multiple widgets, say a seed
  // widget drawn on 3 render windows and have them all synchronized, if they
  // belong to the same group.
  // 
  static void BeginDrawCallback                ( vtkAbstractWidget* );
  static void BeginEraseCallback               ( vtkAbstractWidget* );
  static void MoveCallback                     ( vtkAbstractWidget* );
  static void EndStrokeCallback                ( vtkAbstractWidget* );
  static void UndoCallback                     ( vtkAbstractWidget* );
  static void RedoCallback                     ( vtkAbstractWidget* );
  static void DeleteCallback                   ( vtkAbstractWidget* );
  static void BeginResizeShapeCallback         ( vtkAbstractWidget* );
  static void BeginIsotropicResizeShapeCallback( vtkAbstractWidget* );
  static void EndResizeShapeCallback           ( vtkAbstractWidget* );
  static void ToggleSelectStateCallback        ( vtkAbstractWidget* );
  static void BeginNewSketchCallback           ( vtkAbstractWidget* );
  static void IncrementSketchCallback          ( vtkAbstractWidget* );
  static void DecrementSketchCallback          ( vtkAbstractWidget* );
  static void IncreaseOpacityCallback          ( vtkAbstractWidget* );
  static void DecreaseOpacityCallback          ( vtkAbstractWidget* );
  static void MergeSelectionCallback           ( vtkAbstractWidget* );
  static void ToggleSelectAllSketchesCallback  ( vtkAbstractWidget* );
  static void ToggleSketchMutabilityCallback   ( vtkAbstractWidget* );
  static void EnterWidgetCallback              ( vtkAbstractWidget* );
  static void LeaveWidgetCallback              ( vtkAbstractWidget* );

  // Description:
  // Geometric actions / states on that the representation responds to.
  int BeginDrawAction              ( vtkKWEPaintbrushWidget *dispatcher);
  int EndStrokeAction              ( vtkKWEPaintbrushWidget *dispatcher);
  int BeginEraseAction             ( vtkKWEPaintbrushWidget *dispatcher);
  int HoverAction                  ( vtkKWEPaintbrushWidget *dispatcher);
  int UndoAction                   ( vtkKWEPaintbrushWidget *dispatcher);
  int RedoAction                   ( vtkKWEPaintbrushWidget *dispatcher);
  int DeleteAction                 ( vtkKWEPaintbrushWidget *dispatcher);
  int BeginResizeAction            ( vtkKWEPaintbrushWidget *dispatcher);
  int BeginIsotropicResizeAction   ( vtkKWEPaintbrushWidget *dispatcher);
  int ResizeAction                 ( vtkKWEPaintbrushWidget *dispatcher);
  int EndResizeAction              ( vtkKWEPaintbrushWidget *dispatcher);
  int ToggleSelectStateAction      ( vtkKWEPaintbrushWidget *dispatcher);
  int BeginNewSketchAction         ( vtkKWEPaintbrushWidget *dispatcher);
  int IncrementSketchAction        ( vtkKWEPaintbrushWidget *dispatcher);
  int DecrementSketchAction        ( vtkKWEPaintbrushWidget *dispatcher);
  int EnterWidgetAction            ( vtkKWEPaintbrushWidget *dispatcher);
  int LeaveWidgetAction            ( vtkKWEPaintbrushWidget *dispatcher);
     
  // Description:  
  // Cursor management
  virtual void SetCursor( int interactionState );

private:
  vtkKWEPaintbrushWidget(const vtkKWEPaintbrushWidget&);  //Not implemented
  void operator=(const vtkKWEPaintbrushWidget&);  //Not implemented

  int                             ResizeStartPosition[2];
  double                          ResizeFactor[3];
  int                             WidgetState;
  int                             PaintbrushMode;
  vtkKWEPaintbrushAnnotationWidget * PaintbrushAnnotationWidget;
  vtkKWEPaintbrushSelectionWidget  * PaintbrushSelectionWidget;
};

#endif
