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
// .NAME vtkKWEPaintbrushOperationFloodFill - 
// .SECTION Description
// .SECTION See Also

#ifndef __vtkKWEPaintbrushOperationFloodFill_h
#define __vtkKWEPaintbrushOperationFloodFill_h

#include "vtkKWEPaintbrushOperation.h"

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushOperationFloodFill 
                           : public vtkKWEPaintbrushOperation
{
public:
  
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushOperationFloodFill *New();
  
  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEPaintbrushOperationFloodFill, 
                                vtkKWEPaintbrushOperation);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkKWEPaintbrushOperationFloodFill();
  ~vtkKWEPaintbrushOperationFloodFill();

  virtual void DoOperationOnStencil(
      vtkImageStencilData *, int x, int y, int z);
  
private:
  vtkKWEPaintbrushOperationFloodFill(const vtkKWEPaintbrushOperationFloodFill&);  //Not implemented
  void operator=(const vtkKWEPaintbrushOperationFloodFill&);  //Not implemented
};

#endif
