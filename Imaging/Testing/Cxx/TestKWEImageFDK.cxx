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
#include "vtkColorTransferFunction.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkImageShiftScale.h"
#include "vtkImageViewer2.h"
#include "vtkKWEImageFDKFilter.h"
#include "vtkMath.h"
#include "vtkMetaImageReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"


#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

#include <math.h>

int TestKWEImageFDK(int argc, char *argv[])
{
  int numImages = 360;

  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  iren->SetRenderWindow(renWin);
  renWin->Delete();
  
  vtkRenderer *renderer = vtkRenderer::New();
  renWin->AddRenderer(renderer);

  char *fname=
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/SheppLoganProjections.mhd");
  
  vtkMetaImageReader *reader = vtkMetaImageReader::New();
  reader->SetFileName(fname);
  reader->Update();

  delete[] fname;

  vtkKWEImageFDKFilter * FDKFilter =  vtkKWEImageFDKFilter::New();
  FDKFilter->SetInput(reader->GetOutput());
  FDKFilter->SetRadius(150);

  vtkDoubleArray * angles = FDKFilter->GetAngles();
  angles->SetNumberOfTuples(numImages);

  double PI = vtkMath::DoublePi();

  angles->SetTuple1(0, 0.0);

  for(int i = 1; i < numImages; i++)
    {
    angles->SetTuple1(i, i*(2.0*PI)/numImages);
    }

  //FDKFilter->Update();

  vtkImageShiftScale * scale = vtkImageShiftScale::New();
  scale->SetInputConnection(reader->GetOutputPort());
  scale->SetOutputScalarTypeToUnsignedChar();

  vtkPiecewiseFunction * opacityTransferFunction = 
    vtkPiecewiseFunction::New();
  opacityTransferFunction->AddPoint(20, 0.0);
  opacityTransferFunction->AddPoint(255, 0.2);

  vtkColorTransferFunction * colorTransferFunction =
    vtkColorTransferFunction::New();
  colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(10.0, 1.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(20.0, 0.0, 0.0, 1.0);
  colorTransferFunction->AddRGBPoint(30.0, 0.0, 1.0, 0.0);
  colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);


  vtkVolumeRayCastCompositeFunction * compositeFunction1 = 
    vtkVolumeRayCastCompositeFunction::New();

  vtkVolumeRayCastMapper * volumeMapper = 
    vtkVolumeRayCastMapper::New();

  vtkVolumeProperty * volumeProperty = vtkVolumeProperty::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);
  volumeProperty->SetInterpolationTypeToLinear();
  volumeProperty->ShadeOff();

  volumeMapper->SetVolumeRayCastFunction(compositeFunction1);
  volumeMapper->SetInputConnection(scale->GetOutputPort());

  vtkVolume * volume = vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);

  renderer->AddVolume(volume);

  renderer->SetBackground(0.0,0.0,0.0);

  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    renWin->Render();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
    }

  volumeProperty->Delete();
  FDKFilter->Delete();
  reader->Delete();
  scale->Delete();
  iren->Delete();
  renderer->Delete();

  volume->Delete();
  compositeFunction1->Delete();
  colorTransferFunction->Delete();
  opacityTransferFunction->Delete();
  volumeMapper->Delete();

  return !retVal;
}

