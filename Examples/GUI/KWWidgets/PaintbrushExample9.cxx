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

