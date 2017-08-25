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
// This paintbrushTesting loads a file IBSRData.mha and a label map IBSRLabelMap-UCHAR.mha.
// The label map, (courtesy of IBSR) is a label map with 62 segmentations. This
// paintbrushTesting creates a sketch from each label and initializes the drawing with
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



#include "vtkKWEPaintbrushTesting.h"

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
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkMultiThreader.h"
#include "vtkImageViewer2.h"
#include <sstream>
#include <string>
#include <map>

#include "vtkInteractorEventRecorder.h"
#include "vtkRegressionTestImage.h"

class PaintbrushTesting4Labels
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

int PaintbrushTest4( int argc, char *argv[] )
{
  if( argc < 6 )
    {
    std::cerr << "Usage error: \t " << argv[0] << "\t"
              << "Input_Image_Data  InitialLabelMap EventsLogFile Mode(0:replay 1:record)"
              << "[OutputLabelMap]" << std::endl;
    return EXIT_FAILURE;
    }

  typedef PaintbrushTesting4Labels::Annotation AnnotationType;
  PaintbrushTesting4Labels MyLabels;
  MyLabels.LabelMap[40]=AnnotationType("Right-Cerebral-Exterior",          205 ,  62,   78 );
  MyLabels.LabelMap[41]=AnnotationType("Right-Cerebral-White-Matter",        0 , 225,    0 );
  MyLabels.LabelMap[42]=AnnotationType("Right-Cerebral-Cortex",            205 ,  62,   78 );
  MyLabels.LabelMap[43]=AnnotationType("Right-Lateral-Ventricle",          120 ,  18,  134 );
  MyLabels.LabelMap[44]=AnnotationType("Right-Inf-Lat-Vent",               196 ,  58,  250 );


  vtkKWEPaintbrushTesting *paintbrushTesting = vtkKWEPaintbrushTesting::New();
  paintbrushTesting->SetFilename( argv[1] );
  paintbrushTesting->Initialize();
  vtkImageData * imageData  = paintbrushTesting->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( paintbrushTesting->GetNthImageViewer(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentation2D * rep =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());
    if (rep)
      {
      vtkImageActor * imageActor = paintbrushTesting->GetNthImageActor(i);
      rep->SetImageActor(imageActor);
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
  vtkMetaImageReader * reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[2] );
  reader->Update();

  // Create a PaintbrushLabelData to represent the label map.
  vtkKWEPaintbrushLabelData *paintbrushLabelMap = vtkKWEPaintbrushLabelData::New();
  paintbrushLabelMap->SetLabelMap( reader->GetOutput() );

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

  reader->Delete();

  // Now set the drawing on all representations (axial, coronal and sagittal)
  // in the WidgetSet.
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  // Enable the widget and we are done.
  set->SetEnabled(1);

  // record/replay events
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();
  //Interaction only in the first Image Viewer
  recorder->SetInteractor( paintbrushTesting->GetNthImageViewer(0)->
                      GetRenderWindow()->GetInteractor());

  //Set event log recording file
  std::string eventLogFileName = argv[3];
  recorder->SetFileName(eventLogFileName.c_str());
  recorder->On();

  //Set recorder mode depending on the user's choice
  int retVal = 1;
  unsigned int mode= atoi( argv[4] );
  if( mode )
    {
    std::cout << "Interaction recording....." << std::endl;
    recorder->Record();
    paintbrushTesting->Run();
    }
  else
    {
    std::cout << "Interaction replaying....." << std::endl;
    recorder->Play();
    //recorder must be turned off before regression testing
    recorder->Off();
    retVal = vtkRegressionTestImage(
            paintbrushTesting->GetNthImageViewer(0)->GetRenderWindow() );
    if ( retVal == vtkRegressionTester::DO_INTERACTOR)
      {
      recorder->GetInteractor()->Start();
      }
    }



  if (argc >= 6)
    {
    vtkKWEPaintbrushLabelData * finalLabelMap =
      vtkKWEPaintbrushLabelData::SafeDownCast(drawing->GetPaintbrushData());

    vtkMetaImageWriter * outputWriter = vtkMetaImageWriter::New();
    outputWriter->SetFileName(argv[5]);
    outputWriter->SetInput( finalLabelMap->GetLabelMap() );
    outputWriter->Write();
    outputWriter->Delete();
    }

  // clean up
  recorder->Off();
  recorder->Delete();
  paintbrushLabelMap->Delete();
  set->Delete();
  paintbrushTesting->Delete();

  return !retVal;

}


