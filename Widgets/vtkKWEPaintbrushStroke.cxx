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
#include "vtkKWEPaintbrushStroke.h"

#include "vtkObjectFactory.h"
#include "vtkImageStencilData.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkImageData.h"
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShape.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushStroke, "$Revision: 3550 $");
vtkStandardNewMacro(vtkKWEPaintbrushStroke);
vtkCxxSetObjectMacro(vtkKWEPaintbrushStroke,PaintbrushOperation,
                                      vtkKWEPaintbrushOperation);
vtkCxxSetObjectMacro(vtkKWEPaintbrushStroke,PaintbrushData,vtkKWEPaintbrushData);

//----------------------------------------------------------------------
vtkKWEPaintbrushStroke::vtkKWEPaintbrushStroke()
{
  this->PaintbrushOperation      = NULL;
  this->PaintbrushData           = NULL;  
  this->ImageData                = NULL;
  this->Internals                = new vtkKWEPaintbrushStrokeInternals;
  this->Extent[0]                = -1;
  this->Extent[1]                = 0;
  this->Extent[2]                = -1;
  this->Extent[3]                = 0;
  this->Extent[4]                = -1;
  this->Extent[5]                = 0;
  this->Representation           = vtkKWEPaintbrushEnums::Binary;
  this->Tolerance                = 1e-4;
  this->Label                    = 1;
  this->DrawTime                 = this->GetMTime();
}

//----------------------------------------------------------------------
vtkKWEPaintbrushStroke::~vtkKWEPaintbrushStroke()
{
  this->SetPaintbrushData(NULL);

  // Delete the internals.
  for (unsigned int i=0; i<this->Internals->Node.size(); i++)
    {
    delete this->Internals->Node[i];
    }
  this->Internals->Node.clear();
  delete this->Internals;

  this->SetPaintbrushOperation(NULL);
  this->SetImageData(NULL);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetImageData( vtkImageData * imageData )
{
  if (this->ImageData != imageData)
    {
    vtkImageData * var = this->ImageData;                     
    this->ImageData = imageData;
    if (this->ImageData != NULL) { this->ImageData->Register(this); }
    if (var != NULL)
      {                                                    
      var->UnRegister(this);                    
      }                                                    
    this->Modified();                                      
    }

  if (this->ImageData)
    {
    this->Allocate();
    }
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushStroke::AddShapeAtPosition( double p[3], 
                              vtkKWEPaintbrushData * auxiliaryData1, 
                              vtkKWEPaintbrushData * auxiliaryData2)
{
  if (!this->PaintbrushOperation)
    {
    vtkErrorMacro(<< "PaintbrushOperation must be set prior to use");
    return 0;
    }
  if (!this->PaintbrushData)
    {
    vtkErrorMacro(<< "PaintbrushData must be allocated before use");
    return 0;
    }
  if (!(this->Internals->State == vtkKWEPaintbrushEnums::Draw || 
        this->Internals->State == vtkKWEPaintbrushEnums::Erase))
    {
    return 0;
    }

  // Check if the point is already in the sequence

  for (unsigned int i=0; i<this->Internals->Node.size(); i++)
    {
    if (   fabs(this->Internals->Node[i]->WorldPosition[0] - p[0]) < this->Tolerance
        && fabs(this->Internals->Node[i]->WorldPosition[1] - p[1]) < this->Tolerance
        && fabs(this->Internals->Node[i]->WorldPosition[2] - p[2]) < this->Tolerance)
      {
      return 0;
      } 
    }

  // Make sure that auxiliaryData1 and auxiliaryData2 aren't the same, so that
  // we may avoid adding twice needlessly. This could happen if the sketch
  // and the drawing share the same data (as is the case with label maps).
  vtkKWEPaintbrushData *auxData1 = auxiliaryData1;
  vtkKWEPaintbrushData *auxData2 = 
    (auxiliaryData2 != auxiliaryData1) ? auxiliaryData2 : NULL;

  this->PaintbrushData->SetLabel(this->Label);
  if (auxData1)
    {
    auxData1->SetLabel(this->Label);
    }
  if (auxData2)
    {
    auxData2->SetLabel(this->Label);
    }

  // Add the a template stencil at the given point to the current stencil

  vtkKWEPaintbrushData * data;
  if (this->Representation == vtkKWEPaintbrushEnums::Binary)
    {
    data = vtkKWEPaintbrushStencilData::New();
    }
  else if (this->Representation == vtkKWEPaintbrushEnums::Grayscale)
    {
    data = vtkKWEPaintbrushGrayscaleData::New();
    }
  else
    {
    cerr << "ERROR: Unsupported paintbrush representation type." << std::endl;
    return 0;
    }
  data->SetLabel( this->Label );

  // Get the paintbrush data filtered through the operation. This allows us 
  // to support in-place smart filters etc..
  vtkKWEPaintbrushEnums::OperationType op;
  // This operation may change the value of op.
  this->PaintbrushOperation->GetPaintbrushData(data, p, op);

  if( op == vtkKWEPaintbrushEnums::Replace )  
    {
    this->PaintbrushData->Replace(data); 
    if (auxData1)
      {
      auxData1->Replace(data);
      }
    if (auxData2)
      {
      auxData2->Replace(data);
      }
    }
  else if(  (this->Internals->State == vtkKWEPaintbrushEnums::Draw &&
        op == vtkKWEPaintbrushEnums::Add)  
      ||
       (this->Internals->State == vtkKWEPaintbrushEnums::Erase &&
        op == vtkKWEPaintbrushEnums::Subtract) )
    {
    if (auxData1)
      {
      // Enable delta computation for the first time to compute detlas w.r.t
      // what's already there for the Undo/Redo stuff
      auxData1->SetComputeDelta(1); 

      auxData1->Add(data);
      auxData1->SetComputeDelta(0); 
      }
    if (auxData2)
      {
      auxData2->Add(data);
      }
    this->PaintbrushData->Add(data); // TODO Was InternalAdd before.. check
    }
  else if(  (this->Internals->State == vtkKWEPaintbrushEnums::Erase &&
        op == vtkKWEPaintbrushEnums::Add)  
      ||
       (this->Internals->State == vtkKWEPaintbrushEnums::Draw &&
        op == vtkKWEPaintbrushEnums::Subtract) )
    {
    // This is bound to bite me later.. We need to have the stencilData
    // maintain a notion of a label map (at least a binary one). The deal is
    // that if you are using a StencilData, you need to add within a stroke
    // and subtract a negative stroke from your stroke manager... The point
    // being that for stencils, the stroke maintains the polarity. For the
    // grayscale data, the polarity is ingrained in the grayscale values 
    // emitted by the shape, so the shape (who generates the image data for a
    // shape at a given position) maintains the polarity. TODO I know, this 
    // is confusing.. I will rework this later
    // Stephen's comment: Seems like the following should be conditional on
    //   grayscaleData not stencilData
    if (vtkKWEPaintbrushStencilData::SafeDownCast(this->PaintbrushData))
      {
      if (auxData1)
        {
        // Enable delta computation for the first time to compute detlas w.r.t
        // what's already there for the Undo/Redo stuff
        auxData1->SetComputeDelta(1); 

        auxData1->Subtract(data);
        auxData1->SetComputeDelta(0);
        }
      if (auxData2)
        {
        auxData2->Subtract(data);
        }
      this->PaintbrushData->Add(data); 
      }
    else
      {
      this->PaintbrushData->Subtract(data);
      if (auxData1)
        {
        auxData1->Subtract(data);
        }
      if (auxData2)
        {
        auxData2->Subtract(data);
        }
      }
    }

  // Push the node position on the list. We use the node position to compare
  // and see if the shape moved since the last draw. This is done to avoid
  // multiple brush compositions at the same point, therby buying minor
  // speedups.
  vtkKWEPaintbrushStrokeNode *node = new vtkKWEPaintbrushStrokeNode;
  node->WorldPosition[0] = p[0];
  node->WorldPosition[1] = p[1];
  node->WorldPosition[2] = p[2];
  this->Internals->Node.push_back(node);

  this->DrawTime = data->GetMTime(); // Update the draw time.

  data->Delete();
  return 1;
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::Allocate()
{
  if (this->PaintbrushData)
    {
    vtkErrorMacro( 
      << "Error: vtkKWEPaintbrushStroke::Allocate called twice");
    }
  if (!this->ImageData)
    {
    vtkErrorMacro( << "Set the image data on which the stencil is drawn" );
    }
  
  if (this->Extent[1] < this->Extent[0])
    {
    this->ImageData->GetExtent(this->Extent);
    }
    
  double spacing[3],origin[3];
  this->ImageData->GetSpacing(spacing);
  this->ImageData->GetOrigin( origin );
  
  if (this->Representation == vtkKWEPaintbrushEnums::Binary)
    {
    this->PaintbrushData = vtkKWEPaintbrushStencilData::New();
    }
  else if (this->Representation == vtkKWEPaintbrushEnums::Grayscale)
    {
    this->PaintbrushData = vtkKWEPaintbrushGrayscaleData::New();
    }
  this->PaintbrushData->SetExtent(this->Extent);
  this->PaintbrushData->SetSpacing(spacing);
  this->PaintbrushData->SetOrigin(origin);

  if (this->GetState() == vtkKWEPaintbrushEnums::Erase)
    {
    // Start with an inverted buffer for a Minkowski "min" if we are
    // using grayscale data.
    this->PaintbrushData->Allocate( 
      this->Representation == vtkKWEPaintbrushEnums::Grayscale ? 255.0 : 0.0);
    }
  else
    {
    this->PaintbrushData->Allocate();
    }

  this->PaintbrushData->SetLabel(this->Label);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetState( int state )
{
  this->Internals->State = static_cast<vtkKWEPaintbrushEnums::BrushType>(state);
  if (!this->PaintbrushOperation)
    {
    vtkErrorMacro( << "Set the paintbrush operation first ");
    return;
    }
  vtkKWEPaintbrushShape *s = this->PaintbrushOperation->GetPaintbrushShape();
  if (!s)
    {
    vtkErrorMacro( << "Set the shape on the operation, prior to setting its "
                   << "polarity !");
    return;
    }
  s->SetPolarity(state);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetStateToDraw()
{
  this->SetState( vtkKWEPaintbrushEnums::Draw );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetStateToErase()
{
  this->SetState( vtkKWEPaintbrushEnums::Erase );
}

//----------------------------------------------------------------------
int vtkKWEPaintbrushStroke::GetState()
{
  return this->Internals->State;  
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetExtent( int e[6] )
{
  if (this->Extent[0] != e[0] ||
      this->Extent[1] != e[1] ||
      this->Extent[2] != e[2] ||
      this->Extent[3] != e[3] ||
      this->Extent[4] != e[4] ||
      this->Extent[5] != e[5])
    {
    this->Extent[0] = e[0];
    this->Extent[1] = e[1];
    this->Extent[2] = e[2];
    this->Extent[3] = e[3];
    this->Extent[4] = e[4];
    this->Extent[5] = e[5];
    if (this->PaintbrushData)
      {
      this->PaintbrushData->SetExtent(this->Extent);
      }
    this->Modified();
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::SetRepresentation( int r )
{
  if (r == vtkKWEPaintbrushEnums::Label)
    {
    // Strokes can never be label maps. One stroke can represent only one
    // unique label. ie You can never edit two labels in one swoosh.
    this->SetRepresentation( vtkKWEPaintbrushEnums::Binary );
    }

  if ( this->Representation != r && r != vtkKWEPaintbrushEnums::Label )
    {
    this->Representation = r;
    this->Modified();
    }
}

//----------------------------------------------------------------------
bool vtkKWEPaintbrushStroke::IsRecent( const vtkKWEPaintbrushStroke *a, 
                                    const vtkKWEPaintbrushStroke *b )
{
  return (const_cast<vtkKWEPaintbrushStroke*>(a)->GetDrawTime() <
          const_cast<vtkKWEPaintbrushStroke*>(b)->GetDrawTime());
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushStroke::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Extent: (" 
     << this->Extent[0] << "," << this->Extent[1] << ","
     << this->Extent[2] << "," << this->Extent[3] << "," 
     << this->Extent[4] << "," << this->Extent[5] << ")" << endl;
  os << indent << "ImageData: ";
  if (this->ImageData) 
    {
    os << indent << this->ImageData << endl;
    this->ImageData->PrintSelf(os, indent.GetNextIndent());
    }
  else 
    {
    os << indent << "None" << endl;
    }
  if (this->PaintbrushData) 
    {
    os << indent << this->PaintbrushData << endl;
    this->PaintbrushData->PrintSelf(os, indent.GetNextIndent());
    }
  else 
    {
    os << indent << "None" << endl;
    }
}

