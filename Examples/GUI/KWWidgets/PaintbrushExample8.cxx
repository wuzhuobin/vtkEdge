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
// This example loads a file IBSRData.mha and a label map IBSRLabelMap.mha.
// The label map, (courtesy of IBSR) is a label map with 62 segmentations. This
// example creates a sketch from each label and initializes the drawing with
// these sketches. It also assigns user specified colors to the sketches.

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include <sstream>
#include <string>
#include <map>

class PaintbrushExample8Labels
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

void MySelectEditCallbackMethod( vtkKWEPaintbrushWidget *w, int editMode )
{
  w->SetPaintbrushMode( editMode ? vtkKWEPaintbrushWidget::Edit :
                                   vtkKWEPaintbrushWidget::Select );
}

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

int my_example( int , char *[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  typedef PaintbrushExample8Labels::Annotation AnnotationType;
  PaintbrushExample8Labels MyLabels;
  MyLabels.LabelMap[1]=AnnotationType("Left-Cerebral-Exterior",            205 ,  62,   78 );
  MyLabels.LabelMap[2]=AnnotationType("Left-Cerebral-White-Matter",        225 , 225,  225 );
  MyLabels.LabelMap[3]=AnnotationType("Left-Cerebral-Cortex",              205 ,  62,   78 );
  MyLabels.LabelMap[4]=AnnotationType("Left-Lateral-Ventricle",            120 ,  18,  134 );
  MyLabels.LabelMap[5]=AnnotationType("Left-Inf-Lat-Vent",                 196 ,  58,  250 );
  MyLabels.LabelMap[6]=AnnotationType("Left-Cerebellum-Exterior",            0 , 148,    0 );
  MyLabels.LabelMap[7]=AnnotationType("Left-Cerebellum-White-Matter",      220 , 248,  164 );
  MyLabels.LabelMap[8]=AnnotationType("Left-Cerebellum-Cortex",            230 , 148,   34 );
  MyLabels.LabelMap[9]=AnnotationType("Left-Thalamus",                       0 , 118,   14 );
  MyLabels.LabelMap[10]=AnnotationType("Left-Thalamus-Proper",               0 , 118,   14 );
  MyLabels.LabelMap[11]=AnnotationType("Left-Caudate",                     122 , 186,  220 );
  MyLabels.LabelMap[12]=AnnotationType("Left-Putamen",                     236 ,  13,  176 );
  MyLabels.LabelMap[13]=AnnotationType("Left-Pallidum",                     12 ,  48,  255 );
  MyLabels.LabelMap[14]=AnnotationType("3rd-Ventricle",                    204 , 182,  142 );
  MyLabels.LabelMap[15]=AnnotationType("4th-Ventricle",                     42 , 204,  164 );
  MyLabels.LabelMap[16]=AnnotationType("Brain-Stem",                       119 , 159,  176 );
  MyLabels.LabelMap[17]=AnnotationType("Left-Hippocampus",                 220 , 216,   20 );
  MyLabels.LabelMap[18]=AnnotationType("Left-Amygdala",                    103 , 255,  255 );
  MyLabels.LabelMap[19]=AnnotationType("Left-Insula",                       80 , 196,   98 );
  MyLabels.LabelMap[20]=AnnotationType("Left-Operculum",                    60 ,  58,  210 );
  MyLabels.LabelMap[21]=AnnotationType("Line-1",                            60 ,  58,  210 );
  MyLabels.LabelMap[22]=AnnotationType("Line-2",                            60 ,  58,  210 );
  MyLabels.LabelMap[23]=AnnotationType("Line-3",                            60 ,  58,  210 );
  MyLabels.LabelMap[24]=AnnotationType("CSF",                               60 ,  60,   60 );
  MyLabels.LabelMap[25]=AnnotationType("Left-Lesion",                      255 , 165,    0 );
  MyLabels.LabelMap[26]=AnnotationType("Left-Accumbens-area",              255 , 165,    0 );
  MyLabels.LabelMap[27]=AnnotationType("Left-Substancia-Nigra",              0 , 255,  127 );
  MyLabels.LabelMap[28]=AnnotationType("Left-VentralDC",                   165 ,  42,   42 );
  MyLabels.LabelMap[29]=AnnotationType("Left-undetermined",                135 , 206,  235 );
  MyLabels.LabelMap[30]=AnnotationType("Left-vessel",                      160 ,  32,  240 );
  MyLabels.LabelMap[31]=AnnotationType("Left-choroid-plexus",                0 , 255,  255 );
  MyLabels.LabelMap[32]=AnnotationType("Left-F3orb",                       100 ,  50,  100 );
  MyLabels.LabelMap[33]=AnnotationType("Left-lOg",                         135 ,  50,   74 );
  MyLabels.LabelMap[34]=AnnotationType("Left-aOg",                         122 , 135,   50 );
  MyLabels.LabelMap[35]=AnnotationType("Left-mOg",                          51 ,  50,  135 );
  MyLabels.LabelMap[36]=AnnotationType("Left-pOg",                          74 , 155,   60 );
  MyLabels.LabelMap[37]=AnnotationType("Left-Stellate",                    120 ,  62,   43 );
  MyLabels.LabelMap[38]=AnnotationType("Left-Porg",                         74 , 155,   60 );
  MyLabels.LabelMap[39]=AnnotationType("Left-Aorg",                        122 , 135,   50 );
  MyLabels.LabelMap[40]=AnnotationType("Right-Cerebral-Exterior",          205 ,  62,   78 );
  MyLabels.LabelMap[41]=AnnotationType("Right-Cerebral-White-Matter",        0 , 225,    0 );
  MyLabels.LabelMap[42]=AnnotationType("Right-Cerebral-Cortex",            205 ,  62,   78 );
  MyLabels.LabelMap[43]=AnnotationType("Right-Lateral-Ventricle",          120 ,  18,  134 );
  MyLabels.LabelMap[44]=AnnotationType("Right-Inf-Lat-Vent",               196 ,  58,  250 );
  MyLabels.LabelMap[45]=AnnotationType("Right-Cerebellum-Exterior",          0 , 148,    0 );
  MyLabels.LabelMap[46]=AnnotationType("Right-Cerebellum-White-Matter",    220 , 248,  164 );
  MyLabels.LabelMap[47]=AnnotationType("Right-Cerebellum-Cortex",          230 , 148,   34 );
  MyLabels.LabelMap[48]=AnnotationType("Right-Thalamus",                     0 , 118,   14 );
  MyLabels.LabelMap[49]=AnnotationType("Right-Thalamus-Proper",              0 , 118,   14 );
  MyLabels.LabelMap[50]=AnnotationType("Right-Caudate",                    122 , 186,  220 );
  MyLabels.LabelMap[51]=AnnotationType("Right-Putamen",                    236 ,  13,  176 );
  MyLabels.LabelMap[52]=AnnotationType("Right-Pallidum",                   255 ,  48,  255 );
  MyLabels.LabelMap[53]=AnnotationType("Right-Hippocampus",                220 , 216,   20 );
  MyLabels.LabelMap[54]=AnnotationType("Right-Amygdala",                   103 , 255,  255 );
  MyLabels.LabelMap[55]=AnnotationType("Right-Insula",                      80 , 196,   98 );
  MyLabels.LabelMap[56]=AnnotationType("Right-Operculum",                   60 ,  58,  210 );
  MyLabels.LabelMap[57]=AnnotationType("Right-Lesion",                     255 , 165,    0 );
  MyLabels.LabelMap[58]=AnnotationType("Right-Accumbens-area",             255 , 165,    0 );
  MyLabels.LabelMap[59]=AnnotationType("Right-Substancia-Nigra",             0 , 255,  127 );
  MyLabels.LabelMap[60]=AnnotationType("Right-VentralDC",                  165 ,  42,   42 );
  MyLabels.LabelMap[61]=AnnotationType("Right-undetermined",               135 , 206,  235 );
  MyLabels.LabelMap[62]=AnnotationType("Right-vessel",                     160 ,  32,  240 );

  example->SetFilename( vtkKWMyWindow::ExpandFilename("IBSRData.mha").c_str());
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( example->GetNthRenderWidget(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentation2D * rep =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());
    if (rep)
      {
      vtkImageActor * imageActor = example->GetNthImageActor(i);
      rep->SetImageActor(imageActor);
      rep->SetImageData(imageData);
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing(
          imageData->GetSpacing() );
      rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin(
          imageData->GetOrigin() );
      }

    // This will simply cause the method
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Edit ) or
    //   w->SetPaintbrushMode( vtkKWEPaintbrushWidget::Select )
    // to be invoked based on which radio button is depressed.
    example->SetSelectEditCallbackMethod( MySelectEditCallbackMethod );
    example->AddSelectEditCallback(w);

    set->AddWidget(w);
    w->Delete();
    }

  // Now read in an initial segmentation.

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());

  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  drawing->InitializeData();

  // Clear the drawing and start on a clean slate. The drawing would have
  // automatically created an empty sketch for us, so we can start drawing
  // right away. We will wipe it all off.
  drawing->RemoveAllItems();

  // Read the label map
  vtkMetaImageReader * reader = vtkMetaImageReader::New();
  reader->SetFileName( vtkKWMyWindow::ExpandFilename("IBSRLabelMap.mha").c_str() );
  reader->Update();

  // Create a binary sketch for each label in the label map.

  for (std::map< unsigned short, AnnotationType >::const_iterator cit =
      MyLabels.LabelMap.begin(); cit != MyLabels.LabelMap.end(); ++cit)
    {

    // Note the use of the functor to check the label in the label map.

    vtkKWEPaintbrushStencilData *stencilData = vtkKWEPaintbrushStencilData::New();
    vtkKWEPaintbrushUtilities::GetStencilFromImage<
      vtkKWEPaintbrushUtilities::vtkFunctorEqualTo >(
        reader->GetOutput(), stencilData->GetImageStencilData(), cit->first);

    vtkKWEPaintbrushSketch * sketch = vtkKWEPaintbrushSketch::New();
    drawing->AddItem( sketch );
    sketch->AddNewStroke( vtkKWEPaintbrushEnums::Draw, stencilData );

    // Optionally, the identifier can be used to display text annotations as
    // you navigate through your maze of sketches. 62 segmentations can sure
    // get you lost pretty easily.
    sketch->GetPaintbrushProperty()->SetIdentifier(cit->second.label.c_str());

    // Assign a nice color to the sketch.
    double sketchColor[3] = { (double)(cit->second.R)/255.0,
                              (double)(cit->second.G)/255.0,
                              (double)(cit->second.B)/255.0 };
    sketch->GetPaintbrushProperty()->SetColor( sketchColor );
    sketch->Delete();

    }

  reader->Delete();

  // Now set the drawing on all representations.

  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  // Enable the widget and we are done.
  set->SetEnabled(1);
  int res = example->Run();


  // Save out each of the resulting segmentations. We will write out each
  // sketch in the drawing as a seperate file.
/*
  for (int i = 0; i < drawing->GetNumberOfItems(); i++)
    {
    vtkImageData * outputN = vtkImageData::New();
    drawing->GetItem(i)->GetPaintbrushData()->
                    GetPaintbrushDataAsImageData( outputN );
    vtkMetaImageWriter * outputWriterN = vtkMetaImageWriter::New();
    std::ostringstream filenameN;
    filenameN << "Drawing" << i << ".mha" << std::ends;
    outputWriterN->SetFileName(filenameN.str().c_str());
    outputWriterN->SetInput( outputN );
    outputWriterN->Write();
    outputWriterN->Delete();
    outputN->Delete();
    }
  */
  drawing->Delete();
  set->Delete();

  return res;
}

