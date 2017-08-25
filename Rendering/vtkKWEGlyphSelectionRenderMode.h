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
// .NAME vtkKWEGlyphSelectionRenderMode - Perform Glyph selection.
// .SECTION Description
// Bypass vtkRenderer::Render to perform glyph selection with color buffer.
// .SECTION See Also
// vtkKWEGlyph3DMapper,  vtkVisibleCellSelector

#ifndef __vtkKWEGlyphSelectionRenderMode_h
#define __vtkKWEGlyphSelectionRenderMode_h

#include "vtkRendererDelegate.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_RENDERING_EXPORT vtkKWEGlyphSelectionRenderMode : public vtkRendererDelegate
{
public:
  vtkTypeRevisionMacro(vtkKWEGlyphSelectionRenderMode, vtkRendererDelegate);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkKWEGlyphSelectionRenderMode *New();

  // Description:
  // Render the props of vtkRenderer if Used is on.
  virtual void Render(vtkRenderer *r);
  
  // Description:
  // Called by vtkKWEVisibleGlyphSelector.
  vtkSetMacro(SelectMode, int);
  vtkSetMacro(SelectConst, unsigned int);

protected:
  vtkKWEGlyphSelectionRenderMode();
  virtual ~vtkKWEGlyphSelectionRenderMode();

  void UpdateCamera(vtkRenderer *r);
  void UpdateGeometry(vtkRenderer *r);

  int SelectMode;
  unsigned int SelectConst;

private:
  vtkKWEGlyphSelectionRenderMode(const vtkKWEGlyphSelectionRenderMode&);  // Not implemented.
  void operator=(const vtkKWEGlyphSelectionRenderMode&);  // Not implemented.
};

#endif
