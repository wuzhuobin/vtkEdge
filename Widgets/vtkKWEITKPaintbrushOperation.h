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
// .NAME vtkKWEITKPaintbrushOperation -
// .SECTION Description
// This is an abstract base class.
// .SECTION See Also

#ifndef __vtkKWEITKPaintbrushOperation_h
#define __vtkKWEITKPaintbrushOperation_h

#include "vtkKWEPaintbrushOperation.h"
#include "vtkKWEITKImageToStencilFilter.h"

typedef double  RealType;

class vtkImageStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEITKPaintbrushOperation 
                                : public vtkKWEPaintbrushOperation
{
public:
  
  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEITKPaintbrushOperation, vtkKWEPaintbrushOperation);
  void PrintSelf(ostream& os, vtkIndent indent);
  
//BTX
  typedef vtkitk::vtkKWEITKImageToStencilFilter InternalFilterType;
  InternalFilterType::Pointer InternalFilter;
//ETX

  vtkSetVector3Macro( FilterHalfWidth, double );
  vtkGetVector3Macro( FilterHalfWidth, double );

protected:
  vtkKWEITKPaintbrushOperation();
  ~vtkKWEITKPaintbrushOperation();

  double FilterHalfWidth[3];

  // Description:
  // Filter the incoming data (first arg) through this operation. The 
  // operation is centered at point 'p'.
  virtual void DoOperationOnStencil(vtkImageStencilData *, double p[3]) = 0;

private:
  vtkKWEITKPaintbrushOperation(const vtkKWEITKPaintbrushOperation&);  //Not implemented
  void operator=(const vtkKWEITKPaintbrushOperation&);  //Not implemented
};

#endif
