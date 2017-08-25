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

#include "vtkKWEPaintbrushSelectionWidget.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushSelectionRepresentation2D.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushSelectionWidget, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushSelectionWidget);

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionWidget::vtkKWEPaintbrushSelectionWidget()
{
  this->ManagesCursor     = 1;
  this->EnableDragAndDrop = 1; // Enable drag and drop merge by default

  // These are the event callbacks supported by this widget
  this->CallbackMapper->SetCallbackMethod(
         vtkCommand::LeftButtonPressEvent,
         vtkEvent::NoModifier, 0, 0, NULL,
         vtkKWEPaintbrushWidget::SelectSketchEvent,
         this, vtkKWEPaintbrushSelectionWidget::BeginToggleSelectSketchCallback);
  this->CallbackMapper->SetCallbackMethod(
         vtkCommand::LeftButtonReleaseEvent,
         vtkEvent::NoModifier, 0, 0, NULL,
         vtkCommand::LeftButtonReleaseEvent,
         this, vtkKWEPaintbrushSelectionWidget::EndToggleSelectSketchCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
         vtkEvent::NoModifier, 47, 1, "Delete",
         vtkWidgetEvent::Delete,
         this, vtkKWEPaintbrushSelectionWidget::DeleteSelectionCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
         vtkEvent::NoModifier, 8, 1,"BackSpace",
         vtkWidgetEvent::Delete,
         this, vtkKWEPaintbrushSelectionWidget::DeleteSelectionCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
         vtkEvent::NoModifier, 109, 0, "m",
         vtkKWEPaintbrushWidget::MergeSelectionEvent,
         this, vtkKWEPaintbrushSelectionWidget::MergeSelectionCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+a
           vtkEvent::ControlModifier, 1, 0,"a",
           vtkKWEPaintbrushWidget::ToggleSelectAllSketchesEvent,
           this, vtkKWEPaintbrushSelectionWidget::ToggleSelectAllSketchesCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
         vtkWidgetEvent::Move,
         this, vtkKWEPaintbrushSelectionWidget::MoveCallback);

  this->CreateDefaultRepresentation();

  // Sketches merged are removed by default
  this->RemoveSketchesDuringMerge = 1;

  this->MergedSketch = NULL;
  this->WidgetState  = vtkKWEPaintbrushSelectionWidget::None;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionWidget::~vtkKWEPaintbrushSelectionWidget()
{
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget::CreateDefaultRepresentation()
{
  // Creates only the 2D rep by default for now
  // We will accomodate the 3D rep later.
  if ( ! this->WidgetRep )
    {
    vtkKWEPaintbrushSelectionRepresentation2D *rep =
      vtkKWEPaintbrushSelectionRepresentation2D::New();
    this->SetRepresentation(rep);
    rep->Delete();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget::SetEnabled(int enabling)
{
  if (this->Enabled == enabling)
    {
    return; // Nothing to do
    }

  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);

  // Start off on a clean slate.
  rep->UnSelectAllSketches();

  rep->SetVisibility( enabling );
  this->Superclass::SetEnabled(enabling);
}

// The following methods are the callbacks that the widget responds to.
//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::BeginToggleSelectSketchCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }

  self->WidgetState = None;
  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(self->WidgetRep);

  // Request a selection. The representation is free to deny it based on
  // constraints of the placer etc.
  rep->SetInteractionState(
    vtkKWEPaintbrushSelectionRepresentation::PaintbrushRequestSketchSelect);

  int interactionState = rep->ComputeInteractionState(
    self->Interactor->GetEventPosition()[0],
    self->Interactor->GetEventPosition()[1]);

  if (interactionState == vtkKWEPaintbrushSelectionRepresentation::
        PaintbrushSketchSelect)
    {
    // We selected some sketch.
    self->WidgetState = vtkKWEPaintbrushSelectionWidget::BeginSelecting;
    self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::SelectSketchAction);
    self->EventCallbackCommand->SetAbortFlag(1);
    }

  else if (interactionState == vtkKWEPaintbrushSelectionRepresentation::
        PaintbrushSketchUnselect)
    {
    // We de-selected some sketch.
    self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::UnselectSketchAction);
    self->EventCallbackCommand->SetAbortFlag(1);
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::EndToggleSelectSketchCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }
  self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::ReleaseSelectAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::DeleteSelectionCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }

  self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::DeleteSelectionAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::ToggleSelectAllSketchesCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }

  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(self->WidgetRep);

  const int nSketches = rep->GetPaintbrushDrawing()->GetNumberOfItems();
  if (nSketches)
    {
    const int nSelectedSketches =
      static_cast<int>(rep->GetSelectedSketches().size());

    // If more than half the sketches are selected, unselect all of them.
    // If less than half the sketches are selected, select all of them.
    if (nSelectedSketches > nSketches/2)
      {
      self->UnSelectAllSketchesCallback(self);
      }
    else
      {
      self->SelectAllSketchesCallback(self);
      }
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::SelectAllSketchesCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }

  self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::SelectAllSketchesAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::UnSelectAllSketchesCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled)
    {
    return;
    }

  self->WidgetGroup->DispatchAction( self,
      &vtkKWEPaintbrushSelectionWidget::UnSelectAllSketchesAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::MergeSelectionCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (!self || self->WidgetState == Disabled || !self->WidgetGroup)
    {
    return;
    }

  self->WidgetGroup->DispatchAction( self,
        &vtkKWEPaintbrushSelectionWidget::MergeSelectionAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::MoveCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushSelectionWidget *self =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(w);
  if (self->WidgetState != BeginSelecting)
    {
    return;
    }

  if (!self->EnableDragAndDrop)
    {
    return; // Ignore drag.
    }

  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(self->WidgetRep);
  rep->SetInteractionState( vtkKWEPaintbrushSelectionRepresentation::
      PaintbrushRequestSketchMerge );

  if ( rep->ComputeInteractionState(
          self->Interactor->GetEventPosition()[0],
          self->Interactor->GetEventPosition()[1]) !=
              vtkKWEPaintbrushSelectionRepresentation::None )
    {
    self->Render();
    self->EventCallbackCommand->SetAbortFlag(1);
    }
}

//----------------------------------------------------------------------
// Actions that are dispatched from the WidgetGroup.
int vtkKWEPaintbrushSelectionWidget
::SelectSketchAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);

  this->DeepCopy(dispatcher);

  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkKWEPaintbrushWidget::SelectSketchEvent,
                    static_cast<void *>(rep->GetSelectedSketch()) );
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::UnselectSketchAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);

  this->DeepCopy(dispatcher);

  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkKWEPaintbrushWidget::UnselectSketchEvent,
                    static_cast<void *>(rep->GetSelectedSketch()) );
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::SelectAllSketchesAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  if (this == dispatcher)
    {
    vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
      vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);
    rep->SelectAllSketches();
    }
  else
    {
    this->DeepCopy(dispatcher);
    }

  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent( vtkKWEPaintbrushWidget::SelectAllSketchesEvent );
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::UnSelectAllSketchesAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  if (this == dispatcher)
    {
    vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
      vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);
    rep->UnSelectAllSketches();
    }
  else
    {
    this->DeepCopy(dispatcher);
    }

  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent( vtkKWEPaintbrushWidget::UnSelectAllSketchesEvent );
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::ReleaseSelectAction(vtkKWEPaintbrushSelectionWidget *vtkNotUsed(dispatcher))
{
  if (this->WidgetState == BeginSelecting)
    {
    this->WidgetState = EndSelecting;
    this->EventCallbackCommand->SetAbortFlag(1);

    if (!this->EnableDragAndDrop)
      {
      return 0; // Ignore potential drop.
      }

    // Drag and drop merge support.
    vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
      vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);
    if (rep->GetInteractionState() ==
        vtkKWEPaintbrushSelectionRepresentation::PaintbrushSketchMerge)
      {
      rep->DragAndDropMerge();
      this->Render();
      this->EventCallbackCommand->SetAbortFlag(1);
      }
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::DeleteSelectionAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);

  if (this == dispatcher)
    {
    if (rep->DeleteSelectedSketches() == 0)
      {
      return 0; // Nothing was removed.. just return.
      }
    }
  else
    {
    this->DeepCopy(dispatcher);
    }

  // Render only if needed.
  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent( vtkKWEPaintbrushWidget::DeleteSelectionEvent );
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionWidget
::MergeSelectionAction(vtkKWEPaintbrushSelectionWidget *dispatcher)
{
  vtkKWEPaintbrushSelectionRepresentation *rep = static_cast<
    vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep);

  if (this == dispatcher)
    {
    if (rep->MergeSelectedSketches(
           this->MergedSketch, this->RemoveSketchesDuringMerge) == 0)
      {
      return 0; // Nothing was merged.. just return.
      }
    }
  else
    {
    this->DeepCopy(dispatcher);
    }

  // Render only if needed.
  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent( vtkKWEPaintbrushWidget::DeleteSelectionEvent );
  return 1;
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget::DeepCopy(vtkAbstractWidget *widget)
{
  vtkKWEPaintbrushSelectionWidget *w =
    static_cast< vtkKWEPaintbrushSelectionWidget *>(widget);
  if (this == w || !w)
    {
    return;
    }

  this->WidgetState = w->WidgetState;
  if (w->WidgetGroup)
    {
    w->WidgetGroup->AddWidget(this);
    }

  if (w->WidgetRep && this->WidgetRep)
    {
    (static_cast<vtkKWEPaintbrushSelectionRepresentation *>(this->WidgetRep))->
      DeepCopy(w->WidgetRep);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget::DeleteSelectedSketches()
{
  this->WidgetGroup->DispatchAction( this,
        &vtkKWEPaintbrushSelectionWidget::DeleteSelectionAction);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget
::SetRepresentation(vtkKWEPaintbrushSelectionRepresentation *r)
{
  this->Superclass::SetWidgetRepresentation(r);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

