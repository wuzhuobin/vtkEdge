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
#include "vtkKWEPaintbrushWidgetCallbackMapper.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkAbstractWidget.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkKWEPaintbrushWidget.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"

vtkStandardNewMacro(vtkKWEPaintbrushWidgetCallbackMapper);

//----------------------------------------------------------------------------
vtkKWEPaintbrushWidgetCallbackMapper::vtkKWEPaintbrushWidgetCallbackMapper()
{
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushWidgetCallbackMapper::~vtkKWEPaintbrushWidgetCallbackMapper()
{
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushWidgetCallbackMapper::Bindings()
{
  // These are the event callbacks supported by this widget
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::NoModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginDrawStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginDrawCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ControlModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginEraseStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginEraseCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginDrawStrokeForThisSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginDrawThisSketchCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ControlModifier | vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginEraseStrokeForThisSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginEraseThisSketchCallback);
  this->SetCallbackMethod(vtkCommand::MouseMoveEvent,
           vtkWidgetEvent::Move,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::MoveCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
           vtkKWEPaintbrushWidget::EndStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::EndStrokeCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 8, 1,"BackSpace",
           vtkKWEPaintbrushWidget::UndoStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::UndoCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+z
           vtkEvent::ControlModifier, 26, 0,"z",
           vtkKWEPaintbrushWidget::UndoStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::UndoCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 40, 1, "Right",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::RedoCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+y
           vtkEvent::ControlModifier, 25, 0,"y",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::RedoCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ControlModifier, 8, 1,"BackSpace",
           vtkKWEPaintbrushWidget::RedoStrokeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::RedoCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 47, 1, "Delete",
           vtkWidgetEvent::Delete,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::DeleteCallback);
  this->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
           vtkEvent::ShiftModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback);
  this->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
           vtkEvent::NoModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginIsotropicResizeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginIsotropicResizeShapeCallback);
  this->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
           vtkEvent::ControlModifier, 0, 0, NULL,
           vtkKWEPaintbrushWidget::BeginResizeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginResizeShapeCallback);
  this->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
           vtkKWEPaintbrushWidget::EndResizeEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::EndResizeShapeCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 27, 1,"Escape",
           vtkKWEPaintbrushWidget::ToggleSelectStateEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::ToggleSelectStateCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // Shift+
           vtkEvent::ShiftModifier, 43, 1,"plus",
           vtkKWEPaintbrushWidget::BeginNewSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::BeginNewSketchCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ShiftModifier, 40, 1, "Right",
           vtkKWEPaintbrushWidget::IncrementSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::IncrementSketchCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::ShiftModifier, 38, 1, "Left",
           vtkKWEPaintbrushWidget::DecrementSketchEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::DecrementSketchCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 109, 0, "m",
           vtkKWEPaintbrushWidget::MergeSelectionEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::MergeSelectionCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // Ctrl+a
           vtkEvent::ControlModifier, 1, 0,"a",
           vtkKWEPaintbrushWidget::ToggleSelectAllSketchesEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::ToggleSelectAllSketchesCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // "<"
           vtkEvent::ShiftModifier, 60, 0,"less",
           vtkKWEPaintbrushWidget::DecreaseOpacityEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::DecreaseOpacityCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent, // ">"
           vtkEvent::ShiftModifier, 62, 0,"greater",
           vtkKWEPaintbrushWidget::IncreaseOpacityEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::IncreaseOpacityCallback);
  this->SetCallbackMethod(vtkCommand::EnterEvent,
           vtkKWEPaintbrushWidget::EnterEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::EnterWidgetCallback);
  this->SetCallbackMethod(vtkCommand::LeaveEvent,
           vtkKWEPaintbrushWidget::LeaveEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::LeaveWidgetCallback);
  this->SetCallbackMethod(vtkCommand::KeyPressEvent,
           vtkEvent::NoModifier, 108, 0, "l",
           vtkKWEPaintbrushWidget::ToggleSketchMutabilityEvent,
           this->PaintbrushWidget, vtkKWEPaintbrushWidget::ToggleSketchMutabilityCallback);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushWidgetCallbackMapper::SetPaintbrushWidget( vtkKWEAbstractPaintbrushWidget * w )
{
  this->PaintbrushWidget = w;
  this->SetEventTranslator(w->GetEventTranslator());
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushWidgetCallbackMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

