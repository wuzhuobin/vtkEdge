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
// .NAME vtkKWEITKConnectedThresholdPaintbrushOperation -
// .SECTION Description
// This is an abstract base class.
// .SECTION See Also

#ifndef __vtkKWEITKConnectedThresholdPaintbrushOperation_h
#define __vtkKWEITKConnectedThresholdPaintbrushOperation_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEITKPaintbrushOperation.h"

class vtkImageStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEITKConnectedThresholdPaintbrushOperation 
                                : public vtkKWEITKPaintbrushOperation
{
public:
  
  // Description:
  // Instantiate this class.
  static vtkKWEITKConnectedThresholdPaintbrushOperation *New();
  
  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEITKConnectedThresholdPaintbrushOperation, 
                       vtkKWEITKPaintbrushOperation);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkKWEITKConnectedThresholdPaintbrushOperation();
  ~vtkKWEITKConnectedThresholdPaintbrushOperation();

  // Description:
  // See superclass Doc
  virtual void DoOperationOnStencil(vtkImageStencilData *, double p[3]);
  virtual void DoOperation( vtkKWEPaintbrushData *, double p[3],
                            vtkKWEPaintbrushEnums::OperationType & op );
  
private:
  vtkKWEITKConnectedThresholdPaintbrushOperation(
    const vtkKWEITKConnectedThresholdPaintbrushOperation&);  //Not implemented
  void operator=(const 
      vtkKWEITKConnectedThresholdPaintbrushOperation&);  //Not implemented
};

#endif
