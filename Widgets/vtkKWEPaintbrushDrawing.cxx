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
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushSketch.h"

#include "vtkObjectFactory.h"
#include "vtkImageStencilData.h"
#include "vtkProperty.h"
#include "vtkImageData.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkKWEPaintbrushPropertyManager.h"
#include "vtkKWEPaintbrushLabelData.h"
#include "vtkKWEPaintbrushStroke.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include <vtkstd/algorithm>
#include <limits>

vtkCxxRevisionMacro(vtkKWEPaintbrushDrawing, "$Revision: 725 $");
vtkStandardNewMacro(vtkKWEPaintbrushDrawing);

//----------------------------------------------------------------------
vtkKWEPaintbrushDrawing::vtkKWEPaintbrushDrawing()
{
  this->PaintbrushData            = NULL;
  this->PaintbrushOperation       = NULL;
  this->ImageData                 = NULL;
  this->PaintbrushPropertyManager = vtkKWEPaintbrushPropertyManager::New(this);
  this->Collection                = vtkCollection::New();
  this->Representation            = vtkKWEPaintbrushEnums::Binary;
  this->MaximumNumberOfSketches   = 0;
  this->LabelMap                  = 0;
}

//----------------------------------------------------------------------
vtkKWEPaintbrushDrawing::~vtkKWEPaintbrushDrawing()
{
  if (this->PaintbrushData)
    {
    this->PaintbrushData->Delete();
    }
  this->Collection->Delete();
  if (this->PaintbrushOperation)
    {
    this->PaintbrushOperation->Delete();
    }
  if (this->ImageData)
    {
    this->ImageData->Delete();
    }
  this->PaintbrushPropertyManager->Delete();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::SetImageData( vtkImageData * i)
{
  vtkKWEPaintbrushSketch *a;
  vtkCollectionSimpleIterator ait;
  for ( this->Collection->InitTraversal(ait); 
    (a=static_cast< vtkKWEPaintbrushSketch * >(this->Collection->GetNextItemAsObject(ait))); )
    {
    a->SetImageData(i);
    }
  vtkSetObjectBodyMacro( ImageData, vtkImageData, i)
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::InitializeData()
{
  if (this->PaintbrushData)
    {
    this->PaintbrushData->Delete();
    }
  
  if (this->Representation == vtkKWEPaintbrushEnums::Binary)
    {
    this->PaintbrushData = vtkKWEPaintbrushStencilData::New();
    }
  else if (this->Representation == vtkKWEPaintbrushEnums::Grayscale)
    {
    this->PaintbrushData = vtkKWEPaintbrushGrayscaleData::New();
    }
  else
    {
    this->PaintbrushData = vtkKWEPaintbrushLabelData::New();
    }

  this->AllocatePaintbrushData();  
  if (this->GetNumberOfItems() == 0)
    {
    // If there are no items, create one by default. The user, when he creates
    // this widget, expects there to be at least a sketch alive, so he can 
    // sketch something.
    
    vtkKWEPaintbrushSketch *m = vtkKWEPaintbrushSketch::New();
    m->SetRepresentation(this->Representation);
    this->AddItem(m);
    m->Delete();

    this->AssignUniqueLabelToSketch(m); // Assign a unique label to the sketch.
    }
  
  else
    { 

    vtkKWEPaintbrushSketch *a;
    vtkCollectionSimpleIterator ait;

    // Initialize all the sketches in the drawing. Be prepared to lose your 
    // sketches.
  
    for ( this->Collection->InitTraversal(ait); 
      (a=static_cast< vtkKWEPaintbrushSketch * >(this->Collection->GetNextItemAsObject(ait))); )
      {
      a->Initialize();
      }
      
  
    if (this->Representation == vtkKWEPaintbrushEnums::Label)
      {
      // For Label maps, all sketches point to the same label map, since they
      // will be editing different labels within the same label map.
      this->SetPaintbrushData( static_cast< 
          vtkKWEPaintbrushLabelData * >(this->PaintbrushData) );
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::
SetPaintbrushOperation( vtkKWEPaintbrushOperation * i)
{
  vtkKWEPaintbrushSketch *a;
  vtkCollectionSimpleIterator ait;
  for ( this->Collection->InitTraversal(ait); 
    (a=static_cast< vtkKWEPaintbrushSketch * >(this->Collection->GetNextItemAsObject(ait))); )
    {
    a->SetPaintbrushOperation(i);
    }
  vtkSetObjectBodyMacro( PaintbrushOperation, vtkKWEPaintbrushOperation, i)
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::AllocatePaintbrushData()
{
  vtkImageData *i = this->GetImageData();
  if (!i)
    {
    vtkErrorMacro( << "No image data");
    }
  if (!this->PaintbrushData)
    {
    vtkErrorMacro( << "Must call Initialize() before using this class !");
    }
  
  int extent[6];
  double spacing[3], origin[3];
  i->GetWholeExtent(extent);
  i->GetSpacing(spacing);
  i->GetOrigin(origin);

  // If a label map has been supplied (and our representation is a label map), 
  // used the supplied one, otherwise allocate fresh paintbrush data with the 
  // same size as the canvas.
  
  this->PaintbrushData->SetExtent(extent);
  this->PaintbrushData->SetSpacing(spacing);
  this->PaintbrushData->SetOrigin(origin);
  if (this->Representation == vtkKWEPaintbrushEnums::Label && this->LabelMap )
    {
    
    // Sanity check : Ensure that the label map supplied by the user has the 
    // same extents as the canvas, or we will have cryptic errors later on 
    // during the overlay-blend render phase.
    if (!vtkKWEPaintbrushUtilities::ExtentIsEqualToExtent(
                     extent, this->LabelMap->GetExtent()))
      {
      vtkErrorMacro( << "The extents of the canvas and that of the label map "
                     << "must be equal !" );
      }

    static_cast< vtkKWEPaintbrushLabelData * >(
        this->PaintbrushData)->SetLabelMap( this->LabelMap );
    }
  else
    {
    this->PaintbrushData->Allocate();
    }
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushDrawing::AddNewStroke( 
      int n,    // Stroke is added to the n'th sketch
      int type, // Draw or Erase ?
      vtkKWEPaintbrushData *userSpecifiedStrokeData // User provided initialization
      )
{
  if (userSpecifiedStrokeData)
    {
    this->PaintbrushData->Add(userSpecifiedStrokeData);
    }
  return this->GetItem(n)->AddNewStroke( type, userSpecifiedStrokeData );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::AddShapeToCurrentStroke( int n, double p[3] )
{
  if (n < this->GetNumberOfItems() && n >= 0)
    {
    // Add (or subtract) a shape centered at 'p' to the current stroke. 
    // Also add (or subtract) the same to this->PaintbrushData, as a service.
    this->GetItem(n)->AddShapeToCurrentStroke(p, this->PaintbrushData);
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::SetPaintbrushData( vtkKWEPaintbrushLabelData * data )
{
  vtkSetObjectBodyMacro( PaintbrushData, vtkKWEPaintbrushData, data );
  
  // For label maps, all sketches share the same data, this label map.
  // Set it on all of them.

  vtkKWEPaintbrushSketch *a;
  vtkCollectionSimpleIterator ait;
  for ( this->Collection->InitTraversal(ait); 
    (a=static_cast< vtkKWEPaintbrushSketch * >(
      this->Collection->GetNextItemAsObject(ait))); )
    {
    a->SetPaintbrushData( data );
    }

  // Sanity check. Make sure the scalar type of the data is correct.
  if (data && data->GetLabelMap())
    {
    if (data->GetLabelMap()->GetScalarType() 
              != vtkKWEPaintbrushEnums::GetLabelType())
      {
      vtkErrorMacro( << "DataType of the label map must match the enumeration in "
        << "vtkKWEPaintbrushEnums. Either change the datatype of the label map or change "
        << "the enumeration to suit your needs" );
      }
    }
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawing* vtkKWEPaintbrushDrawing::GetData(vtkInformation* info)
{
  return
    info? vtkKWEPaintbrushDrawing::SafeDownCast(info->Get(DATA_OBJECT())) : 0;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushDrawing* vtkKWEPaintbrushDrawing::GetData(vtkInformationVector* v,
                                                    int i)
{
  return vtkKWEPaintbrushDrawing::GetData(v->GetInformationObject(i));
}

//----------------------------------------------------------------------------
// Determine the modified time of this object
unsigned long vtkKWEPaintbrushDrawing::GetMTime()
{
  unsigned long result = 0;
  vtkKWEPaintbrushSketch *a;
  vtkCollectionSimpleIterator ait;
  for ( this->Collection->InitTraversal(ait); 
          (a=static_cast< vtkKWEPaintbrushSketch * >(
            this->Collection->GetNextItemAsObject(ait))); )
    {
    unsigned long t = a->GetMTime();
    if (t > result) { result = t; }
    }

  unsigned long mtime = vtkObject::GetMTime();
  result = ( mtime > result ? mtime : result);  
  return result;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushSketch * vtkKWEPaintbrushDrawing::AddItem() 
{
  if (this->MaximumNumberOfSketches && 
      this->GetNumberOfItems() == this->MaximumNumberOfSketches)
    {
    return NULL; // We hit the limit.
    }

  vtkKWEPaintbrushSketch * a = vtkKWEPaintbrushSketch::New();
  this->Collection->AddItem(a);
  a->SetRepresentation(this->Representation);
  a->SetImageData(this->ImageData);
  a->SetPaintbrushOperation(this->PaintbrushOperation);
  a->PaintbrushDrawing = this;
  a->Initialize();

  // For Label maps, all sketches point to the same label map, since they
  // will be editing different labels within the same label map.
  if (this->Representation == vtkKWEPaintbrushEnums::Label)
    {
    a->SetPaintbrushData( this->PaintbrushData );
    }

  a->Delete();
  return a;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::AddItem(vtkKWEPaintbrushSketch *a) 
{
  if (this->MaximumNumberOfSketches && 
      this->GetNumberOfItems() == this->MaximumNumberOfSketches)
    {
    return; // We hit the limit.
    }

  this->Collection->AddItem(a);
  a->SetRepresentation(this->Representation);
  a->SetImageData(this->ImageData);
  a->SetPaintbrushOperation(this->PaintbrushOperation);
  a->PaintbrushDrawing = this;
  a->Initialize();

  // For Label maps, all sketches point to the same label map, since they
  // will be editing different labels within the same label map.
  if (this->Representation == vtkKWEPaintbrushEnums::Label)
    {
    a->SetPaintbrushData( this->PaintbrushData );
    }
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushSketch *vtkKWEPaintbrushDrawing::GetItem(int i)
{
  return static_cast< vtkKWEPaintbrushSketch * >(
          this->Collection->GetItemAsObject(i));
}


//----------------------------------------------------------------------------
int vtkKWEPaintbrushDrawing::IsItemPresent( vtkKWEPaintbrushSketch * s )
{
  return this->Collection->IsItemPresent(s);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::RemoveAllItems()
{
  return this->Collection->RemoveAllItems();
}

//----------------------------------------------------------------------------
int vtkKWEPaintbrushDrawing::RemoveItem( vtkKWEPaintbrushSketch *s )
{
  if (this->Collection->IsItemPresent(s))
    {
    this->Collection->RemoveItem(s);
    return 1;
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::ShallowCopy( vtkDataObject * src )
{
  if (src == this) 
    {
    return;
    }

  vtkKWEPaintbrushDrawing *srcDrawing = vtkKWEPaintbrushDrawing::SafeDownCast(src);
  if (!srcDrawing)
    {
    vtkErrorMacro( << "Safedowncast to vtkKWEPaintbrushDrawing failed." )
    return;
    }

  this->SetRepresentation( srcDrawing->GetRepresentation() );
  this->SetImageData( srcDrawing->GetImageData());
  this->SetPaintbrushOperation( srcDrawing->GetPaintbrushOperation() );
  this->MaximumNumberOfSketches = srcDrawing->MaximumNumberOfSketches;
  this->Initialize();

  // Copy some property defaults.
  this->PaintbrushPropertyManager->SetHighlightType( srcDrawing->
      GetPaintbrushPropertyManager()->GetHighlightType() );

  // Shallow copy the collection of sketches.
  this->Collection->UnRegister(this);
  this->Collection = srcDrawing->Collection;
  this->Collection->Register(this);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::ComposeStrokes()
{
  // FIXME
  // Is clearing the whole PaintbrushData necessary ?

  // vector containing all the strokes in the drawing. (upto the undoable one)
  vtkstd::vector< vtkKWEPaintbrushStroke * > strokes;

  vtkKWEPaintbrushSketch *sketch;
  vtkCollectionSimpleIterator it;
  for ( this->Collection->InitTraversal(it); 
         (sketch=static_cast< vtkKWEPaintbrushSketch * >(
           this->Collection->GetNextItemAsObject(it))); )
    {
    sketch->GetStrokes( strokes );
    }

  // Sort the strokes, oldest to latest.
  vtkstd::sort( strokes.begin(), strokes.end(), vtkKWEPaintbrushStroke::IsRecent );

  // FIXME
  // We could be smarter by composing just the strokes with extents that 
  // intersect with the one just undone/redone.

  vtkstd::vector< vtkKWEPaintbrushStroke * >::iterator sit = strokes.begin();
  for ( ; sit != strokes.end(); ++sit )
    {
    vtkKWEPaintbrushStroke * stroke = *sit;
    if (stroke->GetState() == vtkKWEPaintbrushEnums::Erase)
      {
      this->PaintbrushData->Subtract(stroke->GetPaintbrushData()); 
      }
    else   
      {
      this->PaintbrushData->Add(stroke->GetPaintbrushData());
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::AssignUniqueLabelToSketch( vtkKWEPaintbrushSketch *s )
{
  int nSketches = this->GetNumberOfItems();
  vtkstd::vector< vtkKWEPaintbrushEnums::LabelType > labels( nSketches );
  for (int i = 0; i < nSketches; i++)
    {
    labels[i] = this->GetItem(i)->GetLabel();
    }
  vtkKWEPaintbrushEnums::LabelType minLabel = 1; 
  vtkKWEPaintbrushEnums::LabelType maxLabel 
    = vtkstd::numeric_limits< vtkKWEPaintbrushEnums::LabelType >::max();

  for (vtkKWEPaintbrushEnums::LabelType i = minLabel; i < maxLabel; i++)
    {
    if (std::find(labels.begin(), labels.end(), i) != labels.end())
      {
      s->SetLabel(i);
      return;  
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::InitializeSketches()
{
  // For representations that are not label maps, this should already have 
  // been done.
  
  if (!this->PaintbrushData)
    {
    vtkErrorMacro( << "PaintbrushData is NULL. Did you add this sketch to the "
      << "drawing and call InitializeData on the drawing prior to " 
      << "initializing the sketch ?");
    return;
    }

  if (this->GetNumberOfItems() == 0)
    {
    vtkErrorMacro( << "No sketches to initialize. " <<
      "Did you call AddSketch and add sketches to the drawing first ?");
    return;
    }

  if (this->Representation != vtkKWEPaintbrushEnums::Label)
    {
    vtkErrorMacro( << "This method is only intended to be used when editing LabelMaps." 
       << " Did you set the representation first ?" );
    return;
    }
  
  vtkstd::vector< vtkKWEPaintbrushEnums::LabelType > 
        labels(this->Collection->GetNumberOfItems());
  unsigned int i = 0;
  vtkCollectionSimpleIterator ait;
  vtkKWEPaintbrushSketch *a;
  for ( this->Collection->InitTraversal(ait); 
      (a=static_cast< vtkKWEPaintbrushSketch * >(this->Collection->GetNextItemAsObject(ait))); )
    { 
    labels[i++] = a->GetLabel();
    }

  vtkImageData *image = vtkKWEPaintbrushLabelData::SafeDownCast( 
                        this->PaintbrushData )->GetLabelMap();

  // Populate all the strokes at one shot
  vtkstd::map< vtkKWEPaintbrushEnums::LabelType, 
                 vtkSmartPointer< vtkKWEPaintbrushStencilData > > initialStrokes 
      =  vtkKWEPaintbrushUtilities::GetStencilsFromImage( image, labels );
  
  // Initialize all the sketches with their user-specified strokes.
  for ( this->Collection->InitTraversal(ait); 
      (a=static_cast< vtkKWEPaintbrushSketch * >(this->Collection->GetNextItemAsObject(ait))); )
    { 
    a->AddNewStroke( vtkKWEPaintbrushEnums::Draw, initialStrokes[a->GetLabel()], NULL, true );
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushDrawing::PrintSelf(ostream& os, vtkIndent indent)
{
  if (this->Representation == vtkKWEPaintbrushEnums::Binary)
    {
    os << indent << "Representation: Binary" << endl;
    }
  else
    {
    os << indent << "Representation: Grayscale" << endl;
    }
  os << indent << "PaintbrushOperation: " << this->PaintbrushOperation << endl;
  if (this->PaintbrushOperation)
    {
    this->PaintbrushOperation->PrintSelf( os, indent.GetNextIndent() );
    }
  os << indent << "Number of sketches: " << this->GetNumberOfItems() << endl;
  for (int i = 0; i < this->GetNumberOfItems(); i++)
    {
    os << indent << "Sketch: " << i << " (" << this->GetItem(i) << ")" << endl;
    this->GetItem(i)->PrintSelf(os, indent.GetNextIndent());
    }
  if (this->MaximumNumberOfSketches)
    {
    os << indent << "MaximumNumberOfSketches: " 
       << this->MaximumNumberOfSketches << endl;
    }
  this->Superclass::PrintSelf(os,indent);
}
