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
// composite method with no shading.

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
#include "vtkImageMagnify.h"
#include "vtkImageData.h"

int TestGPURayCastFourComponentsCompositeStreaming(int argc,
                                                   char *argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;
  char *cfname=
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/vase_4comp.vti");
  
  vtkXMLImageDataReader *reader=vtkXMLImageDataReader::New();
  reader->SetFileName(cfname);
  delete [] cfname;

  
  vtkImageMagnify *mag=vtkImageMagnify::New();
  mag->SetInputConnection(reader->GetOutputPort());
  mag->SetMagnificationFactors(5,1,1);
  mag->SetInterpolate(1);
  
  int dims[3];
  mag->Update();
  mag->GetOutput()->GetDimensions(dims);
  unsigned long sizekb=mag->GetOutput()->GetActualMemorySize();
  cout<<"Memory usage for the ImageData="<<(sizekb/1024)<<"Mb"<<endl;
  cout<<"Dims of the ImageData="<<dims[0]<<"x"<<dims[1]<<"x"<<dims[2]<<"="<<(dims[0]*dims[1]*dims[2])/1024/1024<<"Mb"<<endl;
  
  
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
  volumeMapper->SetBlendModeToComposite();
  volumeMapper->SetInputConnection(
    mag->GetOutputPort());
  
  vtkPiecewiseFunction *opacity=vtkPiecewiseFunction::New();
  opacity->AddPoint(0,0);
  opacity->AddPoint(255,1);
  
  volumeProperty=vtkVolumeProperty::New();
  volumeProperty->IndependentComponentsOff();
  volumeProperty->ShadeOff();
  volumeProperty->SetScalarOpacity(opacity);
  
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
     
  opacity->Delete();
  mag->Delete();
  
  reader->Delete();
  
  if ((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR))
    {
    return 0;
    }
  else
    {
    return 1;
    }
}
