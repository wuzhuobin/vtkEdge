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
#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkPainterPolyDataMapper.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkKWERenderingFactory.h"
#include "vtkProperty.h"
#define VTK_CREATE_NEW(var, class) vtkSmartPointer<class> var = vtkSmartPointer<class>::New();

int TestHiddenLinesRemoval(int argc, char* argv[])
{
  vtkKWERenderingFactory::RegisterFactory();
  char* filename =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/disk_out_ref_surface.vtp");

  VTK_CREATE_NEW(reader, vtkXMLPolyDataReader);
  if (!reader->CanReadFile(filename))
    {
    cerr << "Cannot read file: " << filename << endl;
    return 1;
    }

  reader->SetFileName(filename);
  delete [] filename;

  reader->Update();

  VTK_CREATE_NEW(renWin, vtkRenderWindow);
  VTK_CREATE_NEW(renderer, vtkRenderer);
  VTK_CREATE_NEW(iren, vtkRenderWindowInteractor);
  renWin->SetReportGraphicErrors(1);
  renWin->AddRenderer(renderer);
  renWin->SetSize(300,300);
  iren->SetRenderWindow(renWin);

  VTK_CREATE_NEW(clone, vtkPolyData);
  clone->ShallowCopy(reader->GetOutput());
  clone->GetPointData()->SetActiveVectors("V");

  VTK_CREATE_NEW(mapper, vtkPainterPolyDataMapper);
  mapper->SetInput(clone);
  mapper->ImmediateModeRenderingOn();

  VTK_CREATE_NEW(actor, vtkActor);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetRepresentation(VTK_WIREFRAME);

  renderer->AddActor(actor);

  renderer->GetActiveCamera()->SetFocalPoint(-1.88, -0.98, -1.04);
  renderer->GetActiveCamera()->SetPosition(13.64, 4.27, -31.59);
  renderer->GetActiveCamera()->SetViewAngle(30);
  renderer->GetActiveCamera()->SetViewUp(0.41, 0.83, 0.35);
  renderer->SetBackground(0.3, 0.3, 0.3);
  //renderer->SetViewport(.2, 0.2, 1, 1);
  renderer->ResetCamera();
  //actor->SetPosition(10, 0, 0);
  renWin->Render();
  int retVal = vtkTesting::Test(argc, argv, renWin, 75);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }

  if ((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR))
    {
    return 0;
    }

  // failed.
  return 1;
}
