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

// This test loads a file IBSRData.mha and a label map IBSRLabelMap-UCHAR.mha.
// The label map, (courtesy of IBSR) is a label map with 62 segmentations. This
// test creates a sketch from each label and initializes the drawing with
// these sketches. It also assigns user specified colors to the sketches.
// ----------------------------------------------------------------------------
//
// User interactions:
//
// The follwing interactions are supported by the brush in Edit mode.
//   Left click and drag       - Draws a stroke
//   Ctrl-Left click and drag  - Erases the stroke
//   Backspace or Left key     - Undo the sketch (deletes the previously drawn stroke in a sketch)
//   Right key                 - Redo on the given sketch
//   Shift "+"                 - Start a new sketch.
//   Shift Right               - Traverse to the next sketch.
//   Shift Left                - Traverse to the previous sketch.
//   Shift Backspace           - Delete a sketch.
//   Right click and drag      - Resize the shape isotropically
//   Ctrl-Right click and drag - Resize the shape anisotropically
//   Shift "<"                 - Decrease the opacity of the overlaid drawing
//   Shift ">"                 - Increase the opacity of the overlaid drawing
//   "l" key                   - Render the current sketch immutable/mutable.
//                               (Supported only when editing labelmaps)
//   Escape                    - Enable/Disable interactions
//
// The follwing interactions are supported by the brush in Select Mode
//   Left click                - Select/Unselect a sketch
//   Left click and drag       - Drag and drop merge of sketches.
//   Backspace key             - Delete selected sketches.
//   "m" key                   - Merge selected sketches.
//   Escape                    - Enable/Disable interactions


#include "vtkInteractorEventRecorder.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageViewer2.h"
#include "vtkRenderer.h"
#include "vtkRenderer.h"
#include "vtkCommand.h"
#include "vtkCamera.h"

#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"

#include "vtkRegressionTestImage.h"
#include "vtkDebugLeaks.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushProperty.h"

#include "vtkSliderRepresentation.h"
#include "vtkSliderRepresentation2D.h"
#include "vtkSliderWidget.h"
#include "vtkMetaImageReader.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushDrawingStatistics.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include <sstream>
#include <string>
#include <map>

//-------------------------------------------------------------------------
class PaintbrushTest1cLabels
{
public:
  class Annotation
    {
    public:
      Annotation() {};
      Annotation(const std::string &l, unsigned char r, unsigned char g, unsigned char b )
        { label = l; R = r; G = g; B = b; }
      bool operator!=(const Annotation &a) const { return a.label != this->label; }
      std::string label; unsigned char R, G, B;
    };
  std::map< unsigned short, Annotation > LabelMap;
};

//-------------------------------------------------------------------------
class vtkSliderCallback100c : public vtkCommand
{
public:
  static vtkSliderCallback100c *New()
    { return new vtkSliderCallback100c; }
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

//-------------------------------------------------------------------------
class vtkKWEPaintbrushDrawingStatisticsCallback : public vtkCommand
{
public:
  static vtkKWEPaintbrushDrawingStatisticsCallback *New()
    { return new vtkKWEPaintbrushDrawingStatisticsCallback; }
  void SetPaintbrushDrawingStatistics(vtkKWEPaintbrushDrawingStatistics *s)
    { m_PaintbrushDrawingStatistics = s; }
  virtual void Execute(vtkObject *caller, unsigned long , void* )
    {
    vtkKWEPaintbrushDrawing *drawing = m_PaintbrushDrawingStatistics->GetDrawing();
    std::cout << "---------" << std::endl;
    m_PaintbrushDrawingStatistics->Update();
    for (unsigned int i = 0; i < drawing->GetNumberOfItems(); i++)
      {
      vtkKWEPaintbrushSketch * sketch = drawing->GetItem(i);
      std::cout << "Volume of " 
        << sketch->GetPaintbrushProperty()->GetIdentifier() << " is " 
        << m_PaintbrushDrawingStatistics->GetVolume(sketch) << "mm^3" 
        << std::endl;
      }
    std::cout << "Total volume of all sketches is " << 
      m_PaintbrushDrawingStatistics->GetVolume() << "mm^3" << std::endl;
    }
protected:
  vtkKWEPaintbrushDrawingStatistics *m_PaintbrushDrawingStatistics;
};


int PaintbrushTest1c( int argc , char *argv[])
{
  if( argc < 6 )
    {
    std::cerr << "Usage error: \t " << argv[0] << "\t"
              << "Input_Image_Data  InitialLabelMap EventsLogFile Mode(0:replay 1:record)"
              << "[OutputLabelMap]" << std::endl;
    return EXIT_FAILURE;
    }

  vtkImageReader2 * reader = vtkMetaImageReader::New();

  reader->SetFileName( argv[1] );
  if( reader->CanReadFile( argv[1] ) != 3 )
    {
    std::cerr << "Meta image reader couldn't read the input data\t" << std::endl;
    return EXIT_FAILURE;
    }

  reader->Update();
  vtkImageData * imageData  = reader->GetOutput();

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(renderer);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

  vtkImageViewer2 *viewer = vtkImageViewer2::New();
  viewer->SetRenderWindow(renWin);
  viewer->SetRenderer(renderer);
  viewer->SetInput(imageData);
  viewer->SetupInteractor(iren);
  double *range = imageData->GetScalarRange();
  viewer->SetColorWindow(range[1] - range[0]);
  viewer->SetColorLevel(0.5 * (range[1] + range[0])+ 0.0000001);

  // Tie a slider widget to manipulate the slices of the image viewer
  //
  vtkSliderRepresentation2D *SliderRepres = vtkSliderRepresentation2D::New();
  int min = viewer->GetSliceMin();
  int max = viewer->GetSliceMax();
  SliderRepres->SetMinimumValue(min);
  SliderRepres->SetMaximumValue(max);
  SliderRepres->SetValue(static_cast<int>((min + max) / 2));
  SliderRepres->SetTitleText("Slice");
  SliderRepres->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  SliderRepres->GetPoint1Coordinate()->SetValue(0.3, 0.05);
  SliderRepres->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  SliderRepres->GetPoint2Coordinate()->SetValue(0.7, 0.05);
  SliderRepres->SetSliderLength(0.02);
  SliderRepres->SetSliderWidth(0.03);
  SliderRepres->SetEndCapLength(0.01);
  SliderRepres->SetEndCapWidth(0.03);
  SliderRepres->SetTubeWidth(0.005);
  SliderRepres->SetLabelFormat("%3.0lf");
  SliderRepres->SetTitleHeight(0.02);
  SliderRepres->SetLabelHeight(0.02);

  vtkSliderWidget *SliderWidget = vtkSliderWidget::New();
  SliderWidget->SetInteractor(iren);
  SliderWidget->SetRepresentation(SliderRepres);
  SliderWidget->KeyPressActivationOff();
  SliderWidget->SetAnimationModeToAnimate();
  SliderWidget->SetEnabled(true);
  vtkSliderCallback100c *SliderCb = vtkSliderCallback100c::New();
  SliderCb->SetImageViewer(viewer);
  SliderWidget->AddObserver(vtkCommand::InteractionEvent, SliderCb);

  viewer->SetSlice(static_cast<int>(SliderRepres->GetValue()));



  typedef PaintbrushTest1cLabels::Annotation AnnotationType;
  PaintbrushTest1cLabels MyLabels;
  MyLabels.LabelMap[40]=AnnotationType("Right-Cerebral-Exterior",          205 ,  62,   78 );
  MyLabels.LabelMap[41]=AnnotationType("Right-Cerebral-White-Matter",        0 , 225,    0 );
  MyLabels.LabelMap[42]=AnnotationType("Right-Cerebral-Cortex",            205 ,  62,   78 );
  MyLabels.LabelMap[43]=AnnotationType("Right-Lateral-Ventricle",          120 ,  18,  134 );
  MyLabels.LabelMap[44]=AnnotationType("Right-Inf-Lat-Vent",               196 ,  58,  250 );

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 1; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( iren );
    vtkKWEPaintbrushRepresentation2D * rep =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());
    if (rep)
      {
      rep->SetImageActor( viewer->GetImageActor() );
      rep->SetImageData(imageData);
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
          imageData->GetSpacing() );
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
          imageData->GetOrigin() );
      }

    set->AddWidget(w);
    w->Delete();
    }

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());

  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();

  // Our internal representation will be to manage a label map.
  drawing->SetRepresentationToLabel();

  // This will allocate our canvas based on the size of the overlay image
  // that was set on the WidgetRepresentation.
  drawing->InitializeData();

  // Clear the drawing and start on a clean slate. The drawing would have
  // automatically created 1 empty sketch for us, so we can start drawing
  // right away. Let's remove it, since we'd like to initialize the drawing
  // with our IBSR label map.
  drawing->RemoveAllItems();

  // Read the IBSR label map
  vtkMetaImageReader * reader2 = vtkMetaImageReader::New();
  reader2->SetFileName( argv[2] );
  reader2->Update();

  // Create a PaintbrushLabelData to represent the label map.
  vtkKWEPaintbrushLabelData *paintbrushLabelMap = vtkKWEPaintbrushLabelData::New();
  paintbrushLabelMap->SetLabelMap( reader2->GetOutput() );

  // Set the label map as the initial canvas data of the drawing.
  drawing->SetPaintbrushData( paintbrushLabelMap );

  // Create a sketch for each label in the label map, we'd like to edit.
  // NOTE: We can skip labels in the label map that we do not care about simply
  // by not adding a sketch corresponding to that label into the drawing. In
  // this case, we will take into account all the labels (1-62).
  // NOTE: As a reminder, bear in mind that 0 is a reserved value and must not
  // be used as a label.

  for (std::map< unsigned short, AnnotationType >::const_iterator cit =
      MyLabels.LabelMap.begin(); cit != MyLabels.LabelMap.end(); ++cit)
    {  // For each label.

    // Initialize the sketch with the label map. Note that you must add the
    // sketch to the drawing and set the label you wish the sketch to represent
    // before you initialize it from the label map.
    //   Also note that when the representation is a "Label-map", all sketches
    // share the same label map to avoid memory duplication. The corollary to
    // this is that if you initialize sketches in your drawing using a labelmap,
    // you must use the same "vtkKWEPaintbrushLabelData" that is used by the
    // drawing.
    vtkKWEPaintbrushSketch * sketch = vtkKWEPaintbrushSketch::New();
    drawing->AddItem( sketch );
    sketch->SetLabel( cit->first );
    sketch->Initialize( paintbrushLabelMap );

    // Optionally, the identifier can be used to display text annotations as
    // you navigate through your maze of sketches. 62 segmentations can sure
    // get you lost pretty quickly.
    sketch->GetPaintbrushProperty()->SetIdentifier(cit->second.label.c_str());

    // Assign a nice color to the sketch.
    double sketchColor[3] = { cit->second.R/255.0,
                              cit->second.G/255.0,
                              cit->second.B/255.0 };
    sketch->GetPaintbrushProperty()->SetColor( sketchColor );

    sketch->Delete();
    }

  vtkKWEPaintbrushDrawingStatistics * statistics 
        = vtkKWEPaintbrushDrawingStatistics::New();
  statistics->SetInput(drawing);

  vtkKWEPaintbrushDrawingStatisticsCallback *statsCallback
    = vtkKWEPaintbrushDrawingStatisticsCallback::New();
  statsCallback->SetPaintbrushDrawingStatistics(statistics);

  // Now set the drawing on all representations (axial, coronal and sagittal)
  // in the WidgetSet.
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    set->GetNthWidget(i)->AddObserver(vtkKWEPaintbrushWidget::EndStrokeEvent, statsCallback);
    set->GetNthWidget(i)->AddObserver(vtkKWEPaintbrushWidget::UndoStrokeEvent, statsCallback);
    set->GetNthWidget(i)->AddObserver(vtkKWEPaintbrushWidget::RedoStrokeEvent, statsCallback);
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );    
    }

  // Enable the widget and we are done.
  set->SetEnabled(1);
  renWin->SetSize( 500, 500 );
  renderer->ResetCamera();
  renWin->Render();

  // record/replay events
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();
  recorder->SetInteractor(iren);

  //Set event log recording file
  std::string eventLogFileName = argv[3];
  recorder->SetFileName(eventLogFileName.c_str());

  //Set recorder mode depending on the user's choice
  int retVal = 1;
  unsigned int mode= atoi( argv[4] );
  if( mode )
    {
    std::cout << "Interaction recording....." << std::endl;
    recorder->Record();
    iren->Start();
    }
  else
    {
    recorder->On();
    std::cout << "Interaction replaying....." << std::endl;
    recorder->Play();
    //recorder must be turned off before regression testing
    recorder->Off();
    retVal = vtkRegressionTestImage( renWin );
    if ( retVal == vtkRegressionTester::DO_INTERACTOR)
      {
      recorder->GetInteractor()->Start();
      }
    }

  // Clean up
  recorder->Off();
  recorder->Delete();
  viewer->Delete();
  reader2->Delete();
  paintbrushLabelMap->Delete();
  set->Delete();
  renderer->Delete();
  renWin->Delete();
  iren->Delete();
  reader->Delete();
  SliderRepres->Delete();
  SliderCb->Delete();
  SliderWidget->Delete();
  statsCallback->Delete();
  statistics->Delete();

  return !retVal;
}

