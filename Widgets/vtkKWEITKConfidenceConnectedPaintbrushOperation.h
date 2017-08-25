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
// .NAME vtkKWEITKConfidenceConnectedPaintbrushOperation -
// .SECTION Description
// This is an abstract base class.
// .SECTION See Also

#ifndef __vtkKWEITKConfidenceConnectedPaintbrushOperation_h
#define __vtkKWEITKConfidenceConnectedPaintbrushOperation_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEITKPaintbrushOperation.h"

class vtkImageStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEITKConfidenceConnectedPaintbrushOperation
                                : public vtkKWEITKPaintbrushOperation
{
public:

  // Description:
  // Instantiate this class.
  static vtkKWEITKConfidenceConnectedPaintbrushOperation *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEITKConfidenceConnectedPaintbrushOperation,
                       vtkKWEITKPaintbrushOperation);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkKWEITKConfidenceConnectedPaintbrushOperation();
  ~vtkKWEITKConfidenceConnectedPaintbrushOperation();

  // Description:
  // See superclass Doc
  virtual void DoOperationOnStencil(vtkImageStencilData *, double p[3]);
  virtual void DoOperation( vtkKWEPaintbrushData *, double p[3],
                            vtkKWEPaintbrushEnums::OperationType & op );


private:
  vtkKWEITKConfidenceConnectedPaintbrushOperation(
    const vtkKWEITKConfidenceConnectedPaintbrushOperation&);  //Not implemented
  void operator=(const
      vtkKWEITKConfidenceConnectedPaintbrushOperation&);  //Not implemented
};

#endif
