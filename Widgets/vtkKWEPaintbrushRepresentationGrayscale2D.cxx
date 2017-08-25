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

#include "vtkKWEPaintbrushRepresentationGrayscale2D.h"

#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEPaintbrushPropertyManager.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushPropertyManager.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageClip.h"
#include "vtkImageActor.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkSmartPointer.h"
#include "vtkActorCollection.h"
#include "vtkMapperCollection.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushRepresentationGrayscale2D, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEPaintbrushRepresentationGrayscale2D);

//----------------------------------------------------------------------
vtkKWEPaintbrushRepresentationGrayscale2D::vtkKWEPaintbrushRepresentationGrayscale2D()
{
  this->ContourFilters         = vtkCollection::New();
  this->ExtractComponents      = vtkCollection::New();
  this->ImageClips             = vtkCollection::New();
  this->ContourPolyDataMappers = vtkMapperCollection::New();
  this->ContourPolyDataActors  = vtkActorCollection::New();
  this->IsoValue               = 127.5;

  this->PaintbrushDrawing->SetRepresentationToGrayscale();

  // The paintbrushes don't snap to the grid
  vtkImageActorPointPlacer * v = vtkImageActorPointPlacer::New();
  this->SetShapePlacer(v);
  v->Delete();

  // Thick highlight looks better when we have contours
  this->PaintbrushDrawing->GetPaintbrushPropertyManager()->SetHighlightType(
      vtkKWEPaintbrushProperty::ThickHighlight);

  this->PaintbrushOperation->GetPaintbrushShape()->
    SetRepresentation( vtkKWEPaintbrushEnums::Grayscale );
}

//----------------------------------------------------------------------
vtkKWEPaintbrushRepresentationGrayscale2D::~vtkKWEPaintbrushRepresentationGrayscale2D()
{
  this->ExtractComponents->Delete();
  this->ImageClips->Delete();
  this->ContourFilters->Delete();
  this->ContourPolyDataMappers->Delete();
  this->ContourPolyDataActors->Delete();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentationGrayscale2D::BuildRepresentation()
{
  // The pipeline is as follows :
  //
  //  SketchImage_0 --> ExtractFirstComponent_0 --> ImageClip_0 -->
  //             ContourFilter_0 --> PolyDataMapper_0 --> Actor_0
  //
  //  SketchImage_1 --> ExtractFirstComponent_1 --> ImageClip_1 -->
  //             ContourFilter_1 --> PolyDataMapper_1 --> Actor_1
  // ....
  // ....

  vtkImageClip *ic;
  vtkActor *ac;
  vtkImageExtractComponents *a;
  vtkContourFilter * cf;
  vtkCollectionSimpleIterator ait;

  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();

  for (int i = 0; i < nSketches; i++)
    {
    vtkKWEPaintbrushSketch * sketch = this->PaintbrushDrawing->GetItem(i);
    vtkKWEPaintbrushGrayscaleData *mask = vtkKWEPaintbrushGrayscaleData::
          SafeDownCast(sketch->GetPaintbrushData());

    if (mask)
      {
      vtkImageData * image = mask->GetImageData();
      bool found = false;

      for ( this->ExtractComponents->InitTraversal(ait);
             (a=static_cast< vtkImageExtractComponents * >(
                this->ExtractComponents->GetNextItemAsObject(ait))); )
        {
        if (a->GetInput() == image)
          {
          found = true;
          break;
          }
        }

      if (!found)
        {
        vtkImageExtractComponents * extractComponents = vtkImageExtractComponents::New();
        vtkImageClip * imageClip = vtkImageClip::New();
        vtkContourFilter * contourFilter = vtkContourFilter::New();
        vtkPolyDataMapper * mapper = vtkPolyDataMapper::New();
        vtkActor * actor = vtkActor::New();

        extractComponents->SetInput( image );
        extractComponents->SetComponents(0);
        imageClip->SetClipData(1);
        imageClip->SetInput(extractComponents->GetOutput());
        contourFilter->SetInput(imageClip->GetOutput());
        contourFilter->SetValue(0, this->IsoValue);
        imageClip->Update();
        contourFilter->Update();
        mapper->SetInput(contourFilter->GetOutput());
        mapper->SetResolveCoincidentTopologyToPolygonOffset();
        mapper->ScalarVisibilityOff();
        actor->SetMapper(mapper);

        vtkProperty * property = vtkProperty::New();
        property->SetColor( 0.0, 0.0, 0.0 );
        actor->SetProperty( property );
        property->Delete();

        this->ContourPolyDataActors->AddItem(actor);
        this->ContourPolyDataMappers->AddItem(mapper);
        this->ExtractComponents->AddItem(extractComponents);
        this->ImageClips->AddItem(imageClip);
        this->ContourFilters->AddItem(contourFilter);

        imageClip->Delete();
        extractComponents->Delete();
        contourFilter->Delete();
        mapper->Delete();
        actor->Delete();
        }
      }
    }

  // Now remove old unused ones, if any

  std::vector< int > indicesToRemove;
  int j = 0;
  for ( this->ExtractComponents->InitTraversal(ait);
         (a=static_cast< vtkImageExtractComponents * >(
            this->ExtractComponents->GetNextItemAsObject(ait))); j++)
    {
    bool found = false;
    for (int i = 0; i < nSketches; i++)
      {
      if (vtkKWEPaintbrushGrayscaleData *mask = vtkKWEPaintbrushGrayscaleData::
          SafeDownCast(this->PaintbrushDrawing->GetItem(i)->GetPaintbrushData()))
        {
        if (a->GetInput() == mask->GetImageData())
          {
          found = true;
          break;
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
    this->ContourPolyDataActors->RemoveItem(indexToRemove);
    this->ContourPolyDataMappers->RemoveItem(indexToRemove);
    this->ExtractComponents->RemoveItem(indexToRemove);
    this->ImageClips->RemoveItem(indexToRemove);
    this->ContourFilters->RemoveItem(indexToRemove);
    }

  // Now, for the new actors we created, we need to set reasonable colors,
  // preferably not the same colors as the ones the other actors use. So
  // let's go about working some magic to get new colors.

  // Clear our table.
  this->PaintbrushDrawing->GetPaintbrushPropertyManager()->Initialize();

  // These are all the actors that were created long ago.
  for ( this->ContourPolyDataActors->InitTraversal(ait);
          (ac=this->ContourPolyDataActors->GetNextActor(ait)); )
    {
    this->PaintbrushDrawing->GetPaintbrushPropertyManager()->AddUsedColor(
        ac->GetProperty());
    }

  // Visit all the newly created actors and assign properties.
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    double rgb[3];
    ac->GetProperty()->GetColor(rgb);
    if (rgb[0] == 0.0 && rgb[1] == 0.0 && rgb[2] == 0.0)
      {

      // This is a newly created actor, with no color set yet. Request a
      // possibly unique color for the sketch that this actor is rendering
      // and assign it to the actor's property.

      ac->SetProperty( this->PaintbrushDrawing->GetPaintbrushPropertyManager()->
          RequestColorForSketch( this->GetSketch(ac) )->GetProperty());
      }
    }


  // Set the update extent to the extent of the image actor. That's all we
  // can see anyway. If the image actor's extents are outside the canvas
  // turn the visibility off.

  int imageActorExtents[6], imageDataExtents[6], intersectingExtents[6];
  this->ImageActor->GetDisplayExtent( imageActorExtents );
  this->ImageData->GetExtent( imageDataExtents );
  int visible =
    vtkKWEPaintbrushUtilities::GetIntersectingExtents( imageActorExtents,
                                                    imageDataExtents,
                                                    intersectingExtents)
    && this->Visibility;

  if (visible)
    {
    for ( this->ImageClips->InitTraversal(ait);
          (ic=static_cast< vtkImageClip * >(
            this->ImageClips->GetNextItemAsObject(ait))); )
      {
      ic->SetOutputWholeExtent( intersectingExtents );
      }

    // Set the isovalue of the contour filters to the isovalue and update the
    // contour filter.
    for ( this->ContourFilters->InitTraversal(ait);
           (cf=static_cast< vtkContourFilter * >(
              this->ContourFilters->GetNextItemAsObject(ait))); )
      {
      cf->SetValue(0, this->IsoValue);
      cf->Update();
      }
    }

  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    ac->SetVisibility( visible );
    }
}

//----------------------------------------------------------------------
vtkKWEPaintbrushSketch * vtkKWEPaintbrushRepresentationGrayscale2D::
GetSketch( vtkActor * a )
{
  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  int i = 0;
  vtkImageData * image = NULL;
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();

  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait)); ++i)
    {
    if (ac == a)
      {
      image = static_cast< vtkImageData *>((
          static_cast< vtkImageExtractComponents * >(
            this->ExtractComponents->GetItemAsObject(i)))->GetInput());
      }
    }

  for (i = 0; i < nSketches; i++)
    {
    vtkKWEPaintbrushSketch * sketch = this->PaintbrushDrawing->GetItem(i);
    if (vtkKWEPaintbrushGrayscaleData *mask = vtkKWEPaintbrushGrayscaleData::
        SafeDownCast(sketch->GetPaintbrushData()))
      {
      if (image == mask->GetImageData())
        {
        return sketch;
        }
      }
    }

  return NULL;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentationGrayscale2D::RenderOverlay(vtkViewport *viewport)
{
  int count = 0;
  if (this->ShapeOutlineActor->GetVisibility())
    {
    count += this->ShapeOutlineActor->RenderOverlay(viewport);
    }

  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    if (ac->GetVisibility())
      {
      count += ac->RenderOverlay(viewport);
      }
    }

  return count;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentationGrayscale2D
::RenderOpaqueGeometry(vtkViewport *viewport)
{
  this->BuildRepresentation();
  int count = 0;
  if (this->ShapeOutlineActor->GetVisibility())
    {
    count += this->ShapeOutlineActor->RenderOpaqueGeometry(viewport);
    }

  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    if (ac->GetVisibility())
      {
      double rgb[3];
      ac->GetProperty()->GetColor(rgb);
      count += ac->RenderOpaqueGeometry(viewport);
      }
    }

  return count;
}

#if VTKEdge_VTK_VERSION_DATE > 20070305
//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentationGrayscale2D
::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  int count = 0;
  if (this->ShapeOutlineActor->GetVisibility())
    {
    count += this->ShapeOutlineActor->RenderTranslucentPolygonalGeometry(viewport);
    }

  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    if (ac->GetVisibility())
      {
      count += ac->RenderTranslucentPolygonalGeometry(viewport);
      }
    }

  return count;
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushRepresentationGrayscale2D::HasTranslucentPolygonalGeometry()
{
  int result = 0;
  if (this->ShapeOutlineActor->GetVisibility())
    {
    result |= this->ShapeOutlineActor->HasTranslucentPolygonalGeometry();
    }

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
int vtkKWEPaintbrushRepresentationGrayscale2D::
RenderTranslucentGeometry(vtkViewport *viewport)
{
  int count = 0;
  if (this->ShapeOutlineActor->GetVisibility())
    {
    count += this->ShapeOutlineActor->RenderTranslucentGeometry(viewport);
    }

  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    if (ac->GetVisibility())
      {
      count += ac->RenderTranslucentGeometry(viewport);
      }
    }

  return count;
}
#endif

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentationGrayscale2D::GetActors( vtkPropCollection * pc )
{
  this->ShapeOutlineActor->GetActors(pc);

  vtkActor *ac;
  vtkCollectionSimpleIterator ait;
  for ( this->ContourPolyDataActors->InitTraversal(ait);
         (ac=this->ContourPolyDataActors->GetNextActor(ait));)
    {
    ac->GetActors(pc);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentationGrayscale2D::DeepCopy(vtkWidgetRepresentation *rep)
{
  if (this == rep)
    {
    return;
    }

  vtkKWEPaintbrushRepresentationGrayscale2D *r
    = vtkKWEPaintbrushRepresentationGrayscale2D::SafeDownCast(rep);
  if (r)
    {
    this->IsoValue = r->IsoValue;
    }

  this->Superclass::DeepCopy(rep);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentationGrayscale2D::SetPaintbrushDrawing(
                       vtkKWEPaintbrushDrawing * drawing )
{
  vtkKWEPaintbrushRepresentation::SetPaintbrushDrawing(drawing);

  // Thick highlight looks better when we have contours
  this->PaintbrushDrawing->GetPaintbrushPropertyManager()->SetHighlightType(
      vtkKWEPaintbrushProperty::ThickHighlight);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushRepresentationGrayscale2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

