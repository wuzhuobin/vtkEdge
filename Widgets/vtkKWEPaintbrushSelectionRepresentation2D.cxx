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
#include "vtkKWEPaintbrushSelectionRepresentation2D.h"

#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkProperty.h"
#include "vtkObjectFactory.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushSelectionRepresentation2D, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushSelectionRepresentation2D);

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionRepresentation2D::vtkKWEPaintbrushSelectionRepresentation2D()
{
  this->PointPlacer = vtkImageActorPointPlacer::New();
  this->DragPolyData = vtkPolyData::New();
  this->DragActor = vtkActor::New();
  vtkPolyDataMapper *dragMapper   = vtkPolyDataMapper::New();
  this->DragActor->SetMapper( dragMapper );
  dragMapper->SetInput( this->DragPolyData );
  dragMapper->SetResolveCoincidentTopologyToPolygonOffset();
  vtkCellArray * cells = vtkCellArray::New();
  vtkPoints *points = vtkPoints::New();
  cells->InsertNextCell(2);
  cells->InsertCellPoint(0);
  cells->InsertCellPoint(1);
  points->SetNumberOfPoints(2);
  this->DragPolyData->SetLines(cells);
  this->DragPolyData->SetPoints(points);
  cells->Delete();
  points->Delete();
  dragMapper->Delete();
  this->DragActor->GetProperty()->SetLineStipplePattern( 0x00ff );
}

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionRepresentation2D::~vtkKWEPaintbrushSelectionRepresentation2D()
{
  this->DragPolyData->Delete();
  this->DragActor->Delete();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation2D::SetImageActor( vtkImageActor * a)
{
  if (vtkImageActorPointPlacer * p =
      vtkImageActorPointPlacer::SafeDownCast(this->PointPlacer))
    {
    p->SetImageActor(a);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation2D::BuildRepresentation()
{
  if ( this->DragBeginPoint[0] == VTK_DOUBLE_MIN ||
       this->DragEndPoint[0]   == VTK_DOUBLE_MIN )
    {
    this->DragActor->VisibilityOff();
    return;
    }

  // Set the color of the leader to the color of the sketch that we are
  // about to drop the dragged sketch.
  this->DragActor->VisibilityOn();
  this->DragActor->GetProperty()->SetColor( this->DragAndDropDestination ?
      this->DragAndDropDestination->GetPaintbrushProperty()->GetColor() :
      this->SelectedSketch->GetPaintbrushProperty()->GetColor() );

  vtkPoints * p = this->DragPolyData->GetPoints();
  p->SetPoint( 0, this->DragBeginPoint );
  p->SetPoint( 1, this->DragEndPoint );
  this->DragPolyData->Modified();
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation2D::RenderOverlay(vtkViewport *viewport)
{
  return this->DragActor->GetVisibility() ?
    this->DragActor->RenderOverlay(viewport) : 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation2D
::RenderOpaqueGeometry(vtkViewport *viewport)
{
  this->BuildRepresentation();
  return this->DragActor->GetVisibility() ?
    this->DragActor->RenderOpaqueGeometry(viewport) : 0;
}

#if VTKEdge_VTK_VERSION_DATE > 20070305
//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation2D
::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  return this->DragActor->GetVisibility() ?
    this->DragActor->RenderTranslucentPolygonalGeometry(viewport) : 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation2D::HasTranslucentPolygonalGeometry()
{
  return this->DragActor->GetVisibility() ?
           this->DragActor->HasTranslucentPolygonalGeometry() : 0;
}

#else

//----------------------------------------------------------------------
// Legacy method to support VTK source versions prior to 2007/03/05
int vtkKWEPaintbrushSelectionRepresentation2D::
RenderTranslucentGeometry(vtkViewport *viewport)
{
  return this->DragActor->GetVisibility() ?
          this->DragActor->RenderTranslucentGeometry(viewport) : 0;
}
#endif

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation2D::GetActors( vtkPropCollection * pc )
{
  this->DragActor->GetActors(pc);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation2D
::DeepCopy(vtkWidgetRepresentation *rep)
{
  vtkKWEPaintbrushSelectionRepresentation2D *r
    = vtkKWEPaintbrushSelectionRepresentation2D::SafeDownCast(rep);
  if (this == r || !r)
    {
    return;
    }

  if (vtkImageActorPointPlacer * pthis =
      vtkImageActorPointPlacer::SafeDownCast(this->PointPlacer))
    {
    if (vtkImageActorPointPlacer * pthat =
        vtkImageActorPointPlacer::SafeDownCast(r->PointPlacer))
      {
      pthis->SetImageActor(pthat->GetImageActor());
      }
    }

  this->Superclass::DeepCopy(rep);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation2D::DragAndDropMerge()
{
  int ret = this->Superclass::DragAndDropMerge();
  this->DragBeginPoint[0] = this->DragEndPoint[0] = VTK_DOUBLE_MIN;
  this->DragActor->VisibilityOff();
  return ret;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation2D
::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

