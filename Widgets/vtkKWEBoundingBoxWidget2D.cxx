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
#include "vtkKWEBoundingBoxWidget2D.h"
#include "vtkKWEBoundingBoxRepresentation2D.h"
#include "vtkActor.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkInteractorObserver.h"
#include "vtkHandleWidget.h"
#include "vtkPolyData.h"

vtkCxxRevisionMacro(vtkKWEBoundingBoxWidget2D, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEBoundingBoxWidget2D);

//----------------------------------------------------------------------
vtkKWEBoundingBoxWidget2D::vtkKWEBoundingBoxWidget2D()
{
  this->ShowSliceScaleBar = 1;

  // 4 handles for the 4 corners of the box.
  this->HandleWidgets = new vtkHandleWidget* [6];
  for (int i=0; i<6; i++)
    {
    this->HandleWidgets[i] = vtkHandleWidget::New();
    this->HandleWidgets[i]->SetParent(this);

    // We will decide what cursor to show.
    this->HandleWidgets[i]->ManagesCursorOff();
    }

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::LeftButtonPressEvent,
            vtkEvent::NoModifier, 0, 0, NULL,
            101001,
            this, vtkKWEBoundingBoxWidget2D::OnLeftButtonDownCallback);
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::LeftButtonReleaseEvent,
            vtkEvent::NoModifier, 0, 0, NULL,
            101002,
            this, vtkKWEBoundingBoxWidget2D::OnLeftButtonUpCallback);
  this->CallbackMapper->SetCallbackMethod(
            vtkCommand::MouseMoveEvent,
            vtkEvent::NoModifier, 0, 1, NULL,
            vtkWidgetEvent::Move,
            this, vtkKWEBoundingBoxWidget2D::OnMouseMoveCallback);

  for (int i = 0; i < 6; i++)
    {
    this->LastComputedBounds[i] = 0.0;
    this->LastComputedExtent[i] = 0;
    }

  this->SetPriority( 0.5 ); // default
}

//----------------------------------------------------------------------
vtkKWEBoundingBoxWidget2D::~vtkKWEBoundingBoxWidget2D()
{
  for (int i=0; i<6; i++)
    {
    this->HandleWidgets[i]->Delete();
    }
  delete [] this->HandleWidgets;
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::CreateDefaultRepresentation()
{
  if ( !this->WidgetRep )
    {
    this->WidgetRep = vtkKWEBoundingBoxRepresentation2D::New();
    this->WidgetRep->SetRenderer(this->CurrentRenderer);
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D
::SetRepresentation( vtkKWEBoundingBoxRepresentation2D * rep )
{
  this->Superclass::SetWidgetRepresentation( rep );
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetEnabled(int enabling)
{
  if ( enabling ) //----------------
    {
    vtkDebugMacro(<<"Enabling widget");

    if ( this->Enabled ) //already enabled, just return
      {
      return;
      }

    if ( ! this->Interactor )
      {
      vtkErrorMacro(<<"The interactor must be set prior to enabling the widget");
      return;
      }

    int X=this->Interactor->GetEventPosition()[0];
    int Y=this->Interactor->GetEventPosition()[1];

    if ( ! this->CurrentRenderer )
      {
      this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(X,Y));

      if (this->CurrentRenderer == NULL)
        {
        return;
        }
      }

    // We're ready to enable
    this->Enabled = 1;
    this->CreateDefaultRepresentation();
    this->WidgetRep->SetRenderer(this->CurrentRenderer);

    // listen for the events found in the EventTranslator
    if ( ! this->Parent )
      {
      this->EventTranslator->AddEventsToInteractor(this->Interactor,
        this->EventCallbackCommand,this->Priority);
      }
    else
      {
      this->EventTranslator->AddEventsToParent(this->Parent,
        this->EventCallbackCommand,this->Priority);
      }

    // Enable each of the handle widgets.
    for(int i=0; i< 4; i++)
      {
      if(this->HandleWidgets[i])
        {
        this->HandleWidgets[i]->SetRepresentation(
          vtkKWEBoundingBoxRepresentation2D::SafeDownCast
          (this->WidgetRep)->GetHandleRepresentation(i));
        this->HandleWidgets[i]->SetInteractor(this->Interactor);
        this->HandleWidgets[i]->GetRepresentation()->SetRenderer(
          this->CurrentRenderer);

        this->HandleWidgets[i]->SetEnabled(enabling);
        }
      }

    // Enable the slice handle widgets
    this->SetEnabledStateOfSliceScaleBar();

    if ( this->ManagesCursor )
      {
      this->WidgetRep->ComputeInteractionState(X, Y);
      this->SetCursor(this->WidgetRep->GetInteractionState());
      }

    this->WidgetRep->BuildRepresentation();
    this->CurrentRenderer->AddViewProp(this->WidgetRep);
    this->WidgetRep->VisibilityOn();

    this->InvokeEvent(vtkCommand::EnableEvent,NULL);
    }

  else //disabling------------------
    {
    vtkDebugMacro(<<"Disabling widget");

    if ( ! this->Enabled ) //already disabled, just return
      {
      return;
      }

    this->Enabled = 0;

    // don't listen for events any more
    if ( ! this->Parent )
      {
      this->Interactor->RemoveObserver(this->EventCallbackCommand);
      }
    else
      {
      this->Parent->RemoveObserver(this->EventCallbackCommand);
      }

    // Disable each of the handle widgets.
    for(int i=0; i<4; i++)
      {
      if(this->HandleWidgets[i])
        {
        this->HandleWidgets[i]->SetEnabled(enabling);
        }
      }

    // Disable the slice handle widgets
    this->SetEnabledStateOfSliceScaleBar();

    this->CurrentRenderer->RemoveViewProp(this->WidgetRep);
    this->WidgetRep->VisibilityOff();

    this->InvokeEvent(vtkCommand::DisableEvent,NULL);
    this->SetCurrentRenderer(NULL);
    }

  // Should only render if there is no parent
  if ( this->Interactor && !this->Parent )
    {
    this->Interactor->Render();
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetEnabledStateOfSliceScaleBar()
{
  if (this->ShowSliceScaleBar && this->Enabled)
    {
    // Enable each of the handle widgets.
    for(int i=4; i< 6; i++)
      {
      if(this->HandleWidgets[i])
        {
        this->HandleWidgets[i]->SetRepresentation(
          vtkKWEBoundingBoxRepresentation2D::SafeDownCast
          (this->WidgetRep)->GetHandleRepresentation(i));
        this->HandleWidgets[i]->SetInteractor(this->Interactor);
        this->HandleWidgets[i]->GetRepresentation()->SetRenderer(
          this->CurrentRenderer);
        this->HandleWidgets[i]->SetEnabled(1);
        }
      }
    }

  else if (!this->ShowSliceScaleBar && !this->Enabled)
    {
    // Disable each of the handle widgets.
    for(int i=4; i<6; i++)
      {
      if(this->HandleWidgets[i])
        {
        this->HandleWidgets[i]->SetEnabled(0);
        }
      }
    }

  else
    {
    return;
    }

  if (vtkKWEBoundingBoxRepresentation2D *rep = static_cast<
        vtkKWEBoundingBoxRepresentation2D *>(this->WidgetRep))
    {
    rep->SetShowSliceScaleBar(this->ShowSliceScaleBar);
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetShowSliceScaleBar(int i)
{
  if (i == this->ShowSliceScaleBar) { return; }
  this->ShowSliceScaleBar = i;
  this->Modified();
  this->SetEnabledStateOfSliceScaleBar();
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::OnLeftButtonDownCallback(vtkAbstractWidget *w)
{
  vtkKWEBoundingBoxWidget2D *self = reinterpret_cast<vtkKWEBoundingBoxWidget2D*>(w);
  vtkKWEBoundingBoxRepresentation2D*rep =
    reinterpret_cast<vtkKWEBoundingBoxRepresentation2D*>(self->WidgetRep);

  if (rep->GetUnplaced())
    {
    // Not yet been placed. We should try to define the widget.
    rep->SetInteractionState( vtkKWEBoundingBoxRepresentation2D::BeginDefining );
    }

  else
    {
    // The widget has been placed. We are probably trying to select a handle.
    // Lets make a request on the representation and see if we are close to one.
    rep->SetInteractionState(
        vtkKWEBoundingBoxRepresentation2D::RequestResizeBox );
    }

  // Let the representation compute itself.
  int interactionState = rep->ComputeInteractionState(
      self->Interactor->GetEventPosition()[0],
      self->Interactor->GetEventPosition()[1] );
  self->SetCursor(interactionState);

  if (interactionState != vtkKWEBoundingBoxRepresentation2D::Outside)
    {
    // Render ourself.
    self->EventCallbackCommand->SetAbortFlag(1);
    self->StartInteraction();
    self->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
    self->Interactor->Render();
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::OnLeftButtonUpCallback(vtkAbstractWidget *w)
{
  vtkKWEBoundingBoxWidget2D *self = reinterpret_cast<vtkKWEBoundingBoxWidget2D*>(w);
  vtkKWEBoundingBoxRepresentation2D *rep =
    reinterpret_cast<vtkKWEBoundingBoxRepresentation2D *>(self->WidgetRep);

  int interactionState = rep->GetInteractionState();

  // If we are defining, end that.
  if (interactionState == vtkKWEBoundingBoxRepresentation2D::Defining)
    {
    rep->SetInteractionState(vtkKWEBoundingBoxRepresentation2D::EndDefining);
    if (rep->ComputeInteractionState(
          self->Interactor->GetEventPosition()[0],
          self->Interactor->GetEventPosition()[1] ) ==
              vtkKWEBoundingBoxRepresentation2D::EndDefining )
      {
      // Still in EndDefining state. Something surely went wrong, such as the
      // user trying to place handles out of the bounds etc...
      // Go back to BeginDefining state.
      self->Interactor->Render();
      return;
      }
    }

  int extent[6];
  double bounds[6];
  rep->GetExtent(extent);
  rep->GetPolyData()->GetBounds(bounds);

  // Reset and render ourself.
  if (interactionState != vtkKWEBoundingBoxRepresentation2D::Outside &&
      interactionState != vtkKWEBoundingBoxRepresentation2D::BeginDefining)
    {
    rep->SetInteractionState( vtkKWEBoundingBoxRepresentation2D::Outside );
    self->Interactor->Render();
    self->SetCursor(vtkKWEBoundingBoxRepresentation2D::Outside);
    self->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
    }

  if (bounds[0] != self->LastComputedBounds[0] ||
      bounds[1] != self->LastComputedBounds[1] ||
      bounds[2] != self->LastComputedBounds[2] ||
      bounds[3] != self->LastComputedBounds[3] ||
      bounds[4] != self->LastComputedBounds[4] ||
      bounds[5] != self->LastComputedBounds[5] )
    {
    self->InvokeEvent( vtkKWEBoundingBoxWidget2D::BoundsChangedEvent, bounds );

    if (extent[0] != self->LastComputedExtent[0] ||
        extent[1] != self->LastComputedExtent[1] ||
        extent[2] != self->LastComputedExtent[2] ||
        extent[3] != self->LastComputedExtent[3] ||
        extent[4] != self->LastComputedExtent[4] ||
        extent[5] != self->LastComputedExtent[5] )
      {
      self->InvokeEvent( vtkKWEBoundingBoxWidget2D::ExtentsChangedEvent, extent );
      }

    for (int i = 0; i < 6; i++)
      {
      self->LastComputedBounds[i] = bounds[i];
      self->LastComputedExtent[i] = extent[i];
      }

    self->EventCallbackCommand->SetAbortFlag(1);
    self->Interactor->Render();
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::OnMouseMoveCallback(vtkAbstractWidget *w)
{
  vtkKWEBoundingBoxWidget2D *self = reinterpret_cast<vtkKWEBoundingBoxWidget2D*>(w);
  vtkKWEBoundingBoxRepresentation2D*rep =
    reinterpret_cast<vtkKWEBoundingBoxRepresentation2D *>(self->WidgetRep);

  int interactionState = rep->GetInteractionState();

  // Let the representation re-compute what the appropriate state is.
  int newInteractionState = rep->ComputeInteractionState(
        self->Interactor->GetEventPosition()[0],
        self->Interactor->GetEventPosition()[1] );

  // If we computed a different interaction state than the one we were in,
  // render in response to any changes. Also take care of trivial cases that
  // require no rendering.
  if (newInteractionState != interactionState ||
      (newInteractionState != vtkKWEBoundingBoxRepresentation2D::Inside &&
       newInteractionState != vtkKWEBoundingBoxRepresentation2D::Outside ))
    {
    self->Interactor->Render();
    self->SetCursor(newInteractionState);
    self->InvokeEvent(vtkCommand::InteractionEvent, NULL);
    }
}

//-------------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetCursor(int state)
{
  switch (state)
    {
    case vtkKWEBoundingBoxRepresentation2D::Resizing:
      this->RequestCursorShape(VTK_CURSOR_HAND);
      break;
    default:
      this->RequestCursorShape(VTK_CURSOR_ARROW);
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetProcessEvents(int pe)
{
  this->Superclass::SetProcessEvents(pe);
  for (int i=0; i<6; i++)
    {
    this->HandleWidgets[i]->SetProcessEvents(pe);
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::SetFade( int f )
{
  if (f == 1)
    {
    this->SetProcessEvents(0);
    }

  vtkKWEBoundingBoxRepresentation2D *rep =
    static_cast<vtkKWEBoundingBoxRepresentation2D *>(this->WidgetRep);
  rep->SetFade(f);
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxWidget2D::GetFade()
{
  vtkKWEBoundingBoxRepresentation2D *rep =
    static_cast<vtkKWEBoundingBoxRepresentation2D *>(this->WidgetRep);
  return rep->GetFade();
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxWidget2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

