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
#include "vtkKWEPaintbrushWidget.h"

#include "vtkKWEWidgetGroup.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushAnnotationWidget.h"
#include "vtkKWEPaintbrushSelectionWidget.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkProperty2D.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkCoordinate.h"
#include "vtkMath.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkGarbageCollector.h"

#define sign(x) ((static_cast<double>(x) < 0.0) ? (-1) : (1))

vtkCxxRevisionMacro(vtkKWEPaintbrushWidget, "$Revision: 590 $");
vtkStandardNewMacro(vtkKWEPaintbrushWidget);

//----------------------------------------------------------------------
vtkKWEPaintbrushWidget::vtkKWEPaintbrushWidget()
{
  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushInteract;
  this->ResizeStartPosition[0] = this->ResizeStartPosition[1] = -1;

  // These are the event callbacks supported by this widget
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::NoModifier, 0, 0, NULL, 
           vtkKWEPaintbrushWidget::BeginDrawStrokeEvent,
           this, vtkKWEPaintbrushWidget::BeginDrawCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ControlModifier, 0, 0, NULL, 
           vtkKWEPaintbrushWidget::BeginEraseStrokeEvent,
           this, vtkKWEPaintbrushWidget::BeginEraseCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
           vtkWidgetEvent::Move,
           this, vtkKWEPaintbrushWidget::MoveCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
           vtkKWEPaintbrushWidget::EndStrokeEvent,
           this, vtkKWEPaintbrushWidget::EndStrokeCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 8, 1,"BackSpace",
           vtkKWEPaintbrushWidget::UndoStrokeEvent,
           this, vtkKWEPaintbrushWidget::UndoCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+z
           vtkEvent::ControlModifier, 26, 0,"z",
           vtkKWEPaintbrushWidget::UndoStrokeEvent,
           this, vtkKWEPaintbrushWidget::UndoCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 40, 1, "Right",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this, vtkKWEPaintbrushWidget::RedoCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+y
           vtkEvent::ControlModifier, 25, 0,"y",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this, vtkKWEPaintbrushWidget::RedoCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ControlModifier, 8, 1,"BackSpace",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this, vtkKWEPaintbrushWidget::RedoCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 47, 1, "Delete",
           vtkWidgetEvent::Delete,
           this, vtkKWEPaintbrushWidget::DeleteCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ShiftModifier, 0, 0, NULL, 
           vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent,
           this, vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
           vtkEvent::NoModifier, 0, 0, NULL, 
           vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent,
           this, vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
           vtkEvent::ControlModifier, 0, 0, NULL, 
           vtkKWEPaintbrushWidget::BeginResizeEvent,
           this, vtkKWEPaintbrushWidget::BeginResizeShapeCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
           vtkKWEPaintbrushWidget::EndResizeEvent,
           this, vtkKWEPaintbrushWidget::EndResizeShapeCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 27, 1,"Escape",
           vtkKWEPaintbrushWidget::ToggleSelectStateEvent,
           this, vtkKWEPaintbrushWidget::ToggleSelectStateCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // Shift+
           vtkEvent::ShiftModifier, 43, 1,"plus",
           vtkKWEPaintbrushWidget::BeginNewSketchEvent,
           this, vtkKWEPaintbrushWidget::BeginNewSketchCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ShiftModifier, 40, 1, "Right",
           vtkKWEPaintbrushWidget::IncrementSketchEvent,
           this, vtkKWEPaintbrushWidget::IncrementSketchCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ShiftModifier, 38, 1, "Left",
           vtkKWEPaintbrushWidget::DecrementSketchEvent,
           this, vtkKWEPaintbrushWidget::DecrementSketchCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 109, 0, "m",
           vtkKWEPaintbrushWidget::MergeSelectionEvent,
           this, vtkKWEPaintbrushWidget::MergeSelectionCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+a
           vtkEvent::ControlModifier, 1, 0,"a",
           vtkKWEPaintbrushWidget::ToggleSelectAllSketchesEvent,
           this, vtkKWEPaintbrushWidget::ToggleSelectAllSketchesCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // "<"
           vtkEvent::ShiftModifier, 60, 0,"less",
           vtkKWEPaintbrushWidget::DecreaseOpacityEvent,
           this, vtkKWEPaintbrushWidget::DecreaseOpacityCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent, // ">"
           vtkEvent::ShiftModifier, 62, 0,"greater",
           vtkKWEPaintbrushWidget::IncreaseOpacityEvent,
           this, vtkKWEPaintbrushWidget::IncreaseOpacityCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::EnterEvent,
           vtkKWEPaintbrushWidget::EnterEvent,
           this, vtkKWEPaintbrushWidget::EnterWidgetCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeaveEvent,
           vtkKWEPaintbrushWidget::LeaveEvent,
           this, vtkKWEPaintbrushWidget::LeaveWidgetCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 108, 0, "l",
           vtkKWEPaintbrushWidget::ToggleSketchMutabilityEvent,
           this, vtkKWEPaintbrushWidget::ToggleSketchMutabilityCallback);


  this->PaintbrushMode = vtkKWEPaintbrushWidget::Edit;

  // The selection widget will be used if we enter a "select" mode.
  this->PaintbrushSelectionWidget = vtkKWEPaintbrushSelectionWidget::New();
  this->PaintbrushSelectionWidget->SetParent(this);

  this->PaintbrushAnnotationWidget = vtkKWEPaintbrushAnnotationWidget::New();
  this->PaintbrushAnnotationWidget->PaintbrushWidget = this;

  this->CreateDefaultRepresentation();
}

//----------------------------------------------------------------------
vtkKWEPaintbrushWidget::~vtkKWEPaintbrushWidget()
{
  this->PaintbrushAnnotationWidget->Delete();
  this->PaintbrushSelectionWidget->Delete();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::CreateDefaultRepresentation()
{
  // Creates only the 2D rep by default for now
  // We will accomodate the 3D rep later.
  if ( ! this->WidgetRep )
    {
    vtkKWEPaintbrushRepresentation2D *rep =
      vtkKWEPaintbrushRepresentation2D::New();
    this->SetRepresentation(rep);
    rep->Delete();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::SetEnabled(int enabling)
{
  vtkKWEPaintbrushRepresentation *rep 
      = vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  // The handle widgets are not actually enabled until they are placed.
  // The handle widgets take their representation from the vtkKWEPaintbrushRepresentation.
  if ( enabling )
    {
    rep->VisibilityOn();
    rep->SetStateToInteract();
    }
  else
    {
    rep->VisibilityOff();
    }

  this->Superclass::SetEnabled(enabling);  
  this->PaintbrushAnnotationWidget->SetInteractor(this->Interactor);
  this->PaintbrushAnnotationWidget->SetEnabled(enabling);

  // The selection widget is enabled only if we are in selection mode and if
  // the parent widget (ourself) is enabled.
  this->PaintbrushSelectionWidget->SetInteractor(this->Interactor);
  this->PaintbrushSelectionWidget->SetEnabled( (this->PaintbrushMode 
    == vtkKWEPaintbrushWidget::Select && enabling == 1) ? 1 : 0 );

  if (enabling)
    {
    rep->InstallPipeline();
    }
  else
    {
    rep->UnInstallPipeline();
    }
}

// The following methods are the callbacks that the widget responds to. 
//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::BeginDrawCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if ( self->WidgetState == PaintbrushDisabled || 
      !self->WidgetGroup)
    {
    return;
    }

  // If we are in "Select" mode, let the PaintbrushSelectionWidget handle it.
  if (self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    // Invoke an event for the PaintbrushSelectionWidget. The 
    // PaintbrushSelectionWidget's parent is ourself and the widget will
    // respond to our invocation of the the event.
    self->InvokeEvent( vtkCommand::LeftButtonPressEvent);

    // Propagate abort events from the selection widget.
    self->EventCallbackCommand->SetAbortFlag( self->
      PaintbrushSelectionWidget->GetEventCallbackCommand()->GetAbortFlag());
    return;
    }

  // We are in "Edit" mode.
  // Check if we are inside the canvas. If we aren't just return.
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  if (!rep->IsInsideCanvas( self->Interactor->GetEventPosition() ))
    {
    return;
    }
  
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::BeginDrawAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::BeginDrawStrokeEvent );
  self->EventCallbackCommand->SetAbortFlag(1);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::BeginEraseCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if ( self->WidgetState == PaintbrushDisabled || 
      !self->WidgetGroup                       ||
      self->PaintbrushMode == vtkKWEPaintbrushWidget::Select )
    {
    return;
    }

  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::BeginEraseAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::BeginEraseStrokeEvent );
  self->EventCallbackCommand->SetAbortFlag(1);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::MoveCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState == PaintbrushDisabled)
    {
    return;
    }  

  if (self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    // Invoke an event for the PaintbrushSelectionWidget. The 
    // PaintbrushSelectionWidget's parent is ourself and the widget will
    // respond to our invocation of the the event.
    self->InvokeEvent( vtkCommand::MouseMoveEvent);
    return;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);

  double eventPos[2] = { self->Interactor->GetEventPosition()[0],
                         self->Interactor->GetEventPosition()[1] };

  if (self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushInteract ||
      self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDraw     || 
      self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushErase)
    {
    // We are in Interact mode.. The template should just
    // hover around along with the cursor.
    int activate = rep->ActivateShapeOutline(static_cast<int>(eventPos[0]),
                                             static_cast<int>(eventPos[1]));
    if ( activate == 1)
      {
      self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::HoverAction);
      }
    else if ( activate == -1 )
      {
        self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::LeaveWidgetAction);
      }
    }

  if (self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushResize ||
      self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushIsotropicResize)
    {

    // Compute the distance in world co-ordinates from the previous point
    // This will determine how much we resize.. If we moved up, we will
    // grow. If we moved down, we will shrink
  
    vtkCoordinate *c = vtkCoordinate::New();
    c->SetCoordinateSystemToDisplay();
    c->SetValue(eventPos[0],eventPos[1],0.0);
    double * p = c->GetComputedWorldValue(self->GetCurrentRenderer());
    double p1[3] = { p[0], p[1], p[2] };
    c->SetValue( self->ResizeStartPosition[0],
                 self->ResizeStartPosition[1], 
                 0.0);
    p = c->GetComputedWorldValue(self->GetCurrentRenderer());
    double p2[3] = { p[0], p[1], p[2] };
    c->Delete();
    
    // Compute the resize sign.. (grow or shrink)

    int signVal;
    if (self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushResize)
      {
      unsigned int idx = 0;
      double max = fabs(p1[0] - p2[0]);
      for (unsigned int i=1; i<3; i++)
        {
        if (fabs(p1[i] - p2[i]) > max)
          {
          idx = i;
          max = fabs(p1[i] - p2[i]);
          }
        }
      signVal = sign(p1[idx]-p2[idx]);
      }
    else
      {
      const int dx = static_cast<int>(eventPos[0] - self->ResizeStartPosition[0]);
      const int dy = static_cast<int>(eventPos[1] - self->ResizeStartPosition[1]);
      signVal = sign(fabs(static_cast<double>(dy)) > fabs(static_cast<double>(dx)) ? dy : dx);
      double a = fabs(static_cast<double>(dy));
      double b = fabs(static_cast<double>(dx));
      if (a > b)
        {
        signVal = sign(dy);
        }
      else
        {
        signVal = sign(dx);
        }
      }


    double distance[3] = {  fabs(p2[0] - p1[0]) * signVal,
                            fabs(p2[1] - p1[1]) * signVal,
                            fabs(p2[2] - p1[2]) * signVal };

    // Let the representation do the actual resize

    if (rep->ResizeShape(
          distance, 
          (self->WidgetState == PaintbrushIsotropicResize) ? 
              vtkKWEPaintbrushShape::PaintbrushResizeIsotropic : 
              vtkKWEPaintbrushShape::PaintbrushResizeAnisotropic ))
      {
      self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::ResizeAction);
      }

    self->ResizeStartPosition[0] = static_cast<int>(eventPos[0]);
    self->ResizeStartPosition[1] = static_cast<int>(eventPos[1]);
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::EndStrokeCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self)
    {
    if (self->PaintbrushMode == vtkKWEPaintbrushWidget::Select &&
        self->WidgetState != PaintbrushDisabled)
      {
      // Invoke an event for the PaintbrushSelectionWidget. The 
      // PaintbrushSelectionWidget's parent is ourself and the widget will
      // respond to our invocation of the the event.
      self->InvokeEvent( vtkCommand::LeftButtonReleaseEvent );
      return;
      }
    else if (self->WidgetState == PaintbrushDisabled || !self->WidgetGroup
          || self->WidgetState == PaintbrushInteract )
      {
      return;
      }
    
    self->WidgetGroup->DispatchAction( self, 
         &vtkKWEPaintbrushWidget::EndStrokeAction);
    self->InvokeEvent( vtkKWEPaintbrushWidget::EndStrokeEvent );
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::UndoCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self)
    {
    if (self->PaintbrushMode == vtkKWEPaintbrushWidget::Select &&
        self->WidgetState != PaintbrushDisabled)
      {
      // Invoke an event for the PaintbrushSelectionWidget. The 
      // PaintbrushSelectionWidget's parent is ourself and the widget will
      // respond to our invocation of the the event.
      self->InvokeEvent( vtkCommand::KeyPressEvent);
      return;
      }
    else if (self->WidgetState != PaintbrushInteract || !self->WidgetGroup)
      {
      return;
      }
    }
  
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::UndoAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::DeleteCallback(vtkAbstractWidget *w)
{
  // Similar to undo.. stuff is removed from the list forever..
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self)
    {
    if (self->PaintbrushMode == vtkKWEPaintbrushWidget::Select &&
        self->WidgetState != PaintbrushDisabled)
      {
      // Invoke an event for the PaintbrushSelectionWidget. The 
      // PaintbrushSelectionWidget's parent is ourself and the widget will
      // respond to our invocation of the the event.
      self->InvokeEvent( vtkCommand::KeyPressEvent);
      return;
      }
    else if (self->WidgetState != PaintbrushInteract || !self->WidgetGroup)
      {
      return;
      }
    }
  
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::DeleteAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::RedoCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self || self->WidgetState == PaintbrushDisabled || !self->WidgetGroup
      || self->WidgetState != vtkKWEPaintbrushWidget::PaintbrushInteract
      || self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }
  
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::RedoAction);
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::BeginNewSketchCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != vtkKWEPaintbrushWidget::PaintbrushInteract ||
      self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }
  
  // If the user has set a limit on the number of sketches in the drawing,
  // obey the limit.
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  int maxSketchesAllowed = 
    rep->GetPaintbrushDrawing()->GetMaximumNumberOfSketches();
  if (maxSketchesAllowed &&
      rep->GetPaintbrushDrawing()->GetNumberOfItems() == maxSketchesAllowed)
    {
    return; // We hit the limit.
    }

  self->WidgetGroup->DispatchAction(self, 
      &vtkKWEPaintbrushWidget::BeginNewSketchAction);
  self->PaintbrushAnnotationWidget->AnnotateNewSketch();  
  self->InvokeEvent( vtkKWEPaintbrushWidget::BeginNewSketchEvent );
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::IncrementSketchCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != vtkKWEPaintbrushWidget::PaintbrushInteract ||
      self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  if (rep->GetPaintbrushDrawing()->GetMaximumNumberOfSketches() == 1)
    {
    return; // We just got one sketch. Why annotate it ?
    }
  
  self->WidgetGroup->DispatchAction(self, 
      &vtkKWEPaintbrushWidget::IncrementSketchAction);
  self->PaintbrushAnnotationWidget->AnnotateIncrementSketch();
  self->InvokeEvent( vtkKWEPaintbrushWidget::IncrementSketchEvent );
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::DecrementSketchCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != vtkKWEPaintbrushWidget::PaintbrushInteract ||
      self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  if (rep->GetPaintbrushDrawing()->GetMaximumNumberOfSketches() == 1)
    {
    return; // We just got one sketch. Why annotate it ?
    }
  
  self->WidgetGroup->DispatchAction(self, 
      &vtkKWEPaintbrushWidget::DecrementSketchAction);
  self->PaintbrushAnnotationWidget->AnnotateDecrementSketch();
  self->InvokeEvent( vtkKWEPaintbrushWidget::DecrementSketchEvent );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::BeginResizeShapeCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self || self->WidgetState != PaintbrushInteract || !self->WidgetGroup
      || self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  if (!rep->IsInsideCanvas( self->Interactor->GetEventPosition() ))
    {
    return;
    }
  
  self->WidgetGroup->DispatchAction(
      self, &vtkKWEPaintbrushWidget::BeginResizeAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::BeginResizeEvent );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback(vtkAbstractWidget *w)
{
  // TODO : Write the corresponding actions for it
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != PaintbrushInteract ||
      self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }
  
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
  if (!rep->IsInsideCanvas( self->Interactor->GetEventPosition() ))
    {
    return;
    }

  self->WidgetGroup->DispatchAction(
      self, &vtkKWEPaintbrushWidget::BeginIsotropicResizeAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent );
}


//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::EndResizeShapeCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self || self->WidgetState == PaintbrushDisabled
      || self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    return;
    }
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::EndResizeAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::EndResizeEvent );
}

//----------------------------------------------------------------------
// The motivation for this method is strange.. Let me explain.. The widget
// by default grabs focus during draws and erases, resize, anisotropic 
// resize etc.. Sometimes, you may be drawing on an image actor where you
// have other callbacks mapped onto similar events.. (for instance window
// /level, zoom, translate etc). This method toggles the select state
// of the widget
void vtkKWEPaintbrushWidget::ToggleSelectStateCallback(vtkAbstractWidget *w)
{
  // TODO : Write the corresponding actions for it
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self)
    {
    return;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);

  if (self->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    {
    self->PaintbrushAnnotationWidget->AnnotateString("Brush enabled");  
    self->WidgetState = vtkKWEPaintbrushWidget::PaintbrushInteract;
    self->PaintbrushSelectionWidget->ProcessEventsOn();
    rep->SetStateToInteract();
    rep->SetShapeOutlineVisibility( self->PaintbrushMode 
                == vtkKWEPaintbrushWidget::Select ? 0 : 1 );
    }
  else
    {
    self->PaintbrushAnnotationWidget->AnnotateString("Brush disabled");  
    self->WidgetState = vtkKWEPaintbrushWidget::PaintbrushDisabled;
    self->ResizeStartPosition[0] = self->ResizeStartPosition[1] = -1;
    self->PaintbrushSelectionWidget->ProcessEventsOff();
    rep->SetStateToDisabled();
    }

  self->SetCursor( rep->GetInteractionState() );  
  self->EventCallbackCommand->SetAbortFlag(1);
  w->Render();
  self->InvokeEvent( vtkKWEPaintbrushWidget::ToggleSelectStateEvent );  
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::MergeSelectionCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self && self->WidgetState != PaintbrushDisabled 
           && self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    // Invoke an event for the PaintbrushSelectionWidget. The 
    // PaintbrushSelectionWidget's parent is ourself and the widget will
    // respond to our invocation of the the event.
    self->InvokeEvent( vtkCommand::KeyPressEvent);
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::ToggleSelectAllSketchesCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self && self->WidgetState != PaintbrushDisabled 
           && self->PaintbrushMode == vtkKWEPaintbrushWidget::Select)
    {
    // Invoke an event for the PaintbrushSelectionWidget. The 
    // PaintbrushSelectionWidget's parent is ourself and the widget will
    // respond to our invocation of the the event.
    self->InvokeEvent( vtkCommand::KeyPressEvent );
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::ToggleSketchMutabilityCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self && self->WidgetState != PaintbrushDisabled 
           && self->PaintbrushMode == vtkKWEPaintbrushWidget::Edit)
    {
    vtkKWEPaintbrushRepresentation *rep = 
      vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
    vtkKWEPaintbrushSketch * sketch 
      = rep->GetPaintbrushDrawing()->GetItem(rep->GetSketchIndex());
    vtkKWEPaintbrushProperty *property = sketch->GetPaintbrushProperty();
    property->SetMutable( 1- property->GetMutable() );
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::DecreaseOpacityCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != PaintbrushDisabled)
    {
    vtkKWEPaintbrushRepresentation *rep = 
      vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
    if (rep->DecreaseOpacity())
      {
      self->Render();
      }
    }    
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::IncreaseOpacityCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (self->WidgetState != PaintbrushDisabled)
    {
    vtkKWEPaintbrushRepresentation *rep = 
      vtkKWEPaintbrushRepresentation::SafeDownCast(self->WidgetRep);
    if (rep->IncreaseOpacity())
      {
      self->Render();
      }
    }    
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::EnterWidgetCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self || self->WidgetState != PaintbrushDisabled)
    {
    return;
    }
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::EnterWidgetAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::EnterEvent );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::LeaveWidgetCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
  if (!self || self->WidgetState == PaintbrushDisabled)
    {
    return;
    }
  self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::LeaveWidgetAction);
  self->InvokeEvent( vtkKWEPaintbrushWidget::LeaveEvent );  
}

//----------------------------------------------------------------------
// Geometric Actions / states that the representation can conform to.
int vtkKWEPaintbrushWidget::BeginDrawAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }
  
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);
  
  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushDraw;
  rep->SetStateToDraw();

  if (dispatcher == this)
    {
    // Make sure that the current shape position is updated.
    rep->ActivateShapeOutline( this->Interactor->GetEventPosition()[0],
                               this->Interactor->GetEventPosition()[1] );
    rep->BeginNewStroke();
    // Get the current position from the active widget.
    double d[3];
    vtkKWEPaintbrushRepresentation *dispatcher_rep = 
      vtkKWEPaintbrushRepresentation::SafeDownCast(dispatcher->WidgetRep);

    dispatcher_rep->GetCurrentShapePosition(d[0], d[1], d[2]);
    rep->CreateShapeOutline(d);

    // If the draw action results in a draw or an erase, do it on the active widget
    if ((rep->GetInteractionState() == PaintbrushDraw || 
         rep->GetInteractionState() == PaintbrushErase))
      {
      rep->GetPaintbrushDrawing()->
        AddShapeToCurrentStroke( rep->GetSketchIndex(), d );
      }
    }
  else
    {
    rep->DeepCopy(dispatcher->WidgetRep);
    }

  this->SetCursor( rep->GetInteractionState() );
  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  return 1;
}
    
//----------------------------------------------------------------------
int vtkKWEPaintbrushWidget::
EndStrokeAction( vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher) )
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushInteract;
  rep->SetStateToInteract();

  // Remove focus
  this->SetCursor( rep->GetInteractionState() );
  this->EventCallbackCommand->SetAbortFlag(0);
  this->InvokeEvent(vtkKWEPaintbrushWidget::EndStrokeEvent, NULL);
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushWidget::BeginEraseAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushErase;
  rep->SetStateToErase();

  if (dispatcher == this)
    {
    rep->BeginNewStroke();
    }
  else
    {
    rep->DeepCopy(dispatcher->WidgetRep);
    }

  this->SetCursor( rep->GetInteractionState() );
  this->Render();
  this->EventCallbackCommand->SetAbortFlag(1);
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushWidget::HoverAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  // Get the current position from the active widget.
  double d[3];
  vtkKWEPaintbrushRepresentation *dispatcher_rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(dispatcher->WidgetRep);
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  dispatcher_rep->GetCurrentShapePosition(d[0], d[1], d[2]);
  rep->SetShapeOutlineVisibility(1);
  rep->CreateShapeOutline(d);

  // If the hover results in a draw or an erase, do it on the active widget
  if (this == dispatcher && 
      (rep->GetInteractionState() == PaintbrushDraw || 
       rep->GetInteractionState() == PaintbrushErase))
    {
    rep->GetPaintbrushDrawing()->
      AddShapeToCurrentStroke( rep->GetSketchIndex(), d );
    }

  this->SetCursor( PaintbrushDraw );
  this->Render();
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushWidget::UndoAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  if (dispatcher == this)
    {
    if (rep->UndoStroke() == 0)
      {
      return 0;
      }
    }

  this->EventCallbackCommand->SetAbortFlag(1); // We are definitely selected
  this->Render();
  this->InvokeEvent( vtkKWEPaintbrushWidget::UndoStrokeEvent );
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::RedoAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  if (dispatcher == this)
    {
    if (rep->RedoStroke() == 0)
      {
      return 0;
      }
    }

  this->EventCallbackCommand->SetAbortFlag(1); // We are definitely selected
  this->Render();
  this->InvokeEvent( vtkKWEPaintbrushWidget::RedoStrokeEvent );
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::BeginNewSketchAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  if (dispatcher == this)
    {
    rep->BeginNewSketch();
    }
  else
    {
    rep->DeepCopy(dispatcher->WidgetRep);
    }  
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget
::IncrementSketchAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);
  rep->IncrementSketch();
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget
::DecrementSketchAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);
  rep->DecrementSketch();
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushWidget::DeleteAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  if (dispatcher == this)
    {
    if (rep->RemoveLastStroke() == 0)
      {
      return 0;
      }
    }

  this->Render();
  this->InvokeEvent( vtkKWEPaintbrushWidget::DeleteStrokeEvent );
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::BeginResizeAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }
  
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  rep->SetStateToResize();
  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushResize;

  if (dispatcher == this)
    {
    this->ResizeStartPosition[0] = this->Interactor->GetEventPosition()[0];
    this->ResizeStartPosition[1] = this->Interactor->GetEventPosition()[1];
    this->PaintbrushAnnotationWidget->AnnotateSize();  
    }
  
  this->SetCursor( rep->GetInteractionState() );
  this->EventCallbackCommand->SetAbortFlag(1); // We are definitely selected
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::BeginIsotropicResizeAction(
                     vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }
  
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushIsotropicResize;
  rep->SetStateToIsotropicResize();

  if (dispatcher == this)
    {
    this->ResizeStartPosition[0] = this->Interactor->GetEventPosition()[0];
    this->ResizeStartPosition[1] = this->Interactor->GetEventPosition()[1];
    this->PaintbrushAnnotationWidget->AnnotateSize();  
    }

  this->SetCursor( rep->GetInteractionState() );
  // We are definitely selected
  this->EventCallbackCommand->SetAbortFlag(1);
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::ResizeAction(vtkKWEPaintbrushWidget *dispatcher)
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }

  if (this == dispatcher)
    {
    // Compute the distance in world co-ordinates from the previous point
    // This will determine how much we resize.. If we moved up, we will
    // grow. If we moved down, we will shrink
  
    double eventPos[2] = { this->Interactor->GetEventPosition()[0],
                           this->Interactor->GetEventPosition()[1] };

    vtkCoordinate *c = vtkCoordinate::New();
    c->SetCoordinateSystemToDisplay();
    c->SetValue(eventPos[0],eventPos[1],0.0);
    double * p = c->GetComputedWorldValue(this->GetCurrentRenderer());
    double p1[3] = { p[0], p[1], p[2] };
    c->SetValue( this->ResizeStartPosition[0],
                 this->ResizeStartPosition[1], 
                 0.0);
    p = c->GetComputedWorldValue(this->GetCurrentRenderer());
    double p2[3] = { p[0], p[1], p[2] };
    c->Delete();
    
    // Compute the resize sign.. (grow or shrink)

    int signVal;
    if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushResize)
      {
      unsigned int idx = 0;
      double max = fabs(p1[0] - p2[0]);
      for (unsigned int i=1; i<3; i++)
        {
        if (fabs(p1[i] - p2[i]) > max)
          {
          idx = i;
          max = fabs(p1[i] - p2[i]);
          }
        }
      signVal = sign(p1[idx]-p2[idx]);
      }
    else
      {
      signVal = sign(eventPos[1] - this->ResizeStartPosition[1]);
      }

    this->ResizeFactor[0] = fabs(p2[0] - p1[0]) * signVal;
    this->ResizeFactor[1] = fabs(p2[1] - p1[1]) * signVal;
    this->ResizeFactor[2] = fabs(p2[2] - p1[2]) * signVal;
    
    // Throw some annotation up on the screen.
    this->PaintbrushAnnotationWidget->AnnotateSize();    
    }
  else
    {
    this->DeepCopy(dispatcher); // synchronize states with the dispatcher
    }
  
  this->Render();
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget
::EndResizeAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    { 
    return 0; 
    }
  
  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);
  
  this->WidgetState = vtkKWEPaintbrushWidget::PaintbrushInteract;
  this->ResizeStartPosition[0] = this->ResizeStartPosition[1] = -1;
  rep->SetStateToInteract();
  this->EventCallbackCommand->SetAbortFlag(0); // remove focus

  this->SetCursor( rep->GetInteractionState() );
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::
ToggleSelectStateAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  // TODO, probably move some stuffs from the callback here
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::
EnterWidgetAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  // TODO if you want to implement anything special
  return 1;
}

//-------------------------------------------------------------------------
int vtkKWEPaintbrushWidget::
LeaveWidgetAction(vtkKWEPaintbrushWidget *vtkNotUsed(dispatcher))
{
  if (this->WidgetState == vtkKWEPaintbrushWidget::PaintbrushDisabled)
    {
    return 0;
    }

  vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep);

  if ( rep->GetShapeOutlineVisibility() )
  {
    //clear string
    this->PaintbrushAnnotationWidget->AnnotateString("");
    rep->SetShapeOutlineVisibility(0);
    this->Render();
  }
  return 1;
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushWidget::DeepCopy(vtkAbstractWidget *widget)
{
  vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::SafeDownCast(widget);
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
    (static_cast<vtkKWEPaintbrushRepresentation *>(this->WidgetRep))->DeepCopy(w->WidgetRep);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::SetRepresentation( vtkKWEPaintbrushRepresentation * r )
{
  this->PaintbrushAnnotationWidget->SetRepresentation(
                        r ? r->GetAnnotation() : NULL);
  this->PaintbrushSelectionWidget->SetRepresentation(
                r ? r->GetSelectionRepresentation() : NULL);
  this->Superclass::SetWidgetRepresentation(r);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::SetPaintbrushMode( int m )
{
  if (m == this->PaintbrushMode)
    {
    return;
    }

  // Enable the selection widget in select mode.
  this->PaintbrushSelectionWidget->SetEnabled( (m
    == vtkKWEPaintbrushWidget::Select && this->Enabled == 1) ? 1 : 0 );

  // turn off the hovering brush if we are in selection mode.
  vtkKWEPaintbrushRepresentation * rep = static_cast< 
    vtkKWEPaintbrushRepresentation * >(this->WidgetRep);
  rep->SetShapeOutlineVisibility( m == vtkKWEPaintbrushWidget::Select ? 0 : 1 );

  // Cursor is a hand cursor in selection mode, crosshair in edit mode
  //this->RequestCursorShape( m == vtkKWEPaintbrushWidget::Select ? 
  //                        VTK_CURSOR_HAND : VTK_CURSOR_CROSSHAIR);

  this->PaintbrushMode = m;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::SetProcessEvents(int pe)
{
  this->Superclass::SetProcessEvents(pe);
  if (vtkKWEPaintbrushRepresentation *rep = 
    vtkKWEPaintbrushRepresentation::SafeDownCast(this->WidgetRep))
  {
  // If we aren't receiving any events, don't show the annoying shape outline.
  rep->SetShapeOutlineVisibility( pe && 
                                  this->WidgetState != PaintbrushDisabled && 
                                  this->PaintbrushMode != Select );
  }

  this->PaintbrushSelectionWidget->SetProcessEvents(pe);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::SetCursor(int vtkNotUsed(nteractionstate))
{
  /*
  if (this->GetPaintbrushMode() == Select)
    {
    // Hand cursor in selection mode
    this->RequestCursorShape( VTK_CURSOR_HAND );
    }

  // Cursor is a hand cursor in selection mode, crosshair in edit mode
  else if (interactionstate == vtkKWEPaintbrushRepresentation::PaintbrushResize ||
           interactionstate == vtkKWEPaintbrushRepresentation::PaintbrushIsotropicResize)
    {
    // Resize cursor
    this->RequestCursorShape( VTK_CURSOR_SIZEALL );
    }
  
  else
    {
    // the default is a crosshair cursor.
    this->RequestCursorShape( VTK_CURSOR_CROSSHAIR);
    }
  */
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent); 
}
