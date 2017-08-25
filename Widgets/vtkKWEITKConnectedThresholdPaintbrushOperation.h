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
