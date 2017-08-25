//=============================================================================
//   This file is part of VTKEdge. See vtkedge.org for more information.
//
//   Copyright (c) 2008 Kitware, Inc.
//
//   VTKEdge may be used under the terms of the GNU General Public License 
//   version 3 as published by the Free Software Foundation and appearing in 
//   the file LICENSE.txt included in the top level directory of this source
//   code distribution. Alternatively you may (at your option) use any later 
//   version of the GNU General Public License if such license has been 
//   publicly approved by Kitware, Inc. (or its successors, if any).
//
//   VTKEdge is distributed "AS IS" with NO WARRANTY OF ANY KIND, INCLUDING
//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
//   PURPOSE. See LICENSE.txt for additional details.
//
//   VTKEdge is available under alternative license terms. Please visit
//   vtkedge.org or contact us at kitware@kitware.com for further information.
//
//=============================================================================
#include "vtkKWEPaintbrushRepresentation.h"

#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushAnnotationRepresentation.h"
#include "vtkKWEPaintbrushSelectionRepresentation.h"
#include "vtkCoordinate.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkInteractorObserver.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkPointPlacer.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushRepresentation, "$Revision: 590 $");
vtkCxxSetObjectMacro(vtkKWEPaintbrushRepresentation, ShapePlacer, vtkPointPlacer);

//----------------------------------------------------------------------
vtkKWEPaintbrushRepresentation::vtkKWEPaintbrushRepresentation()
{
  this->PaintbrushDrawing   = vtkKWEPaintbrushDrawing::New();
  this->PaintbrushOperation = vtkKWEPaintbrushOperation::New();  
  this->PaintbrushDrawing->SetPaintbrushOperation(this->PaintbrushOperation);
  this->InteractionState    = PaintbrushDisabled;
  this->ShapePlacer         = NULL;
  this->SketchIndex         = 0;
  this->Annotation          = vtkKWEPaintbrushAnnotationRepresentation::New();
  this->PipelineInstalled   = 0;
  this->SelectionRepresentation = NULL;
  this->Annotation->PaintbrushRepresentation = this;
  this->LastDisplayPosition[0] = this->LastDisplayPosition[1] = 0;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushRepresentation::~vtkKWEPaintbrushRepresentation()
{
  this->PaintbrushDrawing->Delete();
  this->SetPaintbrushOperation(NULL);
  this->SetShapePlacer(NULL);
  this->Annotation->Delete();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::SetPaintbrushOperation( 
                         vtkKWEPaintbrushOperation * filter )
{
  if (this->PaintbrushOperation != filter)
    {
    vtkKWEPaintbrushOperation * var = this->PaintbrushOperation;                     
    this->PaintbrushOperation = filter;
    if (this->PaintbrushOperation) 
      { 
      this->PaintbrushOperation->Register(this); 
      }
    if (var != NULL)
      {                                                    
      var->UnRegister(this);                    
      }                                                    
    this->Modified();                                      
    }

  if (this->PaintbrushOperation)
    {
    this->PaintbrushDrawing->SetPaintbrushOperation( 
        this->PaintbrushOperation );
    }
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::BeginNewSketch()
{
  if (this->GetSketchIndex() >= 0)
    {
    this->PaintbrushDrawing->AddItem();
    }

  this->SketchIndex = this->PaintbrushDrawing->GetNumberOfItems()-1;
  return 1;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::IncrementSketch()
{
  return this->GoToSketch(this->GetSketchIndex() + 1);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::DecrementSketch()
{
  return this->GoToSketch(this->GetSketchIndex() - 1);
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::GoToSketch( int n )
{
  if (n < this->PaintbrushDrawing->GetNumberOfItems() && n >= 0)
    {
    this->SketchIndex = n;
    return 1;
    }

  return 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::BeginNewStroke()
{
  if (vtkKWEPaintbrushSketch * paintbrushSketch 
        = this->PaintbrushDrawing->GetItem(this->GetSketchIndex()))
    {
    
    // Give the representation a chance to save some memory. Check if the 
    // stroke cannot go beyond these extents.
    int extents[6];
    this->GetEtchExtents( extents );

    if (this->InteractionState == PaintbrushDraw)
      { 
      return paintbrushSketch->AddNewStroke(
          vtkKWEPaintbrushEnums::Draw, NULL, extents);
      }
    if (this->InteractionState == PaintbrushErase)
      { 
      return paintbrushSketch->AddNewStroke(
          vtkKWEPaintbrushEnums::Erase, NULL, extents);
      }
    }
  return 0;
}
  
//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::UndoStroke()
{
  if (vtkKWEPaintbrushSketch * paintbrushSketch 
        = this->PaintbrushDrawing->GetItem(this->GetSketchIndex()))
    {
    return paintbrushSketch->PopStroke(); 
    }
  return 0;
}
  
//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::RedoStroke()
{
  if (vtkKWEPaintbrushSketch * paintbrushSketch 
        = this->PaintbrushDrawing->GetItem(this->GetSketchIndex()))
    {
    return paintbrushSketch->PushStroke(); 
    }
  return 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::RemoveLastStroke()
{
  if (vtkKWEPaintbrushSketch * paintbrushSketch 
        = this->PaintbrushDrawing->GetItem(this->GetSketchIndex()))
    {
    return paintbrushSketch->DeleteLastStroke(); 
    }
  return 0;
}
 
//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::DeepCopy(vtkWidgetRepresentation *rep)
{
  vtkKWEPaintbrushRepresentation *r 
    = vtkKWEPaintbrushRepresentation::SafeDownCast(rep);
  if (this == r || !r)
    {
    return;
    }

  //if (this->ShapePlacer && r->ShapePlacer)
  //  {
  //  this->ShapePlacer->DeepCopy(r->ShapePlacer);
  //  }

  this->CurrentShapePosition[0] = r->CurrentShapePosition[0];
  this->CurrentShapePosition[1] = r->CurrentShapePosition[1];
  this->CurrentShapePosition[2] = r->CurrentShapePosition[2];
  this->InteractionState = r->InteractionState;
  this->SketchIndex = r->GetSketchIndex();
  this->SetPaintbrushDrawing( r->GetPaintbrushDrawing() );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::SetPaintbrushDrawing( 
                       vtkKWEPaintbrushDrawing * drawing )
{
  if (this->PaintbrushDrawing != drawing)
    {
    vtkKWEPaintbrushDrawing * var = this->PaintbrushDrawing;                     
    this->PaintbrushDrawing = drawing;
    if (this->PaintbrushDrawing != NULL) 
      { 
      this->PaintbrushDrawing->Register(this); 
      }
    if (var != NULL)
      {                                                    
      var->UnRegister(this);                    
      }                                                    

    if (this->SelectionRepresentation)
      {
      this->SelectionRepresentation->SetPaintbrushDrawing( drawing );
      }    
    this->Modified();                                      
    }
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::GetSketchIndex()
{
  int upperBound = this->PaintbrushDrawing->GetNumberOfItems()-1;
  if (upperBound < 0)
    {
    upperBound = 0;
    }
  this->SketchIndex = (this->SketchIndex < 0 ? 0 :
    (this->SketchIndex > upperBound ? upperBound : this->SketchIndex));
  return this->SketchIndex;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::InstallPipeline()
{
  this->PipelineInstalled = 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::UnInstallPipeline()
{
  this->PipelineInstalled = 0;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentation::IsInsideCanvas(int displaypos[2])
{
  double displayPos[2], worldPos[3], worldOrient[9];
  displayPos[0] = displaypos[0];   
  displayPos[1] = displaypos[1];

  if ( !this->ShapePlacer->ComputeWorldPosition( this->Renderer,
                                                 displayPos, worldPos,
                                                 worldOrient) )
    {
    return 0; // not a valid display location to paint on.
    }
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::GetEtchExtents( int extents[6] )
{
  extents[0] = extents[2] = extents[4] = 0;
  extents[1] = extents[3] = extents[5] = -1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

