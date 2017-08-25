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

