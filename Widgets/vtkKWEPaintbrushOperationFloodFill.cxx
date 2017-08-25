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
#include "vtkKWEPaintbrushOperationFloodFill.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkKWEPaintbrushOperationFloodFill, "$Revision: 1774 $");
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

