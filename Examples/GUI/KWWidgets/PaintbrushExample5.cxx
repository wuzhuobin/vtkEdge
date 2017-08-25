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

