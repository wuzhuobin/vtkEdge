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

// This test volume renders the vase dataset with 4 dependent components the
// minimum intensity projection method.

#include "vtkKWEGPUVolumeRayCastMapper.h"
#include "vtkTestUtilities.h"
#include "vtkXMLImageDataReader.h"
#include "vtkImageShiftScale.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolumeProperty.h"
#include "vtkCamera.h"
#include "vtkRegressionTestImage.h"

int TestGPURayCastFourComponentsMinIP(int argc,
                                      char *argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;
  char *cfname=
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/vase_4comp.vti");
  
  vtkXMLImageDataReader *reader=vtkXMLImageDataReader::New();
  reader->SetFileName(cfname);
  delete [] cfname;

  vtkImageShiftScale *shiftScale=vtkImageShiftScale::New();
  shiftScale->SetShift(-255);
  shiftScale->SetScale(-1);
  shiftScale->SetInputConnection(reader->GetOutputPort());
  
  
  vtkRenderer *ren1=vtkRenderer::New();
  vtkRenderWindow *renWin=vtkRenderWindow::New();
  renWin->AddRenderer(ren1);
  renWin->SetSize(301,300);
  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  
  renWin->Render();
  
  vtkKWEGPUVolumeRayCastMapper *volumeMapper;
  vtkVolumeProperty *volumeProperty;
  vtkVolume *volume;
  
  volumeMapper=vtkKWEGPUVolumeRayCastMapper::New();
  volumeMapper->SetBlendModeToMinimumIntensity();
  volumeMapper->SetInputConnection(
    shiftScale->GetOutputPort());
  
  volumeProperty=vtkVolumeProperty::New();
  volumeProperty->IndependentComponentsOff();
  
  vtkPiecewiseFunction *f = vtkPiecewiseFunction::New();
  f->AddPoint(0,1.0);
  f->AddPoint(255,0.0);
  volumeProperty->SetScalarOpacity(f);
  f->Delete();
  
  volume=vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);
  
  int valid=volumeMapper->IsRenderSupported(renWin,volumeProperty);
  
  int retVal;
  if(valid)
    {
    iren->Initialize();
    ren1->SetBackground(0.1,0.4,0.2);
    ren1->ResetCamera();
    renWin->Render();
    
    retVal = vtkTesting::Test(argc, argv, renWin, 75);
    if (retVal == vtkRegressionTester::DO_INTERACTOR)
      {
      iren->Start();
      }
    }
  else
    {
    retVal=vtkTesting::PASSED;
    cout << "Required extensions not supported." << endl;
    }
  
  iren->Delete();
  renWin->Delete();
  ren1->Delete();
  volumeMapper->Delete();
  volumeProperty->Delete();
  volume->Delete();
     
  reader->Delete();
  shiftScale->Delete();
  
  if ((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR))
    {
    return 0;
    }
  else
    {
    return 1;
    }
}
