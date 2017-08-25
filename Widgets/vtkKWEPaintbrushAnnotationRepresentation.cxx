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

#include "vtkKWEPaintbrushAnnotationRepresentation.h"

#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkInteractorObserver.h"
#include "vtkCamera.h"
#include "vtkKWEPaintbrushRepresentation.h"
#include "vtkTextProperty.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushAnnotationRepresentation, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushAnnotationRepresentation);

//----------------------------------------------------------------------
vtkKWEPaintbrushAnnotationRepresentation::vtkKWEPaintbrushAnnotationRepresentation()
{
  this->LastDisplayedAnnotation = "";
  this->Annotation             = NULL;
  this->LastDisplayPosition[0] = this->LastDisplayPosition[1] = 0;
  this->AnnotationPosition[0] = this->AnnotationPosition[1] = -1;
  this->BuildTime              = vtkTimeStamp::New();

  // Annotation stuff

  this->TextProperty = vtkTextProperty::New();
  this->TextProperty->SetBold(0);
  this->TextProperty->SetItalic(1);
  this->TextProperty->SetShadow(0);
  this->TextProperty->SetFontFamilyToArial();
  this->TextProperty->SetFontSize(5);
  this->TextMapper = vtkTextMapper::New();
  this->TextMapper->SetTextProperty(this->TextProperty);
  this->TextMapper->SetInput("0.0");
  this->TextActor = vtkActor2D::New();
  this->TextActor->SetMapper(this->TextMapper);
  this->TextActor->VisibilityOn();
}

//----------------------------------------------------------------------
vtkKWEPaintbrushAnnotationRepresentation::~vtkKWEPaintbrushAnnotationRepresentation()
{
  this->SetAnnotation(NULL);
  this->TextProperty->Delete();
  this->TextMapper->Delete();
  this->TextActor->Delete();
  this->BuildTime->Delete();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationRepresentation::BuildRepresentation()
{
  if (!this->Annotation ||
      !this->PaintbrushRepresentation ||
      !this->Renderer)
    {
    this->TextActor->VisibilityOff();
    return;
    }

  int stringSize[2], *winSize = this->Renderer->GetSize();
  vtksys_stl::string annotation(this->Annotation);

  this->TextActor->VisibilityOn();
  double disp[3] = {this->AnnotationPosition[0] * winSize[0],
                    this->AnnotationPosition[1] * winSize[1],
                    0.0 };
  this->TextMapper->SetInput(this->Annotation);

  if (disp[0] < 0)
    {
    // User did not provide a default position. Just place it where the
    // brush is.
    vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,
            this->PaintbrushRepresentation->CurrentShapePosition[0],
            this->PaintbrushRepresentation->CurrentShapePosition[1],
            this->PaintbrushRepresentation->CurrentShapePosition[2], disp);
    }

  this->TextActor->SetPosition(disp[0], disp[1]);

  // Adjust the font size
  vtkTextMapper::SetRelativeFontSize(this->TextMapper,
                this->Renderer, winSize, stringSize, 0.01f);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationRepresentation::SetVisibility(int i)
{
  this->TextActor->SetVisibility(i);
  this->Superclass::SetVisibility(i);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
  this->TextActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushAnnotationRepresentation::RenderOverlay(vtkViewport *viewport)
{
  int count = 0;
  if ( this->TextActor->GetVisibility())
    {
    count += this->TextActor->RenderOverlay(viewport);
    }
  return count;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushAnnotationRepresentation::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count = 0;
  if ( this->TextActor->GetVisibility() )
    {
    count += this->TextActor->RenderOpaqueGeometry(viewport);
    }
  return count;
}

#if VTKEdge_VTK_VERSION_DATE > 20070305
//----------------------------------------------------------------------
int vtkKWEPaintbrushAnnotationRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count=0;
  if ( this->TextActor->GetVisibility() )
    {
    count += this->TextActor->RenderTranslucentPolygonalGeometry(viewport);
    }
  return count;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushAnnotationRepresentation::HasTranslucentPolygonalGeometry()
{
  int result=0;
  if ( this->TextActor->GetVisibility() )
    {
    result |= this->TextActor->HasTranslucentPolygonalGeometry();
    }
  return result;
}

#else

//----------------------------------------------------------------------
// Legacy method to support VTK source versions prior to 2007/03/05
int vtkKWEPaintbrushAnnotationRepresentation::RenderTranslucentGeometry(vtkViewport *viewport)
{
  int count=0;
  if ( this->TextActor->GetVisibility() )
    {
    count += this->TextActor->RenderTranslucentGeometry(viewport);
    }
  return count;
}
#endif

//----------------------------------------------------------------------
void vtkKWEPaintbrushAnnotationRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

