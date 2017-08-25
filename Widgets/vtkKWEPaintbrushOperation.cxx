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
#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEPaintbrushShapeEllipsoid.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushOperation, "$Revision: 742 $");
vtkStandardNewMacro(vtkKWEPaintbrushOperation);
vtkCxxSetObjectMacro(vtkKWEPaintbrushOperation,PaintbrushShape,vtkKWEPaintbrushShape);
vtkCxxSetObjectMacro(vtkKWEPaintbrushOperation,ImageData,vtkImageData);

//----------------------------------------------------------------------
vtkKWEPaintbrushOperation::vtkKWEPaintbrushOperation()
{
  this->ImageData          = NULL;  
  this->PaintbrushShape    = NULL;
  this->Extent[0] = this->Extent[2] = this->Extent[4] = 0;
  this->Extent[1] = this->Extent[3] = this->Extent[5] = -1;

  // Set a default shape
  vtkKWEPaintbrushShapeEllipsoid * paintbrushShape = vtkKWEPaintbrushShapeEllipsoid::New(); 
  this->SetPaintbrushShape( paintbrushShape );
  paintbrushShape->SetWidth(5.0,5.0,5.0);
  paintbrushShape->Delete();  
}

//----------------------------------------------------------------------
vtkKWEPaintbrushOperation::~vtkKWEPaintbrushOperation()
{
  this->SetPaintbrushShape(NULL);
  this->SetImageData(NULL);
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushOperation::
GetPaintbrushData(vtkKWEPaintbrushData *paintbrushData, double p[3],
                  vtkKWEPaintbrushEnums::OperationType & op )
{
  if (!this->PaintbrushShape)
    {
    vtkErrorMacro( << "Set the shape prior to use" );
    }
  
  // Do the operation only if the paintbrush shape (whose center is denoted
  // by "p" is within the bounding extents) or of no bounding extents have 
  // been specified.
 
  bool isWithinBounds = false;
  bool uninitializedExtent = (this->Extent[0] > this->Extent[1]);
  if (!uninitializedExtent)
    {
    double spacing[3], origin[3];
    this->PaintbrushShape->GetSpacing(spacing);
    this->PaintbrushShape->GetOrigin(origin);
    
    double bounds[6] = 
      { this->Extent[0] * spacing[0] + origin[0],
        this->Extent[1] * spacing[0] + origin[0],
        this->Extent[2] * spacing[1] + origin[1],
        this->Extent[3] * spacing[1] + origin[1],
        this->Extent[4] * spacing[2] + origin[2],
        this->Extent[5] * spacing[2] + origin[2]  };
    
    double tolerance[3] = {1e-10, 1e-10, 1e-10};
    if (vtkMath::PointIsWithinBounds( p, bounds, tolerance ))
      {
      isWithinBounds = true;
      }
    }
  
  if (isWithinBounds || uninitializedExtent)
    {
    // Perform the operation.
    this->DoOperation(paintbrushData, p, op);
    
    // Clip the resulting paintbrushdata, if bounding extents have been 
    // specified.
    
    if (!uninitializedExtent )
      {
      paintbrushData->Clip( this->Extent );
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushOperation::
DoOperation( vtkKWEPaintbrushData *data, double p[3],
             vtkKWEPaintbrushEnums::OperationType & op )
{
  op = vtkKWEPaintbrushEnums::Add;

  // The default, pass through operation, simply pulls data from the shape.
  this->PaintbrushShape->GetPaintbrushData( data, p );
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushOperation::DeepCopy(vtkKWEPaintbrushOperation *op)
{
  if (op == this || !op)
    {
    return;
    }

  if (this->ImageData != op->ImageData) // operations on different image data
                                        // may be different.
    {
    return;
    }

  if (this->PaintbrushShape && op->PaintbrushShape)
    {
    this->PaintbrushShape->DeepCopy(op->PaintbrushShape);
    }
  this->Modified();
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushOperation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

