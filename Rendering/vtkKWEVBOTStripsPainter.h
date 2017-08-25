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

// .NAME vtkVBOLinesPainter - this painter paints triangle strips using vertex
// buffer objects to increase data transfer speeds.

// .SECTION Description
// This painter tries to paint triangle strips efficiently.  Data are
// transfered in bulk to vertex buffer objects and then rendered from
// these buffers.  If vertex buffer objects are not supported, or if
// they cannot be allocated, then this renderer cannot render the
// primitive and will propagate the request to the delegate painter.
// Requests to Render any other primitive are ignored and not passed
// to the delegate painter, if any. If this class is able to render
// the primitive, the render request is not propagated to the delegate
// painter. Cell attributes are not supported. 

#ifndef __vtkKWEVBOTStripsPainter_h
#define __vtkKWEVBOTStripsPainter_h

#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkTStripsPainter.h"

class vtkKWEVertexBufferObject;
class vtkRenderWindow;

class VTKEdge_RENDERING_EXPORT vtkKWEVBOTStripsPainter : public vtkTStripsPainter
{
public:
  static vtkKWEVBOTStripsPainter* New();
  vtkTypeRevisionMacro(vtkKWEVBOTStripsPainter, vtkTStripsPainter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Delete VBOs 
  virtual void ReleaseGraphicsResources(vtkWindow* w);
//BTX
protected:
  vtkKWEVBOTStripsPainter();
  ~vtkKWEVBOTStripsPainter();
  
  // Description:
  // The actual rendering happens here. VBOs are generated if
  // necessary, data is copied to the VBOs, and the primitive is drawn.
  virtual int RenderPrimitive(unsigned long flags, vtkDataArray* n,
                              vtkUnsignedCharArray* c, vtkDataArray* t, 
                              vtkRenderer* ren);

  // Description:
  // Copy data (points, normals, colors, tcoords, indices) to the
  // VBOs.  Calls GenerateVBOs to make sure that the VBOs have not
  // been generated.  Returns nonzero if all data was updated
  // successfully.  It is probably a good idea to call
  // ReleaseGraphicsResources if this call is not successful.
  virtual bool UpdateGraphicsResources(unsigned long idx, vtkDataArray* n,
                                      vtkUnsignedCharArray* c, 
                                      vtkDataArray* t, 
                                      vtkRenderer* ren);

  vtkKWEVertexBufferObject* PointIDs;
  vtkKWEVertexBufferObject* PointCoordinates;
  vtkKWEVertexBufferObject* PointColors;
  vtkKWEVertexBufferObject* PointNormals;
  vtkKWEVertexBufferObject* PointTCords;

  vtkWeakPointer<vtkRenderWindow> LastContext;
private:
  vtkKWEVBOTStripsPainter(const vtkKWEVBOTStripsPainter&); // Not implemented.
  void operator=(const vtkKWEVBOTStripsPainter&); // Not implemented.

//ETX
};

#endif
