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
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkGenericDataObjectReader.h"
#include "vtkKWEIlluminatedLinesPainter.h"
#include "vtkKWERenderingFactory.h"
#include "vtkPainterPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTestUtilities.h"

int TestIlluminatedLines(int argc, char *argv[])
{
  // Register factory.
  vtkKWERenderingFactory::RegisterFactory();

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::New();
  interactor->SetRenderWindow(renWin);

  renWin->SetSize(300,300);
  renderer->SetBackground(0.3, 0.3, 0.3);
  renWin->Render();
  if (!vtkKWEIlluminatedLinesPainter::IsSupported(renWin))
    {
    cout << "WARNING: The rendering context does not support required "
      "extensions." << endl;
    return 0;
    }


  char* filename =  
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/disk_out_ref_streamlines.vtk");
 
  vtkGenericDataObjectReader *pl3d = vtkGenericDataObjectReader::New();
  pl3d->SetFileName(filename);
  delete[] filename;
  pl3d->Update();

  vtkPainterPolyDataMapper * mapper = vtkPainterPolyDataMapper::New();
  mapper->SetInputConnection(pl3d->GetOutputPort());
  pl3d->Delete();

  mapper->SelectColorArray("Pres");
  mapper->SetScalarModeToUsePointFieldData();

  mapper->ScalarVisibilityOff();
  mapper->CreateDefaultLookupTable();
  mapper->InterpolateScalarsBeforeMappingOn();
  mapper->ImmediateModeRenderingOff();
  mapper->SetScalarRange(0.006, 0.022);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);
  mapper->Delete();

  actor->GetProperty()->SetSpecularPower(50.0);
//actor->GetProperty()->SetColor(1,1,1);
  actor->GetProperty()->SetColor(0.8,0.4,0.1);
//  actor->GetProperty()->SetSpecularColor(0.5,0.5,0.1);
  actor->GetProperty()->SetDiffuse(0.8);
  actor->GetProperty()->SetSpecular(0.5);
//  actor->GetProperty()->SetSpecularColor(0.9, 0.7, 0.7);
//  actor->GetProperty()->SetDiffuseColor(0.5, 0.5, 0.5);

  renderer->AddActor(actor);
  actor->Delete();
  renderer->GetActiveCamera()->Azimuth(280);
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.5);

  renWin->Render();
  int retVal = vtkTesting::Test(argc, argv, renWin, 75);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    interactor->Start();
    }
  interactor->Delete();

  renderer->RemoveAllViewProps();
  renderer->Delete();
  renWin->Delete();

  if ((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR))
    {
    return 0;
    }

  // failed.
  return 1;
}
