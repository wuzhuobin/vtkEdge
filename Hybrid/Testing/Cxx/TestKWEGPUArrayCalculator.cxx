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

// This test covers vtkKWEGPUArrayCalulator, compare to vtkArrayCalculator.
// The command line arguments are:
// -I        => run in interactive mode; unless this is used, the program will
//              not allow interaction and exit

// An image data is first created with a raw array.
// Some computation on the data array is performed

// IF THIS TEST TIMEOUT, IT IS PROBABLY ON VISTA WITH A NOT-FAST-ENOUGH
// GRAPHICS CARD:
// Depending on how fast/slow is the graphics card, the computation on the GPU
// can take more than 2 seconds. On Vista, after a 2 seconds timeout, the
// Windows Vista's Timeout Detection and Recovery (TDR) kills all the graphics
// contexts, resets the graphics chip and recovers the graphics driver, in
// order to keep the operating system responsive.
// ref: http://www.opengl.org/pipeline/article/vol003_7/
// This reset actually freezes the test. And it really times out this time...
// Example of pathological case: dash1vista32.kitware/Win32Vista-vs80

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

#include "vtkArrayCalculator.h"
#include "vtkKWEGPUArrayCalculator.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkOutlineFilter.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageImport.h"
#include <assert.h>
#include "vtkTimerLog.h"
#include "vtkDoubleArray.h"
#include "vtkXYPlotWidget.h"
#include "vtkXYPlotActor.h"

// On a nVidia Quadro FX 3600M (~GeForce8),
// GL_MAX_TEXTURE_SIZE = 8192, GL_MAX_3D_TEXTURE_SIZE = 2048
// GL_MAX_VIEWPORT_DIMS = 8192, 8192

// The following size matches two complete full 2D textures, one 2D texture
// with one complete row and one 2D texture with just one pixel.
//const vtkIdType TestNumberOfPoints=2*(8192*8192)+8192+1;

const vtkIdType TestNumberOfPoints=(2*(8192*8192)+8192+1)/10; // 2*(8192*8192)+8192+1; //4096*4096;
const int TestNumberOfComponents=1; // 1 or 3

void ComputeAndDisplayMeanErrorAndStandardDeviation(vtkDoubleArray *c,
                                                    vtkDoubleArray *g)
{
  assert("pre: c_exists" && c!=0);
  assert("pre: g_exists" && g!=0);
  assert("pre: same_size" && c->GetNumberOfTuples()==g->GetNumberOfTuples());
  assert("pre: same_components" && c->GetNumberOfComponents()==g->GetNumberOfComponents());
  assert("pre: not_empty"  && c->GetNumberOfTuples()>0);

  vtkIdType n=c->GetNumberOfTuples()*c->GetNumberOfComponents();
  double *a=static_cast<double *>(c->GetVoidPointer(0));
  double *b=static_cast<double *>(g->GetVoidPointer(0));

  vtkIdType i;

  // mean error
  double meanError=0.0;
  double maxError=0.0;

  i=0;
  while(i<n)
    {
      double delta=fabs(a[i]-b[i]);
      if(delta>maxError)
        {
          maxError=delta;
        }

      meanError+=delta;
      ++i;
    }
  meanError/=static_cast<double>(n);

  // std deviation
  double stdDeviation=0.0;

  i=0;
  while(i<n)
    {
      double delta=fabs(a[i]-b[i])-meanError;
      stdDeviation+=delta*delta;
      ++i;
    }
  stdDeviation=sqrt(stdDeviation/static_cast<double>(n));
  cout<<" number of values="<<n<<endl;
  cout<<" mean error="<<meanError<<endl;
  cout<<" standard deviation="<<stdDeviation<<endl;
  cout<<" maxError="<<maxError<<endl;
}


vtkImageImport *CreateSource(vtkIdType numberOfPoints,
                             int numberOfComponents)
{
  assert("pre: valid_number_of_points" && numberOfPoints>0);
  vtkImageImport *im=vtkImageImport::New();
  float *ptr=new float[numberOfPoints*numberOfComponents];
  vtkIdType i=0;
  while(i<numberOfPoints*numberOfComponents)
    {
    ptr[i]=static_cast<float>(i); // 2.0
    ++i;
    }
  im->SetDataScalarTypeToFloat();
  im->SetImportVoidPointer(ptr,0); // let the importer delete it.
  im->SetNumberOfScalarComponents(numberOfComponents);
  im->SetDataExtent(0,static_cast<int>(numberOfPoints-1),0,0,0,0);
  im->SetWholeExtent(0,static_cast<int>(numberOfPoints-1),0,0,0,0);
  im->SetScalarArrayName("values");
  return im;
}

int TestKWEGPUArrayCalculator(int vtkNotUsed(argc),
                              char *vtkNotUsed(argv)[])
{
  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->SetSize(101,99);
  renWin->SetAlphaBitPlanes(1);
  renWin->SetReportGraphicErrors(1);
  iren->SetRenderWindow(renWin);
  renWin->Delete();

  // Useful when looking at the test output in CDash:
  cout << "IF THIS TEST TIMEOUT, IT IS PROBABLY ON VISTA WITH"
       << " A NOT-FAST-ENOUGH GRAPHICS CARD."<<endl;

  cout<<"numPoints="<<TestNumberOfPoints<<endl;
  vtkImageImport *im=CreateSource(TestNumberOfPoints,TestNumberOfComponents);

  im->Update();

  double range[2];
   vtkImageData *image=vtkImageData::SafeDownCast(im->GetOutputDataObject(0));
  image->GetPointData()->GetScalars()->GetRange(range);
  cout<<"range[2]="<<range[0]<<" "<<range[1]<<endl;

  vtkArrayCalculator *calc=vtkArrayCalculator::New();
  calc->SetInputConnection(0,im->GetOutputPort(0)); // reader or source
  calc->AddScalarArrayName("values");
  calc->SetResultArrayName("Result");
    calc->SetFunction("values+10.0");
  //  calc->SetFunction("sin(norm(values))+cos(norm(values))+10.0");
  //      calc->SetFunction("sin(values)*cos(values)+10.0");
    //    calc->SetFunction("exp(sqrt(sin(values)*cos(values)+10.0))");
  vtkTimerLog *timer=vtkTimerLog::New();

  timer->StartTimer();
  calc->Update();
  timer->StopTimer();
  cout<<"Elapsed time with CPU version:"<<timer->GetElapsedTime()<<" seconds."<<endl;

  image=vtkImageData::SafeDownCast(calc->GetOutputDataObject(0));
  image->GetPointData()->GetScalars()->GetRange(range);
  cout<<"range2[2]="<<range[0]<<" "<<range[1]<<endl;


  vtkKWEGPUArrayCalculator *calc2=vtkKWEGPUArrayCalculator::New();
  calc2->SetContext(renWin);
  calc2->SetInputConnection(0,im->GetOutputPort(0)); // reader or source

  calc2->AddScalarArrayName("values");
  calc2->SetResultArrayName("Result");
calc2->SetFunction("values+10");
  //  calc2->SetFunction("sin(norm(values))+cos(norm(values))+10.0");
  //    calc2->SetFunction("sin(values)*cos(values)+10.0");
//    calc2->SetFunction("exp(sqrt(sin(values)*cos(values)+10.0))");
  // my nVidia Quadro FX 3600M has 512MB
  // esimatation of 28MB already taken for screen+current context
  // at 1600*1200: 512-28=484
  //  calc2->SetMaxGPUMemorySizeInBytes(512*1024*1024);
  calc2->SetMaxGPUMemorySizeInBytes(128*1024*1024);
  timer->StartTimer();
  calc2->Update();
  timer->StopTimer();
  cout<<"Elapsed time with GPU version:"<<timer->GetElapsedTime()<<" seconds."<<endl;
  vtkImageData *image2;
  image2=vtkImageData::SafeDownCast(calc2->GetOutputDataObject(0));
  image2->GetPointData()->GetScalars()->GetRange(range);
  cout<<"range3[2]="<<range[0]<<" "<<range[1]<<endl;

  ComputeAndDisplayMeanErrorAndStandardDeviation(
                                                 vtkDoubleArray::SafeDownCast(image->GetPointData()->GetScalars()),
                                                 vtkDoubleArray::SafeDownCast(image2->GetPointData()->GetScalars()));

  timer->Delete();

  calc2->Calibrate();
  cout<<"Calibrated size threshold="<<calc2->GetCalibratedSizeThreshold()<<endl;

#if 0
  vtkRenderer *renderer=vtkRenderer::New();
  renderer->SetBackground(0.0,0.0,0.3);
  renWin->AddRenderer(renderer);
  renderer->Delete();

  vtkXYPlotActor *actor=vtkXYPlotActor::New();
  renderer->AddViewProp(actor);
  actor->Delete();
  actor->AddInput(image,"Result",0);
  actor->SetPlotColor(0,1.0,0.0,0.0);
  actor->AddInput(image2,"Result",0);
  actor->SetPlotColor(1,0.0,1.0,0.0);
  renWin->Render();

  vtkXYPlotWidget *w=vtkXYPlotWidget::New();
  w->SetInteractor(iren);
  w->SetXYPlotActor(actor);
  w->SetEnabled(1);

  int retVal = vtkRegressionTestImage(renWin);
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }

  w->Delete();
#endif

  calc2->Delete(); // need context

  iren->Delete();

  im->Delete();
  calc->Delete();

  return 0; // !retVal; 0: passed, 1: failed
}
