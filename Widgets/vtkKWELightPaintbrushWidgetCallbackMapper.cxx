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
#include "vtkKWELightPaintbrushWidgetCallbackMapper.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkAbstractWidget.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkKWEPaintbrushWidget.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"

vtkStandardNewMacro(vtkKWELightPaintbrushWidgetCallbackMapper);

//----------------------------------------------------------------------------
void vtkKWELightPaintbrushWidgetCallbackMapper::Bindings()
{
  // Remove any events from the existing callback mapper.

  this->EventTranslator->ClearEvents();

  // These are the event callbacks supported by this widget

  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::NoModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginDrawStrokeForThisSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginDrawThisSketchCallback);  
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ControlModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginEraseStrokeForThisSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginEraseThisSketchCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginDrawStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginDrawCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ControlModifier | vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginEraseStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginEraseCallback);
  this->SetCallbackMethod(vtkCommand::MouseMoveEvent,
           vtkWidgetEvent::Move,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::MoveCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
           vtkKWEPaintbrushWidget::EndStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::EndStrokeCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback);
  this->SetCallbackMethod(vtkCommand::EnterEvent,
           vtkKWEPaintbrushWidget::EnterEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::EnterWidgetCallback);
  this->SetCallbackMethod(vtkCommand::LeaveEvent,
           vtkKWEPaintbrushWidget::LeaveEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::LeaveWidgetCallback);
}

//----------------------------------------------------------------------------
void vtkKWELightPaintbrushWidgetCallbackMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}


