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
#include "vtkKWEPaintbrushOperationFloodFill.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushOperationFloodFill, "$Revision: 590 $");
vtkStandardNewMacro(vtkKWEPaintbrushOperationFloodFill);

//----------------------------------------------------------------------
vtkKWEPaintbrushOperationFloodFill::vtkKWEPaintbrushOperationFloodFill()
{
}

//----------------------------------------------------------------------
vtkKWEPaintbrushOperationFloodFill::~vtkKWEPaintbrushOperationFloodFill()
{
}

//----------------------------------------------------------------------
void vtkKWEPaintbrushOperationFloodFill::
DoOperationOnStencil(vtkImageStencilData *stencilData, 
                                          int x, int y, int z)
{
  this->Superclass::DoOperationOnStencil( stencilData, x, y, z );

  if (!this->ImageData)
    {
    return;
    }

  // Now do the floodfill on the image data

}


//----------------------------------------------------------------------
void vtkKWEPaintbrushOperationFloodFill::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

