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
// .NAME vtkKWESurfaceLICDefaultPainter - vtkDefaultPainter replacement that
// inserts the vtkKWESurfaceLICPainter at the correct position in the painter
// chain.
// .SECTION Description
// vtkKWESurfaceLICDefaultPainter is a vtkDefaultPainter replacement
// that inserts the vtkKWESurfaceLICPainter at the correct position in the painter
// chain.

#ifndef __vtkKWESurfaceLICDefaultPainter_h
#define __vtkKWESurfaceLICDefaultPainter_h

#include "vtkDefaultPainter.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWESurfaceLICPainter;

class VTKEdge_RENDERING_EXPORT vtkKWESurfaceLICDefaultPainter : public vtkDefaultPainter
{
public:
  static vtkKWESurfaceLICDefaultPainter* New();
  vtkTypeRevisionMacro(vtkKWESurfaceLICDefaultPainter, vtkDefaultPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the Surface LIC painter.
  void SetSurfaceLICPainter(vtkKWESurfaceLICPainter*);
  vtkGetObjectMacro(SurfaceLICPainter, vtkKWESurfaceLICPainter);

//BTX
protected:
  vtkKWESurfaceLICDefaultPainter();
  ~vtkKWESurfaceLICDefaultPainter();

  // Description:
  // Setups the the painter chain.
  virtual void BuildPainterChain();

  // Description:
  // Take part in garbage collection.
  virtual void ReportReferences(vtkGarbageCollector *collector);

  vtkKWESurfaceLICPainter* SurfaceLICPainter;
private:
  vtkKWESurfaceLICDefaultPainter(const vtkKWESurfaceLICDefaultPainter&); // Not implemented.
  void operator=(const vtkKWESurfaceLICDefaultPainter&); // Not implemented.
//ETX
};

#endif


