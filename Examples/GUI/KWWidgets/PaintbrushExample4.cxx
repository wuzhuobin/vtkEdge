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
// This example differs from PaintbrushExample3 in that it renders the brush in
// as contours. Note while drawing the increased resolution obtained as a
// result. Note also the differences in the way in which the paintbrush data
// is set as the initial segmentation below.
//
// The abstract class vtkKWEPaintbrushData has two concrete subclasses :
// vtkKWEPaintbrushStencilData and vtkPaintrushGrayscaleData. This example uses
// the latter.
//
// Aside from the aforementioned difference, follow the documentation of the
// previous example.

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentationGrayscale2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include <sstream>

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

void MySelectEditCallbackMethod( vtkKWEPaintbrushWidget *w, int editMode )
{
  w->SetPaintbrushMode( editMode ? vtkKWEPaintbrushWidget::Edit :
                                   vtkKWEPaintbrushWidget::Select );
}

int my_example( int argc, char *argv[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();

  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( example->GetNthRenderWidget(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentationGrayscale2D * rep =
                vtkKWEPaintbrushRepresentationGrayscale2D::New();
    w->SetRepresentation(rep);
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
    rep->Delete();
    w->Delete();
    }

  vtkKWEPaintbrushRepresentation2D * repx =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());
  vtkKWEPaintbrushDrawing * drawingx = repx->GetPaintbrushDrawing();
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr =
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawingx );
    }

  // Now read in an initial segmentation.

  typedef vtksys::CommandLineArguments argT;
  argT arg;
  arg.Initialize(argc, argv);
  std::string * filename = NULL, fn;
  arg.AddArgument("--initial_segmentation", argT::SPACE_ARGUMENT, filename,
                  "Initial segmentation");
  arg.Parse();

  if (!filename)
    {
    vtkstd::vector< vtksys_stl::string > stringArray;
    stringArray.push_back(vtkKWMyWindow::GetExampleDataPath());
    stringArray.push_back("Ventricles.mha");
    fn = vtksys::SystemTools::JoinPath(stringArray);
    vtksys::SystemTools::ConvertToUnixSlashes(fn);
    filename = &fn;
    }

  vtkKWEPaintbrushRepresentation2D * rep =
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());

  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  drawing->InitializeData();
  vtkKWEPaintbrushGrayscaleData *data = vtkKWEPaintbrushGrayscaleData::New();

  if (filename && vtksys::SystemTools::FileExists(
                                filename->c_str()))
    {
    // Read the file
    vtkMetaImageReader * reader = vtkMetaImageReader::New();
    reader->SetFileName( filename->c_str() );
    reader->Update();
    data->SetImageData(reader->GetOutput());
    reader->Delete();

    // Set the first stroke as the initial segmentation.
    drawing->AddNewStroke(0, vtkKWEPaintbrushEnums::Draw, data);

    // Instead of the above line, you can set this directly as the new stencil,
    // saves some memory, as the overhead data of an extra stroke need not be
    // managed.
    // drawing->SetPaintbrushData(data);

    for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
      {
      vtkKWEPaintbrushRepresentation * repr =
        vtkKWEPaintbrushRepresentation::SafeDownCast(
          set->GetNthWidget(i)->GetRepresentation());
      repr->SetPaintbrushDrawing( drawing );
      }
    }

  set->SetEnabled(1);
  int res = example->Run();

  // Save out the resulting segmentation.
  //
  vtkImageData * output = vtkImageData::New();
  drawing->GetPaintbrushData()->
                    GetPaintbrushDataAsImageData( output );
  vtkMetaImageWriter * outputWriter = vtkMetaImageWriter::New();
  outputWriter->SetFileName("Drawing.mha");
  outputWriter->SetInput( output );
  outputWriter->Write();

  // Save out each of the resulting segmentations.
  //
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

  data->Delete();
  outputWriter->Delete();
  output->Delete();
  set->Delete();

  return res;
}

