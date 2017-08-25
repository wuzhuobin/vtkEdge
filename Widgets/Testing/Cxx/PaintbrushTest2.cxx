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
#include "vtkCommand.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageViewer2.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"

#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"

#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushProperty.h"

#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"

int PaintbrushTest2( int argc, char *argv[] )
{
  if( argc < 4 )
    {
    std::cerr << "Usage error: \t " << argv[0] << "\t"
              << "Input_Image_Data \tEventsLogFile\t Mode(0:replay 1:record)" << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageReader2 * reader = vtkMetaImageReader::New();

  reader->SetFileName( argv[1] );
  if( reader->CanReadFile( argv[1] ) != 3 )
    {
    std::cerr << "Meta image reader couldn't read the input data\t" << std::endl;
    return EXIT_FAILURE;
    }

  reader->Update();

  vtkImageData * imageData  = reader->GetOutput();

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

  vtkImageViewer2 *viewer = vtkImageViewer2::New();
  viewer->SetRenderWindow(renWin);
  viewer->SetRenderer(renderer);
  viewer->SetInput(imageData);
  viewer->SetupInteractor(iren);
  double *range = imageData->GetScalarRange();
  viewer->SetColorWindow(range[1] - range[0]);
  viewer->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);
  viewer->SetSlice( 15 );

  vtkKWEPaintbrushWidget *widget = vtkKWEPaintbrushWidget::New();
  widget->SetInteractor( iren );

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(widget->GetRepresentation());
  rep->SetImageActor( viewer->GetImageActor() );
  rep->SetImageData(imageData);
  rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
      imageData->GetSpacing() );
  rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
      imageData->GetOrigin() );

  iren->Initialize();
  renWin->SetSize( 500, 500 );
  renWin->Render();

  widget->SetEnabled(1);

  // record/replay events
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();
  recorder->SetInteractor(iren);

  //Set event log recording file
  std::string eventLogFileName = argv[2];
  recorder->SetFileName(eventLogFileName.c_str());
  recorder->On();

  //Set recorder mode depending on the user's choice
  int retVal = 1;
  unsigned int mode= atoi( argv[3] );
  if( mode )
    {
    std::cout << "Interaction recording....." << std::endl;
    recorder->Record();
    iren->Start();
    }
  else
    {
    std::cout << "Interaction replaying....." << std::endl;
    recorder->Play();
    //recorder must be turned off before regression testing
    recorder->Off();
    retVal = vtkRegressionTestImage( renWin );
    if ( retVal == vtkRegressionTester::DO_INTERACTOR)
      {
      recorder->GetInteractor()->Start();
      }
    }

  // Clean up
  recorder->Off();
  recorder->Delete();
  viewer->Delete();
  widget->Delete();
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  reader->Delete();

  return !retVal;
}
