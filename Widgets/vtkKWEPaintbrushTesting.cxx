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

#include "vtkKWEPaintbrushTesting.h"

#include "vtkBMPReader.h"
#include "vtkCommand.h"
#include "vtkImageReader2.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageViewer2.h"
#include "vtkMetaImageReader.h"
#include "vtkObjectFactory.h"
#include "vtkPNGReader.h"
#include "vtkPNMReader.h"
#include "vtkToolkits.h"
#include "vtkXMLImageDataReader.h"
#include "vtkCollection.h"
#include "vtkSliderRepresentation.h"
#include "vtkSliderRepresentation2D.h"
#include "vtkSliderWidget.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty2D.h"

#include "vtksys/ios/sstream"


class vtkKWEPaintbrushTestingSliderCallback : public vtkCommand
{
public:
  static vtkKWEPaintbrushTestingSliderCallback *New()
    { return new vtkKWEPaintbrushTestingSliderCallback; }
  void SetImageViewer(vtkImageViewer2 *viewer)
    { m_Viewer =  viewer; }
  virtual void Execute(vtkObject *caller, unsigned long , void* )
    {
      vtkSliderWidget *slider = static_cast<vtkSliderWidget *>(caller);
      vtkSliderRepresentation *sliderRepres = static_cast<vtkSliderRepresentation *>(slider->GetRepresentation());
      int pos = static_cast<int>(sliderRepres->GetValue());

    m_Viewer->SetSlice(pos);
    }
protected:
  vtkImageViewer2 *m_Viewer;
};

static const char *vtkKWEPaintbrushTestingWindowStrings[] = {
  "Paintbrush - Axial",
  "Paintbrush - Coronal",
  "Paintbrush - Sagittal",
  NULL
};

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWEPaintbrushTesting );
vtkCxxRevisionMacro(vtkKWEPaintbrushTesting, "$Revision: 1774 $");
vtkCxxSetObjectMacro( vtkKWEPaintbrushTesting, Input, vtkImageData );

//----------------------------------------------------------------------------
vtkKWEPaintbrushTesting::vtkKWEPaintbrushTesting()
{
  for (int i = 0; i < 3; i++)
    {
    this->ImageViewer[i] = vtkImageViewer2::New();
    vtkRenderer *renderer = vtkRenderer::New();
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);
    this->ImageViewer[i]->SetRenderWindow(renWin);
    this->ImageViewer[i]->SetRenderer(renderer);
    this->ImageViewer[i]->SetupInteractor(iren);

    renWin->SetWindowName( vtkKWEPaintbrushTestingWindowStrings[i] );

    this->SliderCallbacks[i] = vtkKWEPaintbrushTestingSliderCallback::New();
    this->SliderCallbacks[i]->SetImageViewer( this->ImageViewer[i] );

    vtkSliderRepresentation2D *SliderRepres = vtkSliderRepresentation2D::New();
    SliderRepres->SetTitleText("Slice");
    SliderRepres->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
    SliderRepres->GetPoint1Coordinate()->SetValue(0.15, 0.05);
    SliderRepres->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
    SliderRepres->GetPoint2Coordinate()->SetValue(0.85, 0.05);
    SliderRepres->SetSliderLength(0.02);
    SliderRepres->SetSliderWidth(0.03);
    SliderRepres->SetEndCapLength(0.01);
    SliderRepres->SetEndCapWidth(0.03);
    SliderRepres->SetTubeWidth(0.005);
    SliderRepres->SetLabelFormat("%3.0lf");
    SliderRepres->SetTitleHeight(0.02);
    SliderRepres->SetLabelHeight(0.03);
    SliderRepres->GetTubeProperty()->SetColor( 0, 1, 0 );
    SliderRepres->GetCapProperty()->SetColor( 0, 0.5, 1 );
    SliderRepres->GetSelectedProperty()->SetColor( 0, 1, 1 );

    this->SliderWidget[i] = vtkSliderWidget::New();
    this->SliderWidget[i]->SetInteractor(iren);
    this->SliderWidget[i]->SetRepresentation(SliderRepres);
    this->SliderWidget[i]->KeyPressActivationOff();
    this->SliderWidget[i]->SetAnimationModeToAnimate();
    SliderRepres->Delete();

    this->SliderWidget[i]->AddObserver(vtkCommand::InteractionEvent, this->SliderCallbacks[i]);

    renderer->Delete();
    renWin->Delete();
    iren->Delete();
    }

  this->Filename = NULL;
  this->FourPaneView = 1;
  this->Input = NULL;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushTesting::~vtkKWEPaintbrushTesting()
{
  for (int i = 0; i < 3; i++)
    {
    this->ImageViewer[i]->Delete();
    this->SliderCallbacks[i]->Delete();
    this->SliderWidget[i]->Delete();
    }

  if (this->Filename)
    {
    delete [] this->Filename;
    }
  this->SetInput(NULL);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushTesting::Initialize()
{
  // Read the data, if an input hasn't already been specified

  if (!this->Input)
    {
    vtkImageReader2 *reader;
    this->GetValidReader( reader, this->Filename );
    if (!reader)
      {
      std::cerr << "Cannot read input image file ! " << std::endl;
      return;
      }
    reader->Update();
    this->SetInput(reader->GetOutput());
    reader->Delete();
    }

  // Create an image viewer
  // set the render window and renderer of the renderwidget
  //
  const int size = 350;

  if (this->FourPaneView == 0)
    {

    this->ImageViewer[0]->SetInput(this->Input);

    // Reset the window/level and the camera
    double *range = this->Input->GetScalarRange();
    this->ImageViewer[0]->SetColorWindow(range[1] - range[0]);
    this->ImageViewer[0]->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);

    int min = ImageViewer[0]->GetSliceMin();
    int max = ImageViewer[0]->GetSliceMax();
    vtkSliderRepresentation2D *sliderRepres =
      vtkSliderRepresentation2D::SafeDownCast(
           this->SliderWidget[0]->GetRepresentation());
    sliderRepres->SetMinimumValue(min);
    sliderRepres->SetMaximumValue(max);
    sliderRepres->SetValue(static_cast<int>((min + max) / 2));

    ImageViewer[0]->SetSlice(static_cast<int>(sliderRepres->GetValue()));
    this->SliderWidget[0]->EnabledOn();
    this->ImageViewer[0]->SetSize( size, size );
    this->ImageViewer[0]->SetPosition( 0, 0 );

    }
  else
    {
    for (int i = 0; i < 3; i++)
      {
      this->ImageViewer[i]->SetInput(this->Input);

      // Reset the window/level and the camera

      double *range = this->Input->GetScalarRange();
      this->ImageViewer[i]->SetColorWindow(range[1] - range[0]);
      this->ImageViewer[i]->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);
      this->ImageViewer[i]->SetSliceOrientation(2-i);

      int min = ImageViewer[i]->GetSliceMin();
      int max = ImageViewer[i]->GetSliceMax();
      vtkSliderRepresentation2D *sliderRepres =
        vtkSliderRepresentation2D::SafeDownCast(
             this->SliderWidget[i]->GetRepresentation());
      sliderRepres->SetMinimumValue(min);
      sliderRepres->SetMaximumValue(max);
      sliderRepres->SetValue(static_cast<int>((min + max) / 2));

      ImageViewer[i]->SetSlice(static_cast<int>(sliderRepres->GetValue()));
      this->SliderWidget[i]->EnabledOn();

      this->ImageViewer[i]->SetSize( size, size );
      this->ImageViewer[i]->SetPosition(
        static_cast<int>(1.2 * size * (i%2)),
        static_cast<int>(1.2 * size * (i/2)) );
      }
    }
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushTesting::Run()
{
  if (this->FourPaneView)
    {
    for (int i = 0; i < 3; i++)
      {
      this->ImageViewer[i]->GetRenderWindow()->GetInteractor()->Start();
      }
    }
  else
    {
    this->ImageViewer[0]->GetRenderWindow()->GetInteractor()->Start();
    }

  return 1;
}

//----------------------------------------------------------------------------
vtkImageActor * vtkKWEPaintbrushTesting::GetNthImageActor( int i )
{
  return this->ImageViewer[i]->GetImageActor();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushTesting::GetValidReader(
    vtkImageReader2* & reader, const char *filename)
{
  if (filename == NULL)
    {
    return;
    }

  reader = vtkMetaImageReader::New();
  reader->SetFileName(filename);
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }

/*
 * Damn ! I can't read .vtk files. they don't subclass from vtkImageReader2
 * TODO : put a work around for VTK files later
 *
 * reader = vtkXMLImageDataReader::New();
  int valid = reader->CanReadFile(filename);
  if (reader->CanReadFile(filename) == 3)
    {
    return;
    }
*/

  reader->Delete();
  reader = NULL;
}

