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

// Example demonstrating use of vtkKWEGPUArrayCalculator filter.

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkKWEGPUArrayCalculator.h"

#include "vtkRenderWindow.h"
#include "vtkImageImport.h"
#include <assert.h>

// On a nVidia Quadro FX 3600M (~GeForce8),
// GL_MAX_TEXTURE_SIZE = 8192, GL_MAX_3D_TEXTURE_SIZE = 2048
// GL_MAX_VIEWPORT_DIMS = 8192, 8192
// The following size matches two complete full 2D textures, one 2D texture
// with one complete row and one 2D texture with just one pixel.
//const vtkIdType TestNumberOfPoints=2*(8192*8192)+8192+1;

const vtkIdType TestNumberOfPoints=(2*(8192*8192)+8192+1);

vtkImageImport *CreateSource(vtkIdType numberOfPoints)
{
  assert("pre: valid_number_of_points" && numberOfPoints>0);
  vtkImageImport *im=vtkImageImport::New();
  float *ptr=new float[numberOfPoints];
  vtkIdType i=0;
  while(i<numberOfPoints)
    {
    ptr[i]=static_cast<float>(i);
    ++i;
    }
  im->SetDataScalarTypeToFloat();
  im->SetImportVoidPointer(ptr,0); // let the importer delete it.
  im->SetNumberOfScalarComponents(1);
  im->SetDataExtent(0,static_cast<int>(numberOfPoints-1),0,0,0,0);
  im->SetWholeExtent(0,static_cast<int>(numberOfPoints-1),0,0,0,0);
  im->SetScalarArrayName("values");
  return im;
}

// Example demonstrating use of vtkKWEGPUArrayCalculator filter.
int GPUArrayCalculatorDemo(int vtkNotUsed(argc),
                           char *vtkNotUsed(argv)[])
{
  vtkRenderWindow *win=vtkRenderWindow::New();

  // Create an dataset with an array of values to work with.
  vtkImageImport *im=CreateSource(TestNumberOfPoints);

  // Create the calculator
  vtkKWEGPUArrayCalculator *calc=vtkKWEGPUArrayCalculator::New();

  // Need an OpenGL context
  calc->SetContext(win);
  calc->SetInputConnection(0,im->GetOutputPort(0)); // reader or source

  // We just state that Result:=exp(sqrt(sin(values)*cos(values)+10.0))
  calc->AddScalarArrayName("values");
  calc->SetResultArrayName("Result");
  calc->SetFunction("exp(sqrt(sin(values)*cos(values)+10.0))");

  // Perform the computation
  calc->Update();

  // The result in in calc2->GetOutput().


  calc->Delete();
  win->Delete();
  im->Delete();

  return 0; // !retVal; 0: passed, 1: failed
}

#ifndef KWE_TESTING
int main(int argc, char* argv[])
{
  return GPUArrayCalculatorDemo(argc, argv);
}
#endif
