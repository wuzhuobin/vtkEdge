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
// This example demonstrates how you can use a vtkKWEPaintbrushWidget to create
// and edit segmentations on a render window. Basic editing capabilities,
// drawing, erasing, resizing the brush, undo-redo etc are demonstrated in
// this example.
//
// User interactions:
//   Left click and drag            - Draws a stroke
//   Ctrl-Left click and            - Erases the stroke
//   Backspace or Left keypress     - Undo the sketch (deletes the previously
//                                    drawn stroke in a sketch)
//   Right keypress                 - Redo on the given sketch
//   Shift "+"                      - Start a new sketch.
//   Shift Right                    - Traverse to the next sketch.
//   Shift Left                     - Traverse to the previous sketch.
//   Shift Backspace                - Delete a sketch.
//   Escape                         - Toggle Enable/Disable a brush.

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

void MySelectEditCallbackMethod( vtkKWEPaintbrushWidget *w, int editMode )
{
  w->SetPaintbrushMode( editMode ? vtkKWEPaintbrushWidget::Edit :
                                   vtkKWEPaintbrushWidget::Select );
}

int my_example( int, char *[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  example->FourPaneViewOff();
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEPaintbrushWidget *widget = vtkKWEPaintbrushWidget::New();
  widget->SetInteractor( example->GetAxialRenderWidget()->
                    GetRenderWindow()->GetInteractor());

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(widget->GetRepresentation());
  rep->SetImageActor( example->GetAxialImageActor() );
  rep->SetImageData(imageData);
  rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
      imageData->GetSpacing() );
  rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
      imageData->GetOrigin() );

  widget->SetEnabled(1);

  // This will simply cause the method
  //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Edit ) or
  //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Select )
  // to be invoked based on which radio button is depressed.
  example->SetSelectEditCallbackMethod( MySelectEditCallbackMethod );
  example->AddSelectEditCallback(widget);

  int res = example->Run();

  widget->Delete();
  return res;
}

