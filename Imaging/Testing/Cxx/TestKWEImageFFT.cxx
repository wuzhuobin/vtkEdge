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

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageIdealHighPass.h"
#include "vtkImageReader2.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageViewer2.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTimerLog.h"


#include "vtkKWEImageFFT.h"
#include "vtkKWEImageRFFT.h"

int TestKWEImageFFT(int argc, char *argv[])
{
  vtkImageReader2Factory * createReader = vtkImageReader2Factory::New();

  char *fname=
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/fullhead15.png");

  vtkImageReader2 * reader = createReader->CreateImageReader2(fname);
  reader->SetFileName(fname);

  delete[] fname;

  vtkKWEImageFFT * fft = vtkKWEImageFFT::New();
  fft->SetInputConnection(reader->GetOutputPort());

  vtkImageIdealHighPass * highPass = vtkImageIdealHighPass::New();
  highPass->SetInputConnection(fft->GetOutputPort());
  highPass->SetXCutOff(0.1);
  highPass->SetYCutOff(0.1);
  highPass->ReleaseDataFlagOff();

  vtkKWEImageRFFT * rfft = vtkKWEImageRFFT::New();
  rfft->SetInputConnection(highPass->GetOutputPort());

  vtkImageExtractComponents * real = vtkImageExtractComponents::New();
  real->SetInputConnection(rfft->GetOutputPort());
  real->SetComponents(0);

  vtkImageViewer2 * viewer = vtkImageViewer2::New();
  viewer->SetInputConnection(real->GetOutputPort());
  viewer->SetColorWindow(500);
  viewer->SetColorLevel(0);

  vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::New();
  viewer->SetupInteractor(iren);

  viewer->Render();

  int retVal = vtkRegressionTestImage( viewer->GetRenderWindow() );
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    viewer->Render();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
    }

  createReader->Delete();
  reader->Delete();
  fft->Delete();
  highPass->Delete();
  rfft->Delete();
  real->Delete();
  iren->Delete();
  viewer->Delete();

  return !retVal;
}
