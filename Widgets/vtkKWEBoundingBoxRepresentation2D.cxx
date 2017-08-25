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
#include "vtkKWEBoundingBoxRepresentation2D.h"

#include "vtkSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkInteractorObserver.h"
#include "vtkEvent.h"
#include "vtkSphereHandleRepresentation.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkActor2DCollection.h"
#include "vtkCollection.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkLine.h"

#define min(x,y) ((x<y) ? (x) : (y))
#define max(x,y) ((x>y) ? (x) : (y))
#define VTK_BBOXREP_TOLERANCE 0 // used to be 1e-4. I don't think this is needed.

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEBoundingBoxRepresentation2D, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEBoundingBoxRepresentation2D);

vtkCxxSetObjectMacro(vtkKWEBoundingBoxRepresentation2D,
                     HandleProperty, vtkProperty);
vtkCxxSetObjectMacro(vtkKWEBoundingBoxRepresentation2D,
                     SelectedHandleProperty, vtkProperty);
vtkCxxSetObjectMacro(vtkKWEBoundingBoxRepresentation2D,
                     HoveredHandleProperty, vtkProperty);
vtkCxxSetObjectMacro(vtkKWEBoundingBoxRepresentation2D,
                     ImageData, vtkImageData);

//----------------------------------------------------------------------------
vtkKWEBoundingBoxRepresentation2D::vtkKWEBoundingBoxRepresentation2D()
{
  this->ImageData = NULL;
  this->ShowSliceScaleBar = 1;

  // Create a vtkPolydata for the bounding box.
  this->PolyData          = vtkPolyData::New();
  vtkPoints * points      = vtkPoints::New();
  points->SetNumberOfPoints(8);
  this->PolyData->SetPoints(points);
  for (int i = 0; i < 8 ; i++)
    {
    points->SetPoint( i, 0.0, 0.0, 0.0 );
    }
  vtkCellArray *cellArray = vtkCellArray::New();
  vtkIdType pts[6][4] = { { 1, 0, 2, 3 },
                          { 5, 4, 0, 1 },
                          { 5, 1, 3, 7 },
                          { 0, 4, 6, 2 },
                          { 3, 2, 6, 7 },
                          { 4, 5, 7, 6 } };
  for (int i = 0; i < 6; i++)
    {
    cellArray->InsertNextCell( 4, pts[i] );
    }
  this->PolyData->SetPolys(cellArray);
  cellArray->Delete();
  points->Delete();

  this->LastEventPosition[0] = this->LastEventPosition[1] = 0.0;

  // Construct the poly data representing the box
  this->BoxPolyData       = vtkPolyData::New();
  this->BoxMapper         = vtkPolyDataMapper::New();
  this->BoxActor          = vtkActor::New();
  this->BoxMapper->SetInput(this->BoxPolyData);
  this->BoxMapper->SetResolveCoincidentTopologyToPolygonOffset();
  this->BoxActor->SetMapper(this->BoxMapper);

  this->BoxPoints            = vtkPoints::New(VTK_DOUBLE);
  this->BoxPoints->SetNumberOfPoints(4);
  this->BoxPolyData->SetPoints(this->BoxPoints);

  // Topology of the box
  //     0 ------ 1
  //     |        |
  //     3 ------ 2
  //
  cellArray = vtkCellArray::New();
  cellArray->InsertNextCell(5);
  cellArray->InsertCellPoint(0);
  cellArray->InsertCellPoint(1);
  cellArray->InsertCellPoint(3);
  cellArray->InsertCellPoint(2);
  cellArray->InsertCellPoint(0);
  this->BoxPolyData->SetLines(cellArray);
  cellArray->Delete();

  // Set some default properties.
  // Handle properties
  this->HandleProperty          = vtkProperty::New();
  this->SelectedHandleProperty  = vtkProperty::New();
  this->HoveredHandleProperty   = vtkProperty::New();
  this->HandleProperty        ->SetColor(1.0,1.0,0.7);
  this->SelectedHandleProperty->SetColor(0.9,1.0,0.5);
  this->HoveredHandleProperty ->SetColor(1.0,0.7,0.5);

  // Outline properties (for the hex and the chair)
  this->LineProperty = vtkProperty::New();
  this->LineProperty->SetColor(1.0,0.3,0.1);
  this->LineProperty->SetLineWidth(2.0);
  this->SelectedLineProperty = vtkProperty::New();
  this->SelectedLineProperty->SetColor(0.0,0.0,1.0);
  this->SelectedLineProperty->SetLineWidth(2.0);

  this->BoxActor->SetProperty(this->LineProperty);

  this->CurrentHandleIdx    = -1;
  this->MinimumThickness[0] = 3.0;
  this->MinimumThickness[1] = 3.0;
  this->MinimumThickness[2] = 0.0;
  this->PlaceFactor         = 1.0;
  this->PointPlacer         = vtkImageActorPointPlacer::New();

  // Handle looks like spheres.
  this->HandleRepresentation  = NULL;
  this->HandleRepresentations = NULL;
  vtkSphereHandleRepresentation * hRep = vtkSphereHandleRepresentation::New();
  hRep->SetProperty(this->HandleProperty);
  hRep->SetHandleSize(10.0);
  hRep->SetSelectedProperty(this->SelectedHandleProperty);
  hRep->SetPointPlacer( this->PointPlacer );
  this->SetHandleRepresentation(hRep);
  hRep->Delete();

  // Text annotation for the width and height

  this->TextActors  = vtkActor2DCollection::New();
  this->TextMappers = vtkCollection::New();
  for (int i = 0; i < 2; i++)
    {
    vtkSmartPointer< vtkActor2D > actor
        = vtkSmartPointer< vtkActor2D >::New();
    vtkSmartPointer< vtkTextMapper >mapper = vtkSmartPointer< vtkTextMapper >::New();
    this->TextActors->AddItem( actor );
    this->TextMappers->AddItem( mapper );
    mapper->SetInput("0");
    actor->SetMapper( mapper );
    vtkTextProperty * textProperty = mapper->GetTextProperty();
    textProperty->SetColor(1.0,0.7,0.5);
    textProperty->SetItalic(1);
    }

  this->LabelFormat = new char[6];
  sprintf(this->LabelFormat,"%s","%0.1f");

  // ----------------------------------------
  // A scale bar on the right

  this->ScaleBar  = vtkPolyData::New();
  this->ScaleBar2 = vtkPolyData::New();
  this->ScaleBarActor  = vtkActor::New();
  this->ScaleBarActor2 = vtkActor::New();

  //     ---
  //      |
  //      |
  //      |-
  //      |
  //      |
  //      |
  //     ---
  //
  vtkPoints * scaleBarPoints = vtkPoints::New();
  scaleBarPoints->SetNumberOfPoints(8);
  for (int i = 0; i < 8; i++)
    {
    scaleBarPoints->SetPoint(i, 0.0, 0.0, 0.0);
    }
  this->ScaleBar->SetPoints(scaleBarPoints);
  vtkCellArray *scaleBarCellArray = vtkCellArray::New();
  this->ScaleBar->SetLines( scaleBarCellArray );
  for (int i = 0; i < 4; i++)
    {
    scaleBarCellArray->InsertNextCell(2);
    scaleBarCellArray->InsertCellPoint(2*i);
    scaleBarCellArray->InsertCellPoint(2*i+1);
    }
  scaleBarCellArray->Delete();

  this->ScaleBar2->SetPoints(scaleBarPoints);
  scaleBarCellArray = vtkCellArray::New();
  this->ScaleBar2->SetLines( scaleBarCellArray );
  scaleBarCellArray->InsertNextCell(2);
  scaleBarCellArray->InsertCellPoint(3);
  scaleBarCellArray->InsertCellPoint(4);
  scaleBarCellArray->Delete();
  scaleBarPoints->Delete();

  vtkPolyDataMapper *pdm = vtkPolyDataMapper::New();
  pdm->SetInput(this->ScaleBar);
  pdm->SetResolveCoincidentTopologyToPolygonOffset();
  this->ScaleBarActor->SetMapper( pdm );
  pdm->Delete();

  pdm = vtkPolyDataMapper::New();
  pdm->SetInput(this->ScaleBar2);
  pdm->SetResolveCoincidentTopologyToPolygonOffset();
  this->ScaleBarActor2->SetMapper( pdm );
  pdm->Delete();

  this->ScaleBarActor->GetProperty()->SetColor( 0.7, 1.0, 0.5 );
  this->ScaleBarActor2->GetProperty()->SetColor( 0.7, 1.0, 0.5 );
  this->ScaleBarActor2->GetProperty()->SetLineWidth(2.0);

  for (int i = 4; i < 6; i++)
    {
    vtkSphereHandleRepresentation *rep = vtkSphereHandleRepresentation::New();
    this->HandleRepresentations[i] = rep;
    rep->SetHandleSize(7.0);
    }

  // The slice text actors and mappers.
  for (int i = 0; i < 2; i++)
    {
    vtkSmartPointer< vtkActor2D > actor
        = vtkSmartPointer< vtkActor2D >::New();
    vtkSmartPointer< vtkTextMapper >mapper = vtkSmartPointer< vtkTextMapper >::New();
    this->TextActors->AddItem( actor );
    this->TextMappers->AddItem( mapper );
    mapper->SetInput("0");
    actor->SetMapper( mapper );
    vtkTextProperty * textProperty = mapper->GetTextProperty();
    textProperty->SetColor(0.8, 1.0, 0.5);
    }

  this->Unplaced = 1;
  this->Fade     = 0;
  this->ID       = -1;
  this->InteractionState = Outside;
  this->Point1DisplayPosition[0] = this->Point1DisplayPosition[1] = VTK_INT_MIN;

  // Add an ID Text actor.
  vtkSmartPointer< vtkActor2D > actor
      = vtkSmartPointer< vtkActor2D >::New();
  vtkSmartPointer< vtkTextMapper >mapper =
    vtkSmartPointer< vtkTextMapper >::New();
  this->TextActors->AddItem( actor );
  this->TextMappers->AddItem( mapper );
  actor->SetMapper(mapper);
  actor->VisibilityOff();
  vtkTextProperty * textProperty = mapper->GetTextProperty();
  textProperty->SetColor(1.0,0.7,0.5);
  textProperty->SetItalic(1);
}

//----------------------------------------------------------------------------
vtkKWEBoundingBoxRepresentation2D::~vtkKWEBoundingBoxRepresentation2D()
{
  this->PolyData                  ->Delete();
  this->BoxActor                  ->Delete();
  this->BoxMapper                 ->Delete();
  this->BoxPolyData               ->Delete();
  this->BoxPoints                 ->Delete();
  this->LineProperty              ->Delete();
  this->PointPlacer               ->Delete();
  this->SelectedLineProperty      ->Delete();
  this->TextActors                ->Delete();
  this->TextMappers               ->Delete();
  this->ScaleBar                  ->Delete();
  this->ScaleBarActor             ->Delete();
  this->ScaleBar2                 ->Delete();
  this->ScaleBarActor2            ->Delete();
  this->SetHandleRepresentation   ( NULL );
  this->SetHandleProperty         ( NULL );
  this->SetSelectedHandleProperty ( NULL );
  this->SetHoveredHandleProperty  ( NULL );
  this->SetLabelFormat            ( NULL );
  this->SetImageData              ( NULL );
}

//----------------------------------------------------------------------------
vtkHandleRepresentation* vtkKWEBoundingBoxRepresentation2D
::GetHandleRepresentation( int handleIndex )
{
  return (handleIndex > 5) ? NULL : this->HandleRepresentations[handleIndex];
}

//----------------------------------------------------------------------
// You can swap the handle representation to one that you like.
void vtkKWEBoundingBoxRepresentation2D
::SetHandleRepresentation(vtkHandleRepresentation *handle)
{
  if ( handle == this->HandleRepresentation )
    {
    return;
    }

  vtkSetObjectBodyMacro( HandleRepresentation,
                      vtkHandleRepresentation, handle );

  if (this->HandleRepresentation)
    {
    // Allocate the 4 handles if they haven't been allocated.
    if (!this->HandleRepresentations)
      {
      this->HandleRepresentations = new vtkHandleRepresentation* [6];
      for (int i=0; i<6; i++)
        {
        this->HandleRepresentations[i] = NULL;
        }
      }
    }
  else
    {
    // Free the 4 handles if they haven't been freed.
    if (this->HandleRepresentations)
      {
      for (int i=0; i<6; i++)
        {
        this->HandleRepresentations[i]->Delete();
        }
      delete [] this->HandleRepresentations;
      this->HandleRepresentations = NULL;
      }
    }

  for (int i=0; i<4; i++)
    {

    // We will remove the old handle, in anticipation of the new user-
    // provided handle type that we are going to set a few lines later.
    if (this->HandleRepresentations && this->HandleRepresentations[i])
      {
      this->HandleRepresentations[i]->Delete();
      this->HandleRepresentations[i] = NULL;
      }

    // Copy the new user-provided handle.
    if (this->HandleRepresentation)
      {
      this->HandleRepresentations[i] = this->HandleRepresentation->NewInstance();
      this->HandleRepresentations[i]->ShallowCopy(this->HandleRepresentation);
      this->HandleRepresentations[i]->Highlight(0);
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::SetImageActor( vtkImageActor *a )
{
  if (a != this->PointPlacer->GetImageActor())
    {
    this->PointPlacer->SetImageActor(a);

    // Default placement.
    if (a && this->ImageData)
      {
      this->Unplaced = 1;
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::PlaceWidget()
{
  vtkImageActor * a = this->GetImageActor();
  if (!a || !this->ImageData)
    {
    vtkErrorMacro( <<
      "ImageActor and ImageData must be set prior to placing the widget.");
    }

  this->PointPlacer->SetImageActor(a);
  double bounds[6];
  this->ImageData->GetBounds(bounds);
  for (int i = 0; i < 3; i ++)
    {
    const double c = bounds[2*i+1] - bounds[2*i];
    bounds[2*i]   += c/10.0;
    bounds[2*i+1] -= c/10.0;
    }

  this->PlaceWidget( bounds );
}

//----------------------------------------------------------------------------
vtkImageActor * vtkKWEBoundingBoxRepresentation2D::GetImageActor()
{
  return this->PointPlacer->GetImageActor();
}

//----------------------------------------------------------------------
// This is where the bulk of the work is done.
int vtkKWEBoundingBoxRepresentation2D
::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
  // -----------------------------------------------------------
  double worldOrient[9];

  if ( this->InteractionState == BeginDefining )
    {
    this->Point1DisplayPosition[0] = X;
    this->Point1DisplayPosition[1] = Y;
    this->Point2DisplayPosition[0] = X;
    this->Point2DisplayPosition[1] = Y;
    this->InteractionState = Defining;
    }

  else if ( this->InteractionState == Defining )
    {
    this->Point2DisplayPosition[0] = X;
    this->Point2DisplayPosition[1] = Y;
    }

  if (this->InteractionState == BeginDefining ||
      this->InteractionState == Defining      ||
      this->InteractionState == EndDefining )
    {

    double point1WorldPos[3], point2WorldPos[3], bounds[6];

    if (this->ComputeOrientation() &&
        this->PointPlacer->ComputeWorldPosition( this->Renderer,
          this->Point1DisplayPosition, point1WorldPos, worldOrient ) &&
        this->PointPlacer->ComputeWorldPosition( this->Renderer,
          this->Point2DisplayPosition, point2WorldPos, worldOrient ) )
      {

      this->ImageData->GetBounds(bounds);
      bounds[2*XAxis]   = min(point1WorldPos[XAxis], point2WorldPos[XAxis]);
      bounds[2*XAxis+1] = max(point1WorldPos[XAxis], point2WorldPos[XAxis]);
      bounds[2*YAxis]   = min(point1WorldPos[YAxis], point2WorldPos[YAxis]);
      bounds[2*YAxis+1] = max(point1WorldPos[YAxis], point2WorldPos[YAxis]);
      int success = this->PlaceWidgetInternal(bounds);

      if (this->InteractionState == EndDefining)
        {
        // If we did not successfully placed the widget, go back and start
        // defining it all over again
        this->InteractionState = (success ? Outside : BeginDefining);

        // If we successfully placed the widget, update the Unplaced ivar to
        // reflect that.
        this->Unplaced = (success ? 0 : 1);
        }
      }
    }

  // -----------------------------------------------------------

  else if ( this->InteractionState == RequestResizeBox )
    {
    this->CurrentHandleIdx = -1;

    // We are trying to perform user interaction that might potentially
    // select a handle. Check if we are really near a handle, so it
    // can be selected.

    // Loop over all the handles and check if one of them is selected
    for(int i = 0; i< (this->ShowSliceScaleBar ? 6 : 4); i++)
      {
      this->HandleRepresentations[i]->ComputeInteractionState(X, Y, 0);

       if (this->HandleRepresentations[i]->GetInteractionState() ==
                                 vtkHandleRepresentation::Selecting)
        {
        // The selected handle.
        this->CurrentHandleIdx = i;

        // Highlight the selected handle and unhighlight all others.
        this->SetHandleHighlight(-1, this->HandleProperty);
        this->SetHandleHighlight(
            this->CurrentHandleIdx, this->SelectedHandleProperty);

        this->InteractionState = vtkKWEBoundingBoxRepresentation2D::Resizing;
        break;
        }
      }

    if (this->InteractionState == RequestResizeBox)
      {
      // We aren't near any of the handles, otherwise our state would have
      // changed to Resizing. We are definitely outside.
      this->InteractionState = Outside;
      }
    }


  // (B) -----------------------------------------------------------
  // Handle the resizing operations.

  else if (this->InteractionState == vtkKWEBoundingBoxRepresentation2D::Resizing)
    {
    // Ensure that a handle has been picked.
    if (this->CurrentHandleIdx != -1)
      {
      // Compute world positions corresponding to the current event position
      // (X,Y) and the last event positions such that they lie at the same
      // depth that the handle lies on.

      double handleWorldPos[4], handleTranslation[3],
        handleDisplayPos[4], newHandleWorldPos[3];

      this->HandleRepresentations[this->CurrentHandleIdx]
                                 ->GetWorldPosition(handleWorldPos);
      this->HandleRepresentations[this->CurrentHandleIdx]
                                 ->GetDisplayPosition(handleDisplayPos);

      // The motion vector in display coords
      const double motionVector[3] = { X - this->LastEventPosition[0],
                                       Y - this->LastEventPosition[1],
                                       0.0                            };

      handleDisplayPos[0] += motionVector[0];
      handleDisplayPos[1] += motionVector[1];

      if (this->CurrentHandleIdx < 4)
        {
        if (this->PointPlacer->ComputeWorldPosition( this->Renderer,
            handleDisplayPos, newHandleWorldPos, worldOrient ) )
          {
          handleTranslation[0] = newHandleWorldPos[0] - handleWorldPos[0];
          handleTranslation[1] = newHandleWorldPos[1] - handleWorldPos[1];
          handleTranslation[2] = newHandleWorldPos[2] - handleWorldPos[2];

          // Translate handle and its neighbors.
          this->Translate( handleTranslation, motionVector );
          }
        }
      else
        {
        this->Translate( handleTranslation, motionVector );
        }
      }
    else
      {
      // In theory, we should never get there.
      this->InteractionState = vtkKWEBoundingBoxRepresentation2D::Outside;
      }
    }

  else
    {
    this->InteractionState = vtkKWEBoundingBoxRepresentation2D::Outside;

    // Loop over all the handles and check if we are near one of them.
    for(int i = 0; i< 6; i++)
      {
      this->HandleRepresentations[i]->ComputeInteractionState(X, Y, 0);
      if (this->HandleRepresentations[i]->GetInteractionState() ==
                                    vtkHandleRepresentation::Selecting)
        {
        this->SetHandleHighlight( i, this->HoveredHandleProperty );
        this->InteractionState = vtkKWEBoundingBoxRepresentation2D::Inside;
        break;
        }
      }

    if (this->InteractionState == vtkKWEBoundingBoxRepresentation2D::Outside)
      {
      // Unhighlight all handles and faces.
      this->SetHandleHighlight( -1, this->HandleProperty );
      }
    }

  // Cache the last event position.
  this->LastEventPosition[0] = X;
  this->LastEventPosition[1] = Y;
  return this->InteractionState;
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D
::Translate( const double translation[3], const double cursorMotionVector[3] )
{
  if (!this->ComputeOrientation())
    {
    return;
    }

  if (this->CurrentHandleIdx < 4)
    {
    double handleWorldPos[3], bounds[6];
    const double tolerance = 1e-1;

    this->PolyData->GetBounds( bounds );
    this->HandleRepresentations[
      this->CurrentHandleIdx]->GetWorldPosition(handleWorldPos);

    for (unsigned int k = 0; k < 3; k++)
      {
      if (fabs(handleWorldPos[k] - bounds[2*k]) < tolerance)
        { bounds[2*k] += translation[k]; }
      else if (fabs(handleWorldPos[k] - bounds[2*k+1]) < tolerance)
        { bounds[2*k+1] += translation[k]; }
      }

    this->PlaceWidget(bounds);
    this->Cut();
    this->PositionHandles();
    }

  else
    {
    if (cursorMotionVector[1] == 0.0)
      {
      return;
      }


    double imageSliceBoundsW[2][3], bboxBoundsW[2][3], t;
    this->ScaleBar->GetPoints()->GetPoint(2, imageSliceBoundsW[0]);
    this->ScaleBar->GetPoints()->GetPoint(5, imageSliceBoundsW[1]);
    this->ScaleBar->GetPoints()->GetPoint(3, bboxBoundsW[0]);
    this->ScaleBar->GetPoints()->GetPoint(4, bboxBoundsW[1]);

    double handleDisplayPos[3], worldOrient[9], newHandleWorldPos[3];
    this->HandleRepresentations[this->CurrentHandleIdx]->
                              GetDisplayPosition(handleDisplayPos);
    handleDisplayPos[1] += cursorMotionVector[1];
    this->PointPlacer->ComputeWorldPosition( this->Renderer,
        handleDisplayPos, newHandleWorldPos, worldOrient );

    if (this->CurrentHandleIdx == 4)
      {

      vtkLine::DistanceToLine( newHandleWorldPos,
          imageSliceBoundsW[0], bboxBoundsW[1], t, newHandleWorldPos );
      }
    else
      {
      vtkLine::DistanceToLine( newHandleWorldPos,
          imageSliceBoundsW[1], bboxBoundsW[0], t, newHandleWorldPos );
      }


    // Now find the slice extents.

    t = fabs(imageSliceBoundsW[1][YAxis] - newHandleWorldPos[YAxis]) /
            fabs(imageSliceBoundsW[1][YAxis] - imageSliceBoundsW[0][YAxis]);

    double bounds[6], bbbounds[6];
    this->PolyData->GetBounds( bbbounds );
    this->ImageData->GetBounds(bounds);
    bbbounds[2*ZAxis + this->CurrentHandleIdx-4] =
        t * bounds[2*ZAxis]   + (1-t) * bounds[2*ZAxis+1];

    // Ensure that teh current slice is within the bounds, if not clamp it.
    //
    int extent[6];
    this->GetImageActor()->GetDisplayExtent(extent);

    double displayBounds[2], spacing[3], origin[3];
    this->ImageData->GetSpacing(spacing);
    this->ImageData->GetOrigin(origin);

    displayBounds[0] = extent[2*ZAxis] * spacing[ZAxis] + origin[ZAxis] - VTK_BBOXREP_TOLERANCE;
    displayBounds[1] = extent[2*ZAxis+1] * spacing[ZAxis] + origin[ZAxis] + VTK_BBOXREP_TOLERANCE;
    bbbounds[2*ZAxis] = (bbbounds[2*ZAxis] > displayBounds[0]) ? displayBounds[0] : bbbounds[2*ZAxis];
    bbbounds[2*ZAxis+1] = (bbbounds[2*ZAxis+1] < displayBounds[1]) ? displayBounds[1] : bbbounds[2*ZAxis+1];

    // Now place the widget.. our slice may have changed.
    this->PlaceWidget(bbbounds);
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::GetActors2D(vtkPropCollection *pc)
{
  vtkActor2D* actor;
  vtkCollectionSimpleIterator adit;
  for ( this->TextActors->InitTraversal(adit);
        (actor = this->TextActors->GetNextActor2D(adit));)
    {
    actor->GetActors(pc);
    }
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::GetActors(vtkPropCollection *pc)
{
  for (int i=0; i < (this->ShowSliceScaleBar ? 6 : 4); i++)
    {
    this->HandleRepresentations[i]->GetActors(pc);
    }
  this->BoxActor->GetActors(pc);
  this->ScaleBarActor->GetActors(pc);
}

//----------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::ReleaseGraphicsResources(vtkWindow *w)
{
  this->BoxActor->ReleaseGraphicsResources(w);
  for (int i=0; i< 6; i++)
    {
    this->HandleRepresentations[i]->ReleaseGraphicsResources(w);
    }
  this->ScaleBarActor->ReleaseGraphicsResources(w);
  this->ScaleBarActor2->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::RenderOverlay(vtkViewport *v)
{
  int count = 0;

  // Render if we have been placed or if we are in the process of placing
  if (this->Unplaced == 0 || this->InteractionState == Defining)
    {

    if (this->BoxActor->GetVisibility())
      {
      count += this->BoxActor->RenderOverlay(v);
      }

    for (int i=0; i< (this->ShowSliceScaleBar ? 6 : 4); i++)
      {
      if (this->HandleRepresentations[i]->GetVisibility())
        {
        count+=this->HandleRepresentations[i]->RenderOverlay(v);
        }
      }

    vtkActor2D* actor;
    vtkCollectionSimpleIterator adit;
    for ( this->TextActors->InitTraversal(adit);
          (actor = this->TextActors->GetNextActor2D(adit));)
      {
      // Make sure that the actor is visible before rendering
      if (actor->GetVisibility())
        {
        count += actor->RenderOverlay(v);
        }
      }

    if (this->ShowSliceScaleBar)
      {
      if (this->ScaleBarActor->GetVisibility())
        {
        count+=this->ScaleBarActor->RenderOverlay(v);
        }
      if (this->ScaleBarActor2->GetVisibility())
        {
        count+=this->ScaleBarActor2->RenderOverlay(v);
        }
      }
    }

  return count;
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int count = 0;
  this->BuildRepresentation();

  // Render if we have been placed or if we are in the process of placing
  if (this->Unplaced == 0 || this->InteractionState == Defining)
    {

    if (this->BoxActor->GetVisibility())
      {
      count+=this->BoxActor->RenderOpaqueGeometry(viewport);
      }
    for (int i=0; i< (this->ShowSliceScaleBar ? 6 : 4); i++)
      {
      if (this->HandleRepresentations[i]->GetVisibility())
        {
        count += this->HandleRepresentations[i]->RenderOpaqueGeometry(viewport);
        }
      }

    vtkActor2D* actor;
    vtkCollectionSimpleIterator adit;
    for ( this->TextActors->InitTraversal(adit);
          (actor = this->TextActors->GetNextActor2D(adit));)
      {
      // Make sure that the actor is visible before rendering
      if (actor->GetVisibility())
        {
        count += actor->RenderOpaqueGeometry(viewport);
        }
      }

    if (this->ShowSliceScaleBar)
      {
      if (this->ScaleBarActor->GetVisibility())
        {
        count+=this->ScaleBarActor->RenderOpaqueGeometry(viewport);
        }
      if (this->ScaleBarActor2->GetVisibility())
        {
        count+=this->ScaleBarActor2->RenderOpaqueGeometry(viewport);
        }
      }
    }

  return count;
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::PositionHandles()
{
  bool modified = false;
  double newPos[3], oldPos[3];

  for (int i = 0; i < 4; ++i)
    {
    this->HandleRepresentations[i]->GetWorldPosition(oldPos);
    this->BoxPoints->GetPoint(i, newPos);
    if (oldPos[0] != newPos[0] ||
        oldPos[1] != newPos[1] ||
        oldPos[2] != newPos[2])
      {
      this->HandleRepresentations[i]->SetWorldPosition(newPos);
      modified = true;
      }
    }

  // The position has changed. Force the actor to update the next time.
  if (modified)
    {
    this->BoxPolyData->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::SetHandleHighlight(
                      int handleIdx, vtkProperty *property )
{
  if ( handleIdx == -1)
    {
    // Do for all handles
    for (int i = 0; i < (this->ShowSliceScaleBar ? 6 : 4); i++)
      {
      static_cast< vtkSphereHandleRepresentation * >(
          this->HandleRepresentations[i])->SetProperty(property);
      static_cast< vtkSphereHandleRepresentation * >(
          this->HandleRepresentations[i])->SetSelectedProperty(property);
      }
    }
  else
    {
    static_cast< vtkSphereHandleRepresentation * >(
        this->HandleRepresentations[handleIdx])->SetProperty(property);
    static_cast< vtkSphereHandleRepresentation * >(
        this->HandleRepresentations[handleIdx])->SetSelectedProperty(property);
    }
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::PlaceWidget(double bounds[6])
{
  if (this->PlaceWidgetInternal(bounds))
    {
    // we are no longer defining. We've been placed.
    this->Unplaced = 0;
    }
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::PlaceWidgetInternal(double bounds[6])
{
  double corners[8][3] =
    { { bounds[0], bounds[2], bounds[4] },
      { bounds[1], bounds[2], bounds[4] },
      { bounds[0], bounds[3], bounds[4] },
      { bounds[1], bounds[3], bounds[4] },
      { bounds[0], bounds[2], bounds[5] },
      { bounds[1], bounds[2], bounds[5] },
      { bounds[0], bounds[3], bounds[5] },
      { bounds[1], bounds[3], bounds[5] } };

  vtkImageActor *imageActor = this->GetImageActor();
  if (!imageActor || !this->ImageData)
    {
    vtkErrorMacro( <<
      "ImageActor and ImageData must be set prior to placing the widget.");
    return 0;
    }

  // Scale the corners of parallelopiped according to the place factor.

  double center[3] = {0.0, 0.0, 0.0}, newCorners[8][3];
  for (int j = 0; j < 3; j++)
    {
    for (int i = 0; i < 8; i++)
      {
      center[j] += corners[i][j];
      }
    center[j] /= 8.0;
    for (int i = 0; i < 8; i++)
      {
      newCorners[i][j] = center[j] +
        this->PlaceFactor*(corners[i][j]-center[j]);
      }
    }

  // Make sure that the thickness is greater than minimum thickness.
  double spacing[3];
  this->ImageData->GetSpacing(spacing);

  if (this->InteractionState == Defining ||
      this->InteractionState == BeginDefining ||
      ((sqrt(vtkMath::Distance2BetweenPoints(newCorners[0],
        newCorners[1])) >= this->MinimumThickness[0]*spacing[0]) &&
      (sqrt(vtkMath::Distance2BetweenPoints(newCorners[0],
        newCorners[2])) >= this->MinimumThickness[1]*spacing[1]) &&
      (sqrt(vtkMath::Distance2BetweenPoints(newCorners[0],
        newCorners[4])) >= this->MinimumThickness[2]*spacing[2])) )
    {
    vtkPoints * points = this->PolyData->GetPoints();
    for (int i = 0; i < 8; i++)
      {
      points->SetPoint(i, newCorners[i] );
      }

    this->PolyData->Modified();
    if (this->Cut())
      {
      this->PositionHandles();
      this->AnnotateScaleBar();
      return 1;
      }
    }

  return 0;
}

//----------------------------------------------------------------------------
vtkPolyData * vtkKWEBoundingBoxRepresentation2D::GetPolyData()
{
  return this->PolyData;
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::BuildRepresentation()
{
  // If we are placed or are in the process of placing, render.
  if (this->Unplaced == 0 || this->InteractionState == Defining)
    {
    this->Cut();
    this->Annotate();
    }
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::Cut()
{
  if (!this->ComputeOrientation())
    {
    this->SetVisibilityInternal(0);
    return 0;
    }

  vtkImageActor *imageActor = this->GetImageActor();

  double bounds[6], origin[3], spacing[3];
  int displayExtent[6];

  this->PolyData->GetBounds(bounds);
  this->ImageData->GetOrigin(origin);
  this->ImageData->GetSpacing(spacing);
  imageActor->GetDisplayExtent(displayExtent);

  const double depth = origin[ZAxis] + displayExtent[ZAxis*2]*spacing[ZAxis];

  // cout << "SliceDepth: " << depth <<  " "
  //      << bounds[2*ZAxis] << " " << bounds[2*ZAxis+1] << endl;

  int sliceExtent[2] =
    { vtkMath::Round((bounds[2*ZAxis]-origin[ZAxis])/spacing[ZAxis]),
      vtkMath::Round((bounds[2*ZAxis+1]-origin[ZAxis])/spacing[ZAxis]) };
  double sliceBounds[2] =
    { sliceExtent[0] * spacing[ZAxis] + origin[ZAxis],
      sliceExtent[1] * spacing[ZAxis] + origin[ZAxis] };

  // Check if we should display the box at all. Are we within the bounds ?
  if (depth < sliceBounds[0] || depth > sliceBounds[1])
    {
    this->SetVisibilityInternal(0);
    return 0;
    }

  this->SetVisibilityInternal(1);

  // BoxPoints...
  //   0 ----- 1
  //   |       |
  //   3 ----- 2
  //
  this->BoxPoints->SetPoint(0, bounds[2*XAxis],   bounds[2*YAxis],   depth);
  this->BoxPoints->SetPoint(1, bounds[2*XAxis+1], bounds[2*YAxis],   depth);
  this->BoxPoints->SetPoint(2, bounds[2*XAxis],   bounds[2*YAxis+1], depth);
  this->BoxPoints->SetPoint(3, bounds[2*XAxis+1], bounds[2*YAxis+1], depth);
  this->PositionHandles();

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::ComputeOrientation()
{
  vtkImageActor *imageActor = this->PointPlacer->GetImageActor();
  if (!this->Renderer || !imageActor || !this->ImageData)
    {
    return 0;
    }

  double viewUp[3];
  int displayExtent[6];

  imageActor->GetDisplayExtent(displayExtent);
  if ( displayExtent[0] == displayExtent[1] )
    {
    ZAxis = 0;
    }
  else if ( displayExtent[2] == displayExtent[3] )
    {
    ZAxis = 1;
    }
  else if ( displayExtent[4] == displayExtent[5] )
    {
    ZAxis = 2;
    }
  else
    {
    vtkErrorMacro("Incorrect display extent in Image Actor");
    return 0;
    }

  vtkCamera * cam = this->Renderer->GetActiveCamera();
  cam->GetViewUp(viewUp);

  for (int i = 0; i < 3; i++)
    {
    if (fabs(viewUp[i]) > 0.95)
      {
      YAxis = i;
      continue;
      }
    if (i == ZAxis)
      {
      continue;
      }
    XAxis = i;
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::Annotate()
{
  double p[3][3], dispPos[3][3];
  this->TextActors->InitTraversal();

  for (int i = 0; i < 3; i++)
    {
    this->HandleRepresentations[i]->GetDisplayPosition(dispPos[i]);
    this->BoxPoints->GetPoint(i, p[i]);
    }

  double lengths[2] =
    { sqrt(vtkMath::Distance2BetweenPoints( p[0], p[1] )),
      sqrt(vtkMath::Distance2BetweenPoints( p[0], p[2] )) };

  int textSize[2], stringSize[2], *winSize = this->Renderer->GetSize();
  for (int i = 0; i < 2; i++)
    {
    vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(i));
    char distStr[256];
    sprintf(distStr, this->LabelFormat, lengths[i]);

    mapper->SetInput( distStr );

    vtkTextMapper::SetRelativeFontSize( mapper, this->Renderer, winSize,
                                        stringSize, 0.012f);
    mapper->GetSize(this->Renderer, textSize);

    if (i == 0)
      {
      this->TextActors->GetNextActor2D()->SetPosition(
          (dispPos[0][0] + dispPos[1][0])/2.0 - textSize[0]/1.8,
          (dispPos[0][1] + dispPos[1][1])/2.0 + textSize[1] * 0.5 );
      }
    else
      {
      this->TextActors->GetNextActor2D()->SetPosition(
          (dispPos[0][0] + dispPos[2][0])/2.0 - textSize[0] * 1.1,
          (dispPos[0][1] + dispPos[2][1])/2.0 - textSize[1]/1.8 );
      }
    }

  // Annotate the scale bar as well.
  this->AnnotateScaleBar();

  // Annotate the ID field.
  vtkActor2D * tac = static_cast< vtkActor2D * >(
          this->TextActors->GetItemAsObject(4));
  if (this->ID == -1)
    {
    tac->SetVisibility(0);
    }
  else
    {
    tac->SetVisibility(this->BoxActor->GetVisibility());
    vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(4));
    char distStr[10];
    sprintf(distStr, "%d:", this->ID);
    mapper->SetInput( distStr );
    vtkTextMapper::SetRelativeFontSize( mapper, this->Renderer, winSize,
                                        stringSize, 0.012f);
    mapper->GetSize(this->Renderer, textSize);
    tac->SetPosition(  dispPos[0][0] - textSize[0] * 1.5,
                       dispPos[0][1] - textSize[1]/1.8 );
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::AnnotateScaleBar()
{
  vtkImageActor *imageActor = this->PointPlacer->GetImageActor();
  if (!this->Renderer || !imageActor ||
      !this->ShowSliceScaleBar || !this->ImageData)
    {
    return 0;
    }

  double bounds[6], handleWorldPos[4][3];

  for (int i = 0; i < 4; i++)
    {
    this->BoxPoints->GetPoint(i, handleWorldPos[i]);
    }

  // Width of actor is a tenth of the width of the bbox.
  const double width = sqrt(
      vtkMath::Distance2BetweenPoints(handleWorldPos[0], handleWorldPos[1]))/10.0;

  vtkPoints *scaleBarPoints = this->ScaleBar->GetPoints();
  double offsetVector[3] = { handleWorldPos[1][0] - handleWorldPos[0][0],
                             handleWorldPos[1][1] - handleWorldPos[0][1],
                             handleWorldPos[1][2] - handleWorldPos[0][2] };
  vtkMath::Normalize( offsetVector );

  scaleBarPoints->SetPoint( 0, handleWorldPos[3][0] + 2 * width * offsetVector[0],
                               handleWorldPos[3][1] + 2 * width * offsetVector[1],
                               handleWorldPos[3][2] + 2 * width * offsetVector[2] );
  scaleBarPoints->SetPoint( 1, handleWorldPos[3][0] + 3 * width * offsetVector[0],
                               handleWorldPos[3][1] + 3 * width * offsetVector[1],
                               handleWorldPos[3][2] + 3 * width * offsetVector[2] );
  scaleBarPoints->SetPoint( 2, handleWorldPos[3][0] + 2.5 * width * offsetVector[0],
                               handleWorldPos[3][1] + 2.5 * width * offsetVector[1],
                               handleWorldPos[3][2] + 2.5 * width * offsetVector[2]);
  scaleBarPoints->SetPoint( 5, handleWorldPos[1][0] + 2.5 * width * offsetVector[0],
                               handleWorldPos[1][1] + 2.5 * width * offsetVector[1],
                               handleWorldPos[1][2] + 2.5 * width * offsetVector[2]);
  scaleBarPoints->SetPoint( 6, handleWorldPos[1][0] + 2 * width * offsetVector[0],
                               handleWorldPos[1][1] + 2 * width * offsetVector[1],
                               handleWorldPos[1][2] + 2 * width * offsetVector[2] );
  scaleBarPoints->SetPoint( 7, handleWorldPos[1][0] + 3 * width * offsetVector[0],
                               handleWorldPos[1][1] + 3 * width * offsetVector[1],
                               handleWorldPos[1][2] + 3 * width * offsetVector[2]);

  double bbbounds[6], endPoints[2][3], pp[2][3];

  this->PolyData->GetBounds(bbbounds);
  this->ImageData->GetBounds(bounds);
  scaleBarPoints->GetPoint(2, endPoints[0]);
  scaleBarPoints->GetPoint(5, endPoints[1]);
  for (int i = 0; i < 2; i++)
    {
    const double fraction = fabs(bbbounds[2*ZAxis+i] - bounds[2*ZAxis])
               / (bounds[2*ZAxis+1] - bounds[2*ZAxis]);
    for (int j = 0; j < 3; j++)
      {
      pp[i][j] = fraction * endPoints[1][j] + (1.0-fraction) * endPoints[0][j];
      }
    }

  scaleBarPoints->SetPoint( 3, pp[0] );
  scaleBarPoints->SetPoint( 4, pp[1] );
  this->HandleRepresentations[4]->SetWorldPosition(pp[0]);
  this->HandleRepresentations[5]->SetWorldPosition(pp[1]);

  scaleBarPoints->Modified();


  // Annotate slice-scale text.
  int extent[6];
  this->GetExtent(extent);

  for (int i = 2; i < 4; i++)
    {
    vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(i));
    char distStr[100];
    sprintf(distStr, "%d", extent[2*ZAxis+(i-2)]+1);
    mapper->SetInput( distStr );

    int textSize[2], stringSize[2], *winSize = this->Renderer->GetSize();
    vtkTextMapper::SetRelativeFontSize( mapper, this->Renderer, winSize,
                                        stringSize, 0.012f);
    mapper->GetSize(this->Renderer, textSize);

    double dispPos[3];
    this->HandleRepresentations[i+2]->GetDisplayPosition(dispPos);
    static_cast< vtkActor2D * >(this->TextActors->GetItemAsObject(i))
      ->SetPosition( dispPos[0] + textSize[0]/1.8, dispPos[1] );
    }

  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::SetExtent( int extents[6] )
{
  if (!this->ComputeOrientation())
    {
    vtkErrorMacro( << "Check if the renderer, imageActor have been set and "
        << "that the image actor has an input.")
    return;
    }

  double bounds[6], origin[3], spacing[3];
  this->PolyData->GetBounds(bounds);
  this->ImageData->GetOrigin(origin);
  this->ImageData->GetSpacing(spacing);
  for (int i = 0; i < 3; i++)
    {
    bounds[2*i]   = extents[2*i] * spacing[i] + origin[i];
    bounds[2*i+1] = extents[2*i+1] * spacing[i] + origin[i];
    }

  this->PlaceWidget(bounds);
}

//----------------------------------------------------------------------------
int vtkKWEBoundingBoxRepresentation2D::GetExtent( int extents[6] )
{
  extents[0] = extents[2] = extents[4] = 0;
  extents[1] = extents[3] = extents[5] = -1;

  // Sanity check first, before we proceed to compute the extents.
  if (!this->Renderer ||
      !this->ImageData ||
      this->InteractionState == BeginDefining ||
      (this->InteractionState == EndDefining && this->Unplaced == 1))
    {
    return 0;
    }

  // The extents are easily computed from the bounds of the cuboid that
  // we use to represent out Bounding box, the iVar "PolyData"

  double bounds[6], origin[3], spacing[3];
  this->PolyData->GetBounds(bounds);
  this->ImageData->GetOrigin(origin);
  this->ImageData->GetSpacing(spacing);

  for (int i = 0; i < 3; i++)
    {
    extents[2*i] = vtkMath::Round((bounds[2*i]-origin[i])/spacing[i]);
    extents[2*i+1] = vtkMath::Round((bounds[2*i+1]-origin[i])/spacing[i]);
    if (extents[2*i] > extents[2*i+1])
      {
      extents[0] = extents[2] = extents[4] = 0;
      extents[1] = extents[3] = extents[5] = -1;
      return 0;
      }
    }

  // Sanity check.
  if (extents[0] == extents[1] &&
      extents[2] == extents[3] &&
      extents[4] == extents[5])
    {
    return 0;
    }

  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::SetVisibilityInternal( int vis )
{
  // for everything but the bbox.
  int v = ((vis == 1 && this->Fade == 0) ? 1 : 0);

  for (int i=0; i< 4; i++)
    {
    this->HandleRepresentations[i]->SetVisibility(
        this->InteractionState != Defining ? v : 0);
    }
  for (int i=4; i< 6; i++)
    {
    this->HandleRepresentations[i]->SetVisibility(
      (this->ShowSliceScaleBar && this->InteractionState != Defining) ? v : 0);
    }
  this->BoxActor->SetVisibility(vis);
  this->ScaleBarActor->SetVisibility(
    (this->ShowSliceScaleBar && this->InteractionState != Defining) ? v : 0);
  this->ScaleBarActor2->SetVisibility(
    (this->ShowSliceScaleBar && this->InteractionState != Defining) ? v : 0);
  this->TextActors->InitTraversal();

  // size annotation
  this->TextActors->GetNextActor2D()->SetVisibility(
      this->InteractionState != Defining ? vis : 0);
  this->TextActors->GetNextActor2D()->SetVisibility(
      this->InteractionState != Defining ? vis : 0);

  // Slice annotation.
  this->TextActors->GetNextActor2D()->SetVisibility(
    (this->ShowSliceScaleBar && this->InteractionState != Defining) ? v : 0);
  this->TextActors->GetNextActor2D()->SetVisibility(
    (this->ShowSliceScaleBar && this->InteractionState != Defining) ? v : 0);
}

//----------------------------------------------------------------------------
vtkProperty * vtkKWEBoundingBoxRepresentation2D::GetOutlineProperty()
{
  return this->BoxActor->GetProperty();
}

//----------------------------------------------------------------------------
vtkTextProperty * vtkKWEBoundingBoxRepresentation2D::GetWidthTextProperty()
{
  vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(0));
  return mapper->GetTextProperty();
}

//----------------------------------------------------------------------------
vtkTextProperty * vtkKWEBoundingBoxRepresentation2D::GetHeightTextProperty()
{
  vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(1));
  return mapper->GetTextProperty();
}

//----------------------------------------------------------------------------
vtkTextProperty * vtkKWEBoundingBoxRepresentation2D::GetIDTextProperty()
{
  vtkTextMapper * mapper = static_cast< vtkTextMapper * >(
          this->TextMappers->GetItemAsObject(4));
  return mapper->GetTextProperty();
}

//----------------------------------------------------------------------------
void vtkKWEBoundingBoxRepresentation2D::PrintSelf(ostream& os, vtkIndent indent)
{
  double p[3];
  os << indent << "BoundingBox points: " << this->PolyData->GetPoints() << endl;
  for (int i = 0; i < 8; i++)
    {
    this->PolyData->GetPoints()->GetPoint(i,p);
    os << indent.GetNextIndent() << "Point " << i << " : ("
       << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;
    }
  os << indent << "BoxPoints (cut of the bounding box):" << this->BoxPoints << endl;
  for (int i = 0; i < 4; i++)
    {
    this->BoxPoints->GetPoint(i,p);
    os << indent.GetNextIndent() << "Point " << i << " : ("
       << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;
    }
  os << indent << "HandlePositions (coincides with BoxPoints):" << endl;
  for (int i = 0; i < 4; i++)
    {
    this->HandleRepresentations[i]->GetWorldPosition(p);
    os << indent.GetNextIndent() << "Point " << i << " : ("
       << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;
    }
  os << indent << "ScaleBar points (where the scale bar is displayed): "
     << this->ScaleBar->GetPoints() << endl;
  for (int i = 0; i < 8; i++)
    {
    this->ScaleBar->GetPoints()->GetPoint(i,p);
    os << indent.GetNextIndent() << "Point " << i << " : ("
       << p[0] << ", " << p[1] << ", " << p[2] << ")" << endl;
    }

  this->Superclass::PrintSelf(os,indent);
}

