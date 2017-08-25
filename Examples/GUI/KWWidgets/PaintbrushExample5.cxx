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
// PaintbrushData drawn can be filtered through "operations". These are
// represented by abstract base classes: vtkKWEPaintbrushOperation. The base
// class acts as a pass through filter. Concrete subclasses can do fancy
// filtering operations such as flood-fill etc. These operations permit an
// inplace filtering of a paintbrush data during a sketch.
//
// As an example, a flood-fill operation is implemented in
// vtkKWEITKConfidenceConnectedPaintbrushOperation. This uses a combination of ITK
// region growing and hole-filling filters. This example illustrates how to
// plug an operation into the paintbrush widget.

#include "vtkKWMyWindow.h"

#include "VTKEdgeConfigure.h" // export

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEITKConfidenceConnectedPaintbrushOperation.h"
#include "vtkKWEWidgetGroup.h"

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

int my_example( int , char *[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  vtkKWEITKConfidenceConnectedPaintbrushOperation * operation
    = vtkKWEITKConfidenceConnectedPaintbrushOperation::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( example->GetNthRenderWidget(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentation2D * rep =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());

    rep->SetImageActor( example->GetNthImageActor(i) );

    // Note that an operation if set, must be set before the image data.
    rep->SetPaintbrushOperation(operation);
    rep->SetImageData(imageData);

    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
        imageData->GetSpacing() );
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
        imageData->GetOrigin() );

    set->AddWidget(w);
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

  operation->Delete();
  set->Delete();
  return res;
}

