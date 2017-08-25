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
#include "vtkKWEPaintbrushAnnotationWidget.h"

#include "vtkKWEPaintbrushAnnotationRepresentation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkTextProperty.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkCoordinate.h"
#include "vtkMath.h"
#include <vtksys/ios/sstream>

#define sign(x) ((((double)x) < 0.0) ? (-1) : (1))

vtkCxxRevisionMacro(vtkKWEPaintbrushAnnotationWidget, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushAnnotationWidget);

//----------------------------------------------------------------------
vtkKWEPaintbrushAnnotationWidget::vtkKWEPaintbrushAnnotationWidget()
{
  this->WidgetState = vtkKWEPaintbrushAnnotationWidget::Start;

  // These are the event callbacks supported by this widget
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this, vtkKWEPaintbrushAnnotationWidget::MoveCallback);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::TimerEvent,
                                          vtkWidgetEvent::TimedOut,
                                          this, vtkKWEPaintbrushAnnotationWidget::HoverCallback);

  this->Movement         = 0;
  this->TimerDuration    = 5000;
  this->TimerId          = -1;
  this->PaintbrushWidget = NULL;
  this->CreateDefaultRepresentation();
}

//----------------------------------------------------------------------
vtkKWEPaintbrushAnnotationWidget::~vtkKWEPaintbrushAnnotationWidget()
{
  if (    this->WidgetState == vtkKWEPaintbrushAnnotationWidget::Timing
       && this->TimerId != -1 && this->Interactor)
    {
    this->Interactor->DestroyTimer(this->TimerId);
    this->TimerId = -1;
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::CreateDefaultRepresentation()
{
  // Creates only the 2D rep by default for now
  // We will accomodate the 3D rep later.
  if ( ! this->WidgetRep )
    {
    vtkKWEPaintbrushAnnotationRepresentation *rep =
      vtkKWEPaintbrushAnnotationRepresentation::New();
    this->WidgetRep = rep;
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::SetEnabled(int enabling)
{
  // The handle widgets are not actually enabled until they are placed.
  // The handle widgets take their representation from the vtkKWEPaintbrushAnnotationRepresentation.
  if ( enabling )
    {
    vtkKWEPaintbrushAnnotationRepresentation *rep
      = vtkKWEPaintbrushAnnotationRepresentation::SafeDownCast(this->WidgetRep);
    rep->VisibilityOn();
    }
  else
    {
    (vtkKWEPaintbrushAnnotationRepresentation::SafeDownCast(
                          this->WidgetRep))->VisibilityOff();
    }

  this->Superclass::SetEnabled(enabling);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotateString( const char *s )
{
  if (vtkKWEPaintbrushAnnotationRepresentation *rep
     = static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep))
    {
    rep->SetAnnotation(s);
    this->SetWidgetState( vtkKWEPaintbrushAnnotationWidget::BeginTimer );
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotateIncrementSketch()
{
  if (vtkKWEPaintbrushRepresentation *rep = static_cast<
    vtkKWEPaintbrushRepresentation*>(this->PaintbrushWidget->GetRepresentation()))
    {
    if (vtkKWEPaintbrushSketch *sketch = rep->GetPaintbrushDrawing()->
                                            GetItem(rep->GetSketchIndex()))
      {
      vtkKWEPaintbrushProperty * sketchProperty = sketch->GetPaintbrushProperty();

      vtksys_ios::ostringstream o;
      o << "Sketch " << rep->GetSketchIndex();
      if (sketchProperty->GetIdentifier())
        {
        o << ": " << sketchProperty->GetIdentifier();
        }
      o << ends;

      // Annotation color to match the sketch color.
      static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep)->
        GetTextProperty()->SetColor( sketchProperty->GetColor() );

      this->AnnotateString(o.str().c_str());
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotateDecrementSketch()
{
  if (vtkKWEPaintbrushRepresentation *rep = static_cast<
    vtkKWEPaintbrushRepresentation*>(this->PaintbrushWidget->GetRepresentation()))
    {
    if (vtkKWEPaintbrushSketch *sketch = rep->GetPaintbrushDrawing()->
                                            GetItem(rep->GetSketchIndex()))
      {
      vtkKWEPaintbrushProperty * sketchProperty = sketch->GetPaintbrushProperty();

      vtksys_ios::ostringstream o;
      o << "Sketch " << rep->GetSketchIndex();
      if (sketchProperty->GetIdentifier())
        {
        o << ": " << sketchProperty->GetIdentifier();
        }
      o << ends;


      // Annotation color to match the sketch color.
      static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep)->
        GetTextProperty()->SetColor( sketchProperty->GetColor() );

      this->AnnotateString(o.str().c_str());
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotateNewSketch()
{
  if (vtkKWEPaintbrushRepresentation *rep = static_cast<
    vtkKWEPaintbrushRepresentation*>(this->PaintbrushWidget->GetRepresentation()))
    {
    if (vtkKWEPaintbrushSketch *sketch = rep->GetPaintbrushDrawing()->
                                            GetItem(rep->GetSketchIndex()))
      {
      vtkKWEPaintbrushProperty * sketchProperty = sketch->GetPaintbrushProperty();

      vtksys_ios::ostringstream o;
      o << "New sketch " << rep->GetSketchIndex();
      if (sketchProperty->GetIdentifier())
        {
        o << ": " << sketchProperty->GetIdentifier();
        }
      o << ends;

      // Annotation color to match the sketch color.
      static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep)->
        GetTextProperty()->SetColor( sketchProperty->GetColor() );

      this->AnnotateString(o.str().c_str());
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotateSize()
{
  if (vtkKWEPaintbrushRepresentation *rep = static_cast<
    vtkKWEPaintbrushRepresentation*>(this->PaintbrushWidget->GetRepresentation()))
    {
    char str[256];
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->GetAnnotation(str);

    if (vtkKWEPaintbrushSketch *sketch = rep->GetPaintbrushDrawing()->
                                            GetItem(rep->GetSketchIndex()))
      {
      vtkKWEPaintbrushProperty * sketchProperty = sketch->GetPaintbrushProperty();

      // Annotation color to match the sketch color.
      static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep)->
        GetTextProperty()->SetColor( sketchProperty->GetColor() );
      }
    this->AnnotateString(str);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::AnnotatePaintbrushMode()
{
  vtksys_stl::string annotationString =
    (this->PaintbrushWidget->GetPaintbrushMode()
       == vtkKWEPaintbrushWidget::Edit) ? "EditMode" : "Select Mode";

  if (vtkKWEPaintbrushRepresentation *rep = static_cast<
    vtkKWEPaintbrushRepresentation*>(this->PaintbrushWidget->GetRepresentation()))
    {
    if (vtkKWEPaintbrushSketch *sketch = rep->GetPaintbrushDrawing()->
                                            GetItem(rep->GetSketchIndex()))
      {
      vtkKWEPaintbrushProperty * sketchProperty = sketch->GetPaintbrushProperty();

      // Annotation color to match the sketch color.
      static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep)->
        GetTextProperty()->SetColor( sketchProperty->GetColor() );
      }
    this->AnnotateString(annotationString.c_str());
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::SetWidgetState( int w )
{
  if (this->WidgetState != w)
    {
    bool needRender = false;
    vtkKWEPaintbrushAnnotationRepresentation *rep
      = static_cast< vtkKWEPaintbrushAnnotationRepresentation *>(this->WidgetRep);

    // Create a new timer.
    if (w == vtkKWEPaintbrushAnnotationWidget::BeginTimer)
      {
      if ( this->WidgetState == vtkKWEPaintbrushAnnotationWidget::Timing )
        {
        this->Interactor->DestroyTimer(this->TimerId);
        this->TimerId = -1;
        }

      this->TimerId = this->Interactor->CreateOneShotTimer(this->TimerDuration);
      this->WidgetState = vtkKWEPaintbrushAnnotationWidget::Timing;
      this->Movement = 0;
      if (rep)
        {
        rep->VisibilityOn();
        rep->BuildRepresentation();
        needRender = true;
        }
      }

    // Remove any timers
    if (w == vtkKWEPaintbrushAnnotationWidget::EndTimer)
      {
      if ( this->WidgetState == vtkKWEPaintbrushAnnotationWidget::Timing)
        {
        this->Interactor->DestroyTimer(this->TimerId);
        this->TimerId = -1;
        this->WidgetState = vtkKWEPaintbrushAnnotationWidget::Start;
        if (rep)
          {
          rep->VisibilityOff();
          needRender = true;
          }
        }
      }
    if (needRender)
      {
      this->Render();
      }
    }
}

// The following methods are the callbacks that the widget responds to.
//-------------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::MoveCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushAnnotationWidget *self =
          reinterpret_cast<vtkKWEPaintbrushAnnotationWidget*>(w);
  if (self->Movement++ > 20)
    {
    // Remove annotation. User is trying to shirk it off
    self->SetWidgetState(vtkKWEPaintbrushAnnotationWidget::EndTimer);
    }
}

//-------------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::HoverCallback(vtkAbstractWidget *w)
{
  vtkKWEPaintbrushAnnotationWidget *self =
    reinterpret_cast<vtkKWEPaintbrushAnnotationWidget*>(w);
  if (self->CallData)
    {
    int timerId = *(reinterpret_cast<int*>(self->CallData));

    // If this is the timer event we are waiting for...
    if ( timerId == self->TimerId
        && self->WidgetState == vtkKWEPaintbrushAnnotationWidget::Timing )
      {
      // Remove annotation. Its been there for too long (TimerDuration time).
      //self->SetWidgetState(vtkKWEPaintbrushAnnotationWidget::EndTimer);
      self->EventCallbackCommand->SetAbortFlag(1); //no one else gets this timer
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::SetRepresentation(
           vtkKWEPaintbrushAnnotationRepresentation * r )
{
  this->Superclass::SetWidgetRepresentation(r);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}
