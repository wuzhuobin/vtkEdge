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
