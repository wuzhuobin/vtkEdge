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

// .NAME vtkKWEPaintbrushHighlightActors - 
// .SECTION Description
// Can be instantiated only by a vtkObject.

#ifndef __vtkKWEPaintbrushHighlightActors_h
#define __vtkKWEPaintbrushHighlightActors_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkProp3D.h"

class vtkActorCollection;
class vtkMapperCollection;
class vtkKWEStencilContourFilter;
class vtkCollection;
class vtkKWEPaintbrushDrawing;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushHighlightActors 
                                  : public vtkProp3D
{
  //BTX
  friend class vtkKWEPaintbrushRepresentation2D;
  //ETX
public:
  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushHighlightActors,
                       vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The drawing must be set. Our actors need to highlight something right ?
  virtual void SetPaintbrushDrawing( vtkKWEPaintbrushDrawing* );
  vtkGetObjectMacro( PaintbrushDrawing, vtkKWEPaintbrushDrawing );

  // Description:
  // Set / get extents
  vtkSetVector6Macro( Extent, int );
  vtkGetVector6Macro( Extent, int );

  // Description:
  // Methods required by vtkProp superclass.
  virtual int  RenderOpaqueGeometry(vtkViewport *viewport);

#if VTKEdge_VTK_VERSION_DATE > 20070305
  virtual int  RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual int  HasTranslucentPolygonalGeometry();
#else
  // Legacy method to support VTK source versions prior to 2007/03/05
  virtual int  RenderTranslucentGeometry(vtkViewport *viewport);
#endif
  
  // Description:
  // See vtkProp3D.
  virtual void GetActors(vtkPropCollection *);
  virtual double *GetBounds();
  
protected:
  vtkKWEPaintbrushHighlightActors();
  ~vtkKWEPaintbrushHighlightActors();

  // Description:
  // Can be instantiated only by a vtkObject.
  static vtkKWEPaintbrushHighlightActors *New();

  vtkCollection        * ContourFilters;
  vtkMapperCollection  * ContourPolyDataMappers;
  vtkActorCollection   * ContourPolyDataActors;
  vtkKWEPaintbrushDrawing * PaintbrushDrawing;
  int                    Extent[6];
  
  // Description:
  // Update and build all the actors according to the drawing.
  virtual void BuildRepresentation();
  
private:
  vtkKWEPaintbrushHighlightActors(
      const vtkKWEPaintbrushHighlightActors&);  //Not implemented
  void operator=(const vtkKWEPaintbrushHighlightActors&); //Not implemented
};

#endif

