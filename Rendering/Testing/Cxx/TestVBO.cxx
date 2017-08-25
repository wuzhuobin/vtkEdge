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
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkGenericDataObjectReader.h"
#include "vtkKWERenderingFactory.h"
#include "vtkPainterPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"
#include "vtkSphereSource.h"
#include "vtkStripper.h"
#include "vtkTimerLog.h"

int TestVBO(int argc, char* argv[])
{
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  vtkRenderer *renderer = vtkRenderer::New();
  renWin->AddRenderer(renderer);
  renderer->SetBackground(1, 1, 0);
  renderer->Delete();

  vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::New();
  interactor->SetRenderWindow(renWin);

  vtkKWERenderingFactory::RegisterFactory();

  vtkSphereSource* sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(1024);
  sphere->SetPhiResolution(1024);

  vtkStripper* stipper = vtkStripper::New();
  stipper->SetInputConnection(sphere->GetOutputPort());

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInputConnection(stipper->GetOutputPort());
  mapper->ImmediateModeRenderingOn();

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renWin->Render();

  vtkTimerLog* log = vtkTimerLog::New();
  double sum = 0.0;
  for (int cc=0; cc < 50; cc++)
    {
    log->StartTimer();
    renWin->Render();
    log->StopTimer();
    sum += log->GetElapsedTime();
    }
  log->Delete();
  cout << "Avg Time: " << sum/50.0 << endl;


  //interactor->Start();

  renWin->Delete();
  interactor->Delete();
  sphere->Delete();
  stipper->Delete();
  mapper->Delete();
  actor->Delete();
  return 0;

}
