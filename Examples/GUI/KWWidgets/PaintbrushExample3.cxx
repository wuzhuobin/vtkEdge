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
// This example shows how you can initialize the paintbrush with several initial
// segmentation. It also demonstrates how you can save the final segmentation.
// A user-defined segmentation can be added to the existing segmentation, at
// any time, (not just to initialize the paintbrush data).
// 
// The initial segmentation file used here is "Ventricles.mhd" should, of course 
// have the same metadata as the input image. In this example, the segmentation is 
// added as a new stroke, so it is possible to undo-redo it. Note that this allows
// you to actually compose multiple segmentations as :
//
//    drawing->AddItem( sketch1 );
//    sketch1->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData1 );
//    drawing->AddItem( sketch2 );
//    sketch2->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData2 );
// 
// It also allows you to add to an existing segmentation as :
//    sketch2->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData3 );
//
// The example demonstrates writing out the final segmentations as 
// DrawingN.mha

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include <sstream>

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
  vtkMetaImageReader * reader;
  
  example->SetFilename( vtkKWMyWindow::ExpandFilename("brainweb1e1a10f20.mha").c_str());
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
    if (rep)
      {
      vtkImageActor * imageActor = example->GetNthImageActor(i);
      rep->SetImageActor(imageActor);
      rep->SetImageData(imageData);
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing( 
          imageData->GetSpacing() );
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin( 
          imageData->GetOrigin() );
      }

    // This will simply cause the method 
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Edit ) or 
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Select ) 
    // to be invoked based on which radio button is depressed.  
    example->SetSelectEditCallbackMethod( MySelectEditCallbackMethod );
    example->AddSelectEditCallback(w);

    set->AddWidget(w);
    w->Delete();
    }

  // Now read in an initial segmentation.

  vtkKWEPaintbrushRepresentation2D * rep = 
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());
  
  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  drawing->InitializeData();
  
  // Clear the drawing and start on a clean slate. The drawing would have
  // automatically created an empty sketch for us, so we can start drawing
  // right away. We will wipe it all off. 
  drawing->RemoveAllItems(); 
  
  // ----- Segmentation 1 -------
  // Read segmentation and create binary data out of it.
  
  vtkKWEPaintbrushStencilData *stencilData1 = vtkKWEPaintbrushStencilData::New();
  reader = vtkMetaImageReader::New();
  reader->SetFileName( vtkKWMyWindow::ExpandFilename("LRVentricles.mha").c_str() );
  reader->Update();
  vtkKWEPaintbrushUtilities::GetStencilFromImage< 
    vtkKWEPaintbrushUtilities::vtkFunctorGreaterThanEqualTo >(
      reader->GetOutput(), stencilData1->GetImageStencilData(), 128.0);
  reader->Delete();
  
  // Now create a sketch.
  // Add the segmentation to the sketch, as a stroke.
  // Add the sketch to drawing.
  vtkKWEPaintbrushSketch * sketch1 = vtkKWEPaintbrushSketch::New();
  drawing->AddItem( sketch1 );
  sketch1->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData1 );
  sketch1->Delete();

  
  // ----- Segmentation 2 -------
  // Read segmentation and create binary data out of it.
  
  vtkKWEPaintbrushStencilData *stencilData2 = vtkKWEPaintbrushStencilData::New();
  reader = vtkMetaImageReader::New();
  reader->SetFileName( vtkKWMyWindow::ExpandFilename("WhiteMatter.mha").c_str() );
  reader->Update();
  vtkKWEPaintbrushUtilities::GetStencilFromImage< 
    vtkKWEPaintbrushUtilities::vtkFunctorGreaterThanEqualTo >(
      reader->GetOutput(), stencilData2->GetImageStencilData(), 128.0);
  reader->Delete();
  
  // Now create a sketch.
  // Add the segmentation to the sketch, as a stroke.
  // Add the sketch to drawing.
  vtkKWEPaintbrushSketch * sketch2 = vtkKWEPaintbrushSketch::New();
  drawing->AddItem( sketch2 );
  sketch2->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData2 );
  sketch2->Delete();
  

  // Now set the drawing on all representations.
  
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr = 
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  // Enable the widget and we are done.
  set->SetEnabled(1);
  int res = example->Run();


  // Save out each of the resulting segmentations. We will write out each 
  // sketch in the drawing as a seperate file.

  for (int i = 0; i < drawing->GetNumberOfItems(); i++)
    {
    vtkImageData * outputN = vtkImageData::New();
    drawing->GetItem(i)->GetPaintbrushData()->
                    GetPaintbrushDataAsImageData( outputN );
    vtkMetaImageWriter * outputWriterN = vtkMetaImageWriter::New();
    std::ostringstream filenameN;
    filenameN << "Drawing" << i << ".mha" << std::ends;
    outputWriterN->SetFileName(filenameN.str().c_str());
    outputWriterN->SetInput( outputN );
    outputWriterN->Write();
    outputWriterN->Delete();
    outputN->Delete();
    }
  
  stencilData1->Delete();
  stencilData2->Delete();
  set->Delete();

  return res;
}
