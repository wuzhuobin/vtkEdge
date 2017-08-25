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

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushRepresentationGrayscale2D.h"
#include "vtkKWEPaintbrushMergeSketches.h"
//#include "vtkITKPaintbrushExtractConnectedComponents.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushStroke.h"
#include "vtkKWEPaintbrushShapeEllipsoid.h"
#include "vtkKWEPaintbrushShapeBox.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushBlend.h"
#include "vtkKWEPaintbrushAnnotationRepresentation.h"
#include "vtkKWEPaintbrushAnnotationWidget.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushPropertyManager.h"
#include "vtkKWEWidgetGroup.h"
#include "vtkGarbageCollector.h"

int PaintbrushInstantiatonMemLeaksTest( int , char *[] )
{
  {
    vtkKWEPaintbrushWidget *o = vtkKWEPaintbrushWidget::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushAnnotationWidget *o = vtkKWEPaintbrushAnnotationWidget::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushRepresentationGrayscale2D *o = vtkKWEPaintbrushRepresentationGrayscale2D::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushRepresentation2D *o = vtkKWEPaintbrushRepresentation2D::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushAnnotationRepresentation *o = vtkKWEPaintbrushAnnotationRepresentation::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushStencilData *o = vtkKWEPaintbrushStencilData::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushGrayscaleData *o = vtkKWEPaintbrushGrayscaleData::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushLabelData *o = vtkKWEPaintbrushLabelData::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushDrawing *o = vtkKWEPaintbrushDrawing::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushSketch *o = vtkKWEPaintbrushSketch::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushStroke *o = vtkKWEPaintbrushStroke::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushShapeEllipsoid *o = vtkKWEPaintbrushShapeEllipsoid::New();
    double w[3];
    o->GetWidth(w);
    (void)w;
    o->SetResolution(10);
    int res = o->GetResolution();
    if (res != 10)
      {
      return EXIT_FAILURE;
      }
    vtkKWEPaintbrushShapeEllipsoid *o2 = vtkKWEPaintbrushShapeEllipsoid::New();
    o2->DeepCopy(o);
    o2->Print(std::cout);
    o2->Delete();
    o->Delete();
  }
  {
    vtkKWEPaintbrushShapeBox *o = vtkKWEPaintbrushShapeBox::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushOperation *o = vtkKWEPaintbrushOperation::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushBlend *o = vtkKWEPaintbrushBlend::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEWidgetGroup *o = vtkKWEWidgetGroup::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    vtkKWEPaintbrushMergeSketches *o = vtkKWEPaintbrushMergeSketches::New();
    o->Print(std::cout);
    o->Delete();
  }
  {
    //vtkITKPaintbrushExtractConnectedComponents *o = vtkITKPaintbrushExtractConnectedComponents::New();
    //o->Print(std::cout);
    //o->Delete();
  }
  {
    vtkKWEWidgetGroup *os = vtkKWEWidgetGroup::New();
    vtkKWEPaintbrushWidget *o = vtkKWEPaintbrushWidget::New();
    os->AddWidget(o);
    o->Delete();
    vtkKWEPaintbrushWidget *o1 = vtkKWEPaintbrushWidget::New();
    os->AddWidget(o1);
    os->Print(std::cout);
    o1->Delete();
    os->Delete();
  }
  {
    vtkKWEWidgetGroup *os = vtkKWEWidgetGroup::New();
    vtkKWEPaintbrushWidget *o = vtkKWEPaintbrushWidget::New();
    os->AddWidget(o);
    o->Delete();
    os->RemoveWidget(o);
    vtkKWEPaintbrushWidget *o1 = vtkKWEPaintbrushWidget::New();
    os->AddWidget(o1);
    o1->Delete();
    os->Delete();
  }

  return EXIT_SUCCESS;
}

