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

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPLOT3DReader.h"
#include "vtkExtractGrid.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkArrowSource.h"
#include "vtkExecutive.h"
#include "vtkInformationVector.h"
#include "vtkTimerLog.h"

// If USE_FILTER is defined, glyph3D->PolyDataMapper is used instead of
// Glyph3DMapper.
//#define USE_FILTER

#ifdef USE_FILTER
# include "vtkGlyph3D.h"
#else
# include "vtkKWEGlyph3DMapper.h"
#endif

// from Graphics/Testing/Python/glyphComb.py

int TestGlyph3DMapperArrow(int argc, char *argv[])
{
  vtkPLOT3DReader *reader=vtkPLOT3DReader::New();
  char *fname=
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/combxyz.bin");
  reader->SetXYZFileName(fname);
  delete[] fname;
  fname=vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/combq.bin");
  reader->SetQFileName(fname);
  delete[] fname;
  reader->SetScalarFunctionNumber(100);
  reader->SetVectorFunctionNumber(202);
  reader->Update();

  vtkExtractGrid *eg=vtkExtractGrid::New();
  eg->SetInputConnection(reader->GetOutputPort());
  reader->Delete();
  eg->SetSampleRate(4,4,4);
  eg->Update();

  cout<< "eg pts="<<eg->GetOutput()->GetNumberOfPoints()<<endl;
  cout<< "eg cells="<<eg->GetOutput()->GetNumberOfCells()<<endl;

// create simple poly data so we can apply glyph
  vtkArrowSource *arrow=vtkArrowSource::New();
  arrow->Update();
  cout<< "pts="<<arrow->GetOutput()->GetNumberOfPoints()<<endl;
  cout<< "cells="<<arrow->GetOutput()->GetNumberOfCells()<<endl;

#ifdef USE_FILTER
  vtkGlyph3D *glypher=vtkGlyph3D::New();
#else
  vtkKWEGlyph3DMapper *glypher=vtkKWEGlyph3DMapper::New();
#endif
  glypher->SetInputConnection(eg->GetOutputPort());
  eg->Delete();
  glypher->SetSourceConnection(arrow->GetOutputPort());
  glypher->SetScaleFactor(2.0);
  arrow->Delete();

#ifdef USE_FILTER
  vtkPolyDataMapper *glyphMapper=vtkPolyDataMapper::New();
  glyphMapper->SetInputConnection(glypher->GetOutputPort());
#endif

  vtkActor *glyphActor=vtkActor::New();
#ifdef USE_FILTER
  glyphActor->SetMapper(glyphMapper);
  glyphMapper->Delete();
#else
  glyphActor->SetMapper(glypher);
#endif
  glypher->Delete();

  //Create the rendering stuff

  vtkRenderer *ren=vtkRenderer::New();
  vtkRenderWindow *win=vtkRenderWindow::New();
  win->SetMultiSamples(0); // make sure regression images are the same on all platforms
  win->AddRenderer(ren);
  ren->Delete();
  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(win);
  win->Delete();

  ren->AddActor(glyphActor);
  glyphActor->Delete();
  ren->SetBackground(0.5,0.5,0.5);
  win->SetSize(450,450);

  vtkCamera *cam=ren->GetActiveCamera();
  cam->SetClippingRange(3.95297,50);
  cam->SetFocalPoint(8.88908,0.595038,29.3342);
  cam->SetPosition(-12.3332,31.7479,41.2387);
  cam->SetViewUp(0.060772,-0.319905,0.945498);


  vtkTimerLog *timer=vtkTimerLog::New();
  timer->StartTimer();
  win->Render();
  timer->StopTimer();
  cout<<"first frame: "<< timer->GetElapsedTime()<<" seconds" <<endl;

  //  ren->GetActiveCamera()->Zoom(1.5);
  timer->StartTimer();
  win->Render();
  timer->StopTimer();
  cout<<"second frame: "<< timer->GetElapsedTime()<<" seconds" <<endl;
  timer->Delete();

  int retVal = vtkRegressionTestImage(win);
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }
  iren->Delete();
  
  return !retVal;
}
