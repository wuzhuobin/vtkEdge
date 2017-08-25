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
