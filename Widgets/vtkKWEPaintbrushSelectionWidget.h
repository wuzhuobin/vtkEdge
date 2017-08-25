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

// .NAME vtkKWEPaintbrushSelectionWidget 
#ifndef __vtkKWEPaintbrushSelectionWidget_h
#define __vtkKWEPaintbrushSelectionWidget_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEAbstractPaintbrushWidget.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushSelectionRepresentation;
class vtkKWEPaintbrushSketch;
class vtkKWEWidgetGroup;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushSelectionWidget : public vtkKWEAbstractPaintbrushWidget
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushSelectionWidget *New();

  // Description:
  // Standard methods for a VTK class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushSelectionWidget,vtkKWEAbstractPaintbrushWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The method for activiating and deactiviating this widget. This method
  // must be overridden because it is a composite widget and does more than
  // its superclass' vtkAbstractWidget::SetEnabled() method.
  virtual void SetEnabled(int);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkKWEPaintbrushSelectionRepresentation *r);

  // Description:
  // Method to satisfy the superclass API. 
  // Create the default vtkKWEPaintbrushRepresentation if one is not set.
  virtual void CreateDefaultRepresentation();
    
  // Description:
  // The widget by default supports drag and drop merge of sketches. You 
  // may enable/disable this
  vtkSetMacro( EnableDragAndDrop, int );
  vtkGetMacro( EnableDragAndDrop, int );
  vtkBooleanMacro( EnableDragAndDrop, int );

  // Description:
  // Delete selected sketches.
  virtual void DeleteSelectedSketches();

  // Description:
  // Merge sketches into supplied sketch. Old sketches 
  //virtual void MergeSelectedSketches( vtkKWEPaintbrushSketch * s );

  //BTX
  //virtual void RemoveSketches( std::vector< vtkKWEPaintbrushSketch * > );
  //virtual void MergeSketchesInto( std::vector< vtkKWEPaintbrushSketch * >, 
  //                                vtkKWEPaintbrushSketch *newSketch );
  //ETX

  // Description:
  // Synchronize states etc.
  virtual void DeepCopy( vtkAbstractWidget * );

  // Description:
  // INTERNAL - Do not use
  // To be used by the vtkKWEPaintbrushWidget to propagate Abort events from
  // the callback command, since this widget is meant to be used as a child
  // of vtkKWEPaintbrushWidget
  vtkGetObjectMacro( EventCallbackCommand, vtkCallbackCommand );
  
protected:
  vtkKWEPaintbrushSelectionWidget();
  ~vtkKWEPaintbrushSelectionWidget();
 
  // Description:
  // States
  //BTX
  enum
    {
    Disabled = 10000,
    BeginSelecting,
    EndSelecting,
    None
    };
  //ETX
  
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
  static void BeginToggleSelectSketchCallback( vtkAbstractWidget* );
  static void EndToggleSelectSketchCallback  ( vtkAbstractWidget* );
  static void DeleteSelectionCallback        ( vtkAbstractWidget* );
  static void MergeSelectionCallback         ( vtkAbstractWidget* );
  static void SelectAllSketchesCallback      ( vtkAbstractWidget* );
  static void UnSelectAllSketchesCallback    ( vtkAbstractWidget* );
  static void ToggleSelectAllSketchesCallback( vtkAbstractWidget* );
  static void MoveCallback                   ( vtkAbstractWidget* );

  // Description:
  // Geometric actions / states on that the representation responds to.
  int SelectSketchAction       ( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int UnselectSketchAction     ( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int SelectAllSketchesAction  ( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int UnSelectAllSketchesAction( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int ReleaseSelectAction      ( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int DeleteSelectionAction    ( vtkKWEPaintbrushSelectionWidget *dispatcher);
  int MergeSelectionAction     ( vtkKWEPaintbrushSelectionWidget *dispatcher);

  int                   WidgetState;
  vtkKWEPaintbrushSketch * MergedSketch;
  int                   RemoveSketchesDuringMerge;
  int                   EnableDragAndDrop;
  
private:
  vtkKWEPaintbrushSelectionWidget(const 
      vtkKWEPaintbrushSelectionWidget&);  //Not implemented
  void operator=(const vtkKWEPaintbrushSelectionWidget&);  //Not implemented
};

#endif
