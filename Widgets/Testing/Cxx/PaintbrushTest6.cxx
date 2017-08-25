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
//
// The example differs from the previous ones in that it renders the brush in
// as contours. The brush data is grayscale in nature and is represented with
// vtkKWEPaintbrushGrayscaleData. The paintbrush representation is also a grayscale
// representation (vtkKWEPaintbrushRepresentationGrayscale2D).

#include "vtkKWEPaintbrushTesting.h"
#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentationGrayscale2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkMultiThreader.h"
#include "vtkImageViewer2.h"
#include <sstream>
#include <string>
#include <map>

#include "vtkInteractorEventRecorder.h"
#include "vtkRegressionTestImage.h"

int PaintbrushTest6( int argc, char *argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage error: \t " << argv[0] << "\t"
              << "Input_Image_Data  InitialLabelMap EventsLogFile Mode(0:replay 1:record)"
              << "[OutputLabelMap]" << std::endl;
    return EXIT_FAILURE;
    }

  vtkKWEPaintbrushTesting *paintbrushTesting = vtkKWEPaintbrushTesting::New();
  paintbrushTesting->SetFilename( argv[1] );
  paintbrushTesting->Initialize();
  vtkImageData * imageData  = paintbrushTesting->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( paintbrushTesting->GetNthImageViewer(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentationGrayscale2D * rep =
                vtkKWEPaintbrushRepresentationGrayscale2D::New();
    w->SetRepresentation(rep);
    if (rep)
      {
      vtkImageActor * imageActor = paintbrushTesting->GetNthImageActor(i);
      rep->SetImageActor(imageActor);
      rep->SetImageData(imageData);
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
          imageData->GetSpacing() );
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
          imageData->GetOrigin() );
      }

    set->AddWidget(w);
    rep->Delete();
    w->Delete();
    }

  vtkKWEPaintbrushRepresentation2D * repx =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());
  vtkKWEPaintbrushDrawing * drawingx = repx->GetPaintbrushDrawing();
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawingx );
    }

  // Now read in an initial segmentation.

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());

  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  drawing->InitializeData();
  vtkKWEPaintbrushGrayscaleData *data = vtkKWEPaintbrushGrayscaleData::New();

  // Read the file (initial segmentation)
  vtkMetaImageReader * reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[2] );
  reader->Update();
  data->SetImageData(reader->GetOutput());
  reader->Delete();

  // Set the first stroke as the initial segmentation.
  drawing->AddNewStroke(0, vtkKWEPaintbrushEnums::Draw, data);

  // Instead of the above line, you can set this directly as the new stencil,
  // saves some memory, as the overhead data of an extra stroke need not be
  // managed.
  // drawing->SetPaintbrushData(data);

  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation * repr =
      vtkKWEPaintbrushRepresentation::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  set->SetEnabled(1);


  // record/replay events
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();

  //Interaction only in the first Image Viewer
  recorder->SetInteractor( paintbrushTesting->GetNthImageViewer(0)->
                      GetRenderWindow()->GetInteractor());

  //Set event log recording file
  std::string eventLogFileName = argv[3];
  recorder->SetFileName(eventLogFileName.c_str());
  recorder->On();

  //Set recorder mode depending on the user's choice
  unsigned int mode = atoi( argv[4] );
  int retVal = 1;
  if( mode )
    {
    std::cout << "Interaction recording....." << std::endl;
    recorder->Record();
    paintbrushTesting->Run();
    }
  else
    {
    std::cout << "Interaction replaying....." << std::endl;
    recorder->Play();
    //recorder must be turned off before regression testing
    recorder->Off();
    retVal = vtkRegressionTestImage(
            paintbrushTesting->GetNthImageViewer(0)->GetRenderWindow() );
    if ( retVal == vtkRegressionTester::DO_INTERACTOR)
      {
      recorder->GetInteractor()->Start();
      }
    }



  // Save out the resulting segmentation.
  //
  vtkImageData * output = vtkImageData::New();
  drawing->GetPaintbrushData()->
                    GetPaintbrushDataAsImageData( output );
  vtkMetaImageWriter * outputWriter = vtkMetaImageWriter::New();
  outputWriter->SetFileName("Drawing.mha");
  outputWriter->SetInput( output );
  outputWriter->Write();

  // Save out each of the resulting segmentations.
  //
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

  recorder->Delete();
  data->Delete();
  outputWriter->Delete();
  output->Delete();
  set->Delete();
  paintbrushTesting->Delete();

  return !retVal;

}

