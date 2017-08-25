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
// Unlike PaintbrushExample1, this example shows how the user can use a
// vtkKWEWidgetGroup to tie multiple paintbrushes to paint on several
// render windows. Here a paintbrush can be used on a 3 pane view.

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEWidgetGroup.h"

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

void MySelectEditCallbackMethod( vtkKWEPaintbrushWidget *w, int editMode )
{
  w->SetPaintbrushMode( editMode ? vtkKWEPaintbrushWidget::Edit :
                                   vtkKWEPaintbrushWidget::Select );
}

int my_example( int , char *[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( example->GetNthRenderWidget(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentation2D * rep =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());

    rep->SetImageActor( example->GetNthImageActor(i) );
    rep->SetImageData(imageData);
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
        imageData->GetSpacing() );
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
        imageData->GetOrigin() );

    set->AddWidget(w);

    // This will simply cause the method
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Edit ) or
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Select )
    // to be invoked based on which radio button is depressed.
    example->SetSelectEditCallbackMethod( MySelectEditCallbackMethod );
    example->AddSelectEditCallback(w);

    w->Delete();
    }

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());
  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  set->SetEnabled(1);

  int res = example->Run();

  set->Delete();
  return res;
}

