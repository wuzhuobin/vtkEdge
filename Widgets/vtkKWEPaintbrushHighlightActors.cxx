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
#include "vtkKWEPaintbrushHighlightActors.h"

#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEStencilContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkActorCollection.h"
#include "vtkMapperCollection.h"
#include "vtkImageStencilData.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushHighlightActors, "$Revision: 590 $");
vtkCxxSetObjectMacro(vtkKWEPaintbrushHighlightActors, PaintbrushDrawing, vtkKWEPaintbrushDrawing);

//----------------------------------------------------------------------------
// We've hidden the New() method from users by making it protected. So we've
// got to provide the implementation ourself instead of using 
// vtkStandardNewMacro.
vtkKWEPaintbrushHighlightActors* vtkKWEPaintbrushHighlightActors::New()
{
  if (vtkObject* ret = vtkObjectFactory::CreateInstance("vtkKWEPaintbrushHighlightActors"))
    {
    return static_cast<vtkKWEPaintbrushHighlightActors*>(ret);
    }
  return new vtkKWEPaintbrushHighlightActors;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushHighlightActors::vtkKWEPaintbrushHighlightActors()
{
  this->ContourFilters         = vtkCollection::New();
  this->ContourPolyDataMappers = vtkMapperCollection::New();
  this->ContourPolyDataActors  = vtkActorCollection::New();
  this->PaintbrushDrawing      = NULL;
  this->Extent[0] = this->Extent[2] = this->Extent[4] = 0;
  this->Extent[1] = this->Extent[3] = this->Extent[5] = -1;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushHighlightActors::~vtkKWEPaintbrushHighlightActors()
{
  this->ContourFilters->Delete();
  this->ContourPolyDataMappers->Delete();
  this->ContourPolyDataActors->Delete();
  this->SetPaintbrushDrawing(NULL);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushHighlightActors::BuildRepresentation()
{
  // The pipeline is as follows :
  //
  //  SketchImage_0 --> StencilContourFilter_0 --> PolyDataMapper_0 --> Actor_0
  //
  //  SketchImage_1 --> StencilContourFilter_1 --> PolyDataMapper_1 --> Actor_1
  // ....
  // ....

  vtkKWEStencilContourFilter * cf;
  vtkCollectionSimpleIterator ait;
  
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();

  // Create new actors for newly highlighted sketches.

  for (int i = 0; i < nSketches; i++)
    {
    vtkKWEPaintbrushSketch * sketch = this->PaintbrushDrawing->GetItem(i);
    
    // If the sketch isn't highlighted, don't even bother.
    if (sketch->GetPaintbrushProperty()->GetHighlight() == 0)
      {
      continue;
      }

    if (vtkKWEPaintbrushStencilData *mask = vtkKWEPaintbrushStencilData::
                          SafeDownCast(sketch->GetPaintbrushData()))
      {
      vtkImageStencilData * stencil = mask->GetImageStencilData();
      bool found = false;
      
      for ( this->ContourFilters->InitTraversal(ait); 
             (cf=static_cast< vtkKWEStencilContourFilter * >(
                this->ContourFilters->GetNextItemAsObject(ait))); )
        {
        if (static_cast< vtkImageStencilData * >(cf->GetInput()) == stencil)
          {
          found = true;
          break;
          }
        }
      
      if (!found)
        {
        vtkKWEStencilContourFilter * contourFilter = vtkKWEStencilContourFilter::New();
        vtkPolyDataMapper * mapper = vtkPolyDataMapper::New();
        vtkActor * actor = vtkActor::New();

        contourFilter->SetInput(stencil);
        contourFilter->SetExtent(this->Extent);
        contourFilter->Update();        
        mapper->SetInput(contourFilter->GetOutput());
        mapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapper->ScalarVisibilityOff();
        actor->SetMapper(mapper);
        actor->SetProperty( sketch->GetPaintbrushProperty()->GetProperty() );
        
        this->ContourPolyDataActors->AddItem(actor);
        this->ContourPolyDataMappers->AddItem(mapper);
        this->ContourFilters->AddItem(contourFilter);

        contourFilter->Delete();
        mapper->Delete();
        actor->Delete();
        }
      }
    }

  // Now remove actors that correspond to sketches that have been removed or 
  // unhighlighted.
  
  std::vector< int > indicesToRemove;
  int j = 0;
  for ( this->ContourFilters->InitTraversal(ait); 
         (cf=static_cast< vtkKWEStencilContourFilter * >(
            this->ContourFilters->GetNextItemAsObject(ait))); j++)
    {
    bool found = false;    
    for (int i = 0; i < nSketches; i++)
      {
      vtkKWEPaintbrushSketch * sketch = this->PaintbrushDrawing->GetItem(i);
      if (sketch->GetPaintbrushProperty()->GetHighlight())
        {
        if (vtkKWEPaintbrushStencilData *mask = vtkKWEPaintbrushStencilData::
                              SafeDownCast(sketch->GetPaintbrushData()))
          {
          if (static_cast< vtkImageStencilData * >(cf->GetInput())
                                    == mask->GetImageStencilData())
            {
            found = true;
            break;
            }
          }
        }
      }

    if (!found)
      {
      indicesToRemove.push_back(j);
      }
    }
    
    
  for (std::vector< int >::const_iterator it = indicesToRemove.begin(); 
         it != indicesToRemove.end(); ++it)
    {
    int indexToRemove = *it;

    // We need to get rid of this actor and its associated pipeline
    this->ContourPolyDataActors->RemoveItem( indexToRemove );
    this->ContourPolyDataMappers->RemoveItem( indexToRemove );
    this->ContourFilters->RemoveItem( indexToRemove );
    }
    
  
  for ( this->ContourFilters->InitTraversal(ait); 
         (cf=static_cast< vtkKWEStencilContourFilter * >(
            this->ContourFilters->GetNextItemAsObject(ait))); )
    {
    cf->SetExtent( this->Extent );
    cf->Update();
    }
}
     
//----------------------------------------------------------------------
int vtkKWEPaintbrushHighlightActors::RenderOpaqueGeometry(vtkViewport *viewport)
{
  this->BuildRepresentation();
  int count = 0;
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    count += ac->RenderOpaqueGeometry(viewport);
    }
    
  return count;
}

#if VTKEdge_VTK_VERSION_DATE > 20070305
//----------------------------------------------------------------------
int vtkKWEPaintbrushHighlightActors
::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count = 0;
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    count += ac->RenderTranslucentPolygonalGeometry(viewport);
    }
    
  return count;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushHighlightActors::HasTranslucentPolygonalGeometry()
{
  int result = 0;
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    result |= ac->HasTranslucentPolygonalGeometry();
    }
    
  return result;
}

#else

//----------------------------------------------------------------------
// Legacy method to support VTK source versions prior to 2007/03/05
int vtkKWEPaintbrushHighlightActors::
RenderTranslucentGeometry(vtkViewport *viewport)
{
  int count = 0;
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    count += ac->RenderTranslucentGeometry(viewport);
    }
    
  return count;
}
#endif 

//----------------------------------------------------------------------
void vtkKWEPaintbrushHighlightActors::GetActors( vtkPropCollection * pc )
{
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    ac->GetActors(pc);
    }  
}

//----------------------------------------------------------------------
double* vtkKWEPaintbrushHighlightActors::GetBounds()
{
  double bounds[6];
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait); 
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    ac->GetBounds(bounds);
    for (int i=0; i<3; i++)
      {
      this->Bounds[2*i+1] = (bounds[2*i+1]>this->Bounds[2*i+1]) ? 
                             (bounds[2*i+1]):(this->Bounds[2*i+1]);
      this->Bounds[2*i]   = (bounds[2*i]<this->Bounds[2*i]) ? 
                             (bounds[2*i]):(this->Bounds[2*i]);
      }  
    }
  return this->Bounds;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushHighlightActors::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

