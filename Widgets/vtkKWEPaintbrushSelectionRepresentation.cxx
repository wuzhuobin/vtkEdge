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
#include "vtkKWEPaintbrushSelectionRepresentation.h"

#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkCoordinate.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkInteractorObserver.h"
#include "vtkPointPlacer.h"
#include "vtkProperty.h"
#include <algorithm>

vtkCxxRevisionMacro(vtkKWEPaintbrushSelectionRepresentation, "$Revision: 1774 $");
vtkCxxSetObjectMacro(vtkKWEPaintbrushSelectionRepresentation, PointPlacer, vtkPointPlacer);
vtkCxxSetObjectMacro(vtkKWEPaintbrushSelectionRepresentation, PaintbrushDrawing, vtkKWEPaintbrushDrawing);

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionRepresentation::vtkKWEPaintbrushSelectionRepresentation()
{
  this->PaintbrushDrawing   = NULL;
  this->PointPlacer         = NULL;
  this->SelectedSketch      = NULL;
  this->InteractionState    = None;
  this->DragBeginPoint[0]   =
  this->DragBeginPoint[1]   =
  this->DragBeginPoint[2]   = VTK_DOUBLE_MIN;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushSelectionRepresentation::~vtkKWEPaintbrushSelectionRepresentation()
{
  this->SetPaintbrushDrawing(NULL);
  this->SetPointPlacer(NULL);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation
::ComputeInteractionState( int X, int Y, int vtkNotUsed(modifier) )
{
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();
  if (this->InteractionState == PaintbrushRequestSketchSelect)
    {

    // First check if the point placer validates us.
    double displayPos[2] = {X, Y}, worldPos[3], worldOrient[9] = {1.0,0.0,0.0,
                                                                  0.0,1.0,0.0,
                                                                  0.0,0.0,1.0};
    if ( this->PointPlacer->ComputeWorldPosition( this->Renderer,
             displayPos, worldPos, worldOrient ) )
      {

      // Traverse the list of selected sketches and see if we are trying
      // to unselect any of them.

      for (SelectedSketchesType::reverse_iterator rit =
              this->SelectedSketches.rbegin();
           rit != this->SelectedSketches.rend(); ++rit)
        {
        if (this->WorldPositionIsInside( *rit, worldPos ))
          {
          // We clicked on a selected sketch. We should toggle its selection;
          // Unselect it.

          this->InteractionState =
            this->PrevInteractionState = PaintbrushSketchUnselect;
          this->SelectedSketch = *rit; // Its really an unselected sketch.
          this->RemoveSketchFromSelection(*rit);
          return this->InteractionState;
          }
        }

      // Now traverse the list of un-selected sketches in the drawing and see
      // if we are trying to select any of them.

      for (int i = 0; i < nSketches; i++)
        {
        vtkKWEPaintbrushSketch *s = this->PaintbrushDrawing->GetItem(i);

        // Skip any sketches that are already selected for efficiency. We
        // know that we aren't over an already selected sketch; if we were,
        // we'd have unselected it in above.

        if (vtkstd::find(this->SelectedSketches.begin(),
             this->SelectedSketches.end(), s) != this->SelectedSketches.end())
          {
          continue;
          }

        if (this->WorldPositionIsInside( s, worldPos ))
          {
          // Select this sketch.

          this->InteractionState =
            this->PrevInteractionState = PaintbrushSketchSelect;
          this->SelectedSketch = s;
          this->AddSketchToSelection(s);
          return this->InteractionState;
          }
        }
      }
    }

  // Drag and drop merge support...

  else if (this->InteractionState == PaintbrushRequestSketchMerge
        && nSketches > 1)
    {
    double displayPos[2]={X, Y}, worldOrient[9] = {1.0,0.0,0.0,
                                                   0.0,1.0,0.0,
                                                   0.0,0.0,1.0};
    if (this->PrevInteractionState == PaintbrushSketchSelect &&
        this->SelectedSketch )
      {
      this->DragEndPoint[0] =
      this->DragEndPoint[1] =
      this->DragEndPoint[2] = VTK_DOUBLE_MIN;
      if ( !this->PointPlacer->ComputeWorldPosition( this->Renderer,
             displayPos, this->DragBeginPoint, worldOrient ) )
        {
        this->DragBeginPoint[0] =
        this->DragBeginPoint[1] =
        this->DragBeginPoint[2] = VTK_DOUBLE_MIN;
        }
      this->PrevInteractionState = PaintbrushRequestSketchMerge;
      }

    if ( !this->PointPlacer->ComputeWorldPosition( this->Renderer,
           displayPos, this->DragEndPoint, worldOrient ) )
      {
      this->DragEndPoint[0] =
      this->DragEndPoint[1] =
      this->DragEndPoint[2] = VTK_DOUBLE_MIN;
      }
    else
      {
      this->DragAndDropDestination = NULL;
      for (int i = nSketches-1; i >= 0 ; --i)
        {
        vtkKWEPaintbrushSketch *s = this->PaintbrushDrawing->GetItem(i);
        if (this->WorldPositionIsInside( s, this->DragEndPoint))
          {
          this->DragAndDropDestination = s;
          this->InteractionState = PaintbrushSketchMerge;
          break;
          }
        }
      }

    return this->InteractionState;
    }


  // Clearly, if we got here, we haven't managed to toggle the selected
  // state of any sketch.

  this->InteractionState = None;
  this->SelectedSketch = NULL;
  return this->InteractionState;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation
::AddSketchToSelection( vtkKWEPaintbrushSketch * s )
{
  if (vtkstd::find(this->SelectedSketches.begin(),
       this->SelectedSketches.end(), s) == this->SelectedSketches.end())
    {
    this->SelectedSketches.push_back(s);
    s->GetPaintbrushProperty()->HighlightOn();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation::SelectAllSketches()
{
  this->SelectedSketches.clear();
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();
  for (int n = 0; n < nSketches; n++)
    {
    vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
    this->SelectedSketches.push_back(s);
    s->GetPaintbrushProperty()->HighlightOn();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation::UnSelectAllSketches()
{
  // Iterate over all the sketches and un-highlight them.

  for (SelectedSketchesType::iterator it = this->SelectedSketches.begin();
       it != this->SelectedSketches.end(); ++it)
    {
    if (this->PaintbrushDrawing->IsItemPresent(*it))
      {
      (*it)->GetPaintbrushProperty()->HighlightOff();
      }
    }
  this->SelectedSketches.clear();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation
::RemoveSketchFromSelection( vtkKWEPaintbrushSketch * s )
{
  SelectedSketchesType::iterator it = vtkstd::find(
      this->SelectedSketches.begin(), this->SelectedSketches.end(), s);
  if (it != this->SelectedSketches.end())
    {
    s->GetPaintbrushProperty()->HighlightOff();
    this->SelectedSketches.erase(it);
    }
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation
::WorldPositionIsInside( vtkKWEPaintbrushSketch *s, double worldPos[3] )
{
  return s->GetPaintbrushData()->IsInside(worldPos);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation
::DisplayPositionIsInside( vtkKWEPaintbrushSketch *s, double dispPos[2] )
{
  // First check if the point placer validates us.
  double worldPos[3], worldOrient[9] = {1.0,0.0,0.0,
                                        0.0,1.0,0.0,
                                        0.0,0.0,1.0};
  if ( this->PointPlacer->ComputeWorldPosition( this->Renderer,
           dispPos, worldPos, worldOrient ) )
    {
    return this->WorldPositionIsInside( s, worldPos );
    }

  return 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation::DeleteSelectedSketches()
{
  int nSketchesRemoved = 0;
  for (SelectedSketchesType::iterator it = this->SelectedSketches.begin();
         it != this->SelectedSketches.end(); ++it)
    {
    nSketchesRemoved += this->PaintbrushDrawing->RemoveItem( *it );
    }

  this->SelectedSketch = NULL;
  this->SelectedSketches.clear();

  // Don't let the drawing lie without any sketches at all. Keep one there,
  // so that we can draw something.
  return nSketchesRemoved;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation
::MergeSelectedSketches( vtkKWEPaintbrushSketch *mergedSketch,
                         int removeSketches )
{
  if (this->SelectedSketches.size() < 2)
    {
    return 0; // We need at least two to merge.
    }

  // Assert that mergedSketch, if supplied, is a sketch from the drawing.
  if (mergedSketch && this->PaintbrushDrawing->IsItemPresent(mergedSketch) == 0)
    {
    vtkErrorMacro( "The sketch youre trying to merge into must be a sketch"
      << " from the drawing" );
    return 0;
    }

  SelectedSketchesType::iterator it = this->SelectedSketches.begin();
  vtkKWEPaintbrushSketch *sketchToMergeInto = mergedSketch ? mergedSketch : (*it);

  // Now merge all the selected sketches into sketchToMergeInto
  for (; it != this->SelectedSketches.end(); ++it)
    {
    // We don't want to merge the sketchToMergeInto with sketchToMergeInto
    if (*it != sketchToMergeInto)
      {
      // Sanity check to make sure that the sketch is present in the drawing
      if (this->PaintbrushDrawing->IsItemPresent(*it))
        {
        sketchToMergeInto->GetPaintbrushData()->Add( (*it)->GetPaintbrushData() );
        if (removeSketches)
          {
          (*it)->GetPaintbrushProperty()->HighlightOff();
          this->PaintbrushDrawing->RemoveItem( *it );
          }
        }
      }
    }

  if (removeSketches)
    {
    // If the sketch being merged into was a part of the selection, leave it as
    // selected and remove the rest.
    if (vtkstd::find(this->SelectedSketches.begin(), this->SelectedSketches.end(),
          sketchToMergeInto) != this->SelectedSketches.end())
      {
      this->SelectedSketch = sketchToMergeInto;
      this->SelectedSketches.clear();
      this->SelectedSketches.push_back( sketchToMergeInto );
      }
    else
      {
      this->SelectedSketch = NULL;
      this->SelectedSketches.clear();
      }
    }

  // Don't let the drawing lie without any sketches at all. Keep one there,
  // so that we can draw something.
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation::DeepCopy(vtkWidgetRepresentation *rep)
{
  vtkKWEPaintbrushSelectionRepresentation *r
    = vtkKWEPaintbrushSelectionRepresentation::SafeDownCast(rep);
  if (this == r || !r)
    {
    return;
    }

  //if (this->PointPlacer && r->PointPlacer)
  //  {
  //  this->PointPlacer->DeepCopy(r->PointPlacer);
  //  }

  this->InteractionState = r->InteractionState;
  this->SelectedSketch   = r->SelectedSketch;
  this->SelectedSketches = r->SelectedSketches;
  this->SetPaintbrushDrawing( r->GetPaintbrushDrawing() );
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushSelectionRepresentation::DragAndDropMerge()
{
  // If both the drag and the drop sketches aren't null..
  if (this->SelectedSketch && this->DragAndDropDestination &&
      this->PaintbrushDrawing->IsItemPresent(this->SelectedSketch) &&
      this->PaintbrushDrawing->IsItemPresent(this->DragAndDropDestination))
    {
    this->DragAndDropDestination->GetPaintbrushData()->Add(
                this->SelectedSketch->GetPaintbrushData() );
    this->RemoveSketchFromSelection(this->SelectedSketch);
    this->PaintbrushDrawing->RemoveItem(this->SelectedSketch);
    this->SelectedSketch = NULL;
    return 1;
    }
  return 0;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushSelectionRepresentation
::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

