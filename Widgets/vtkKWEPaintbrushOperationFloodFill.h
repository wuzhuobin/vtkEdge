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
