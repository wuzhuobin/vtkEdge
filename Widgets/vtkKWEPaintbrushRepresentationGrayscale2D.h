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

#ifndef __vtkKWEPaintbrushRepresentationGrayscale2D_h
#define __vtkKWEPaintbrushRepresentationGrayscale2D_h

#include "vtkKWEPaintbrushRepresentation2D.h"

class vtkActorCollection;
class vtkMapperCollection;
class vtkContourFilter;
class vtkCollection;
class vtkKWEPaintbrushSketch;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushRepresentationGrayscale2D
                                  : public vtkKWEPaintbrushRepresentation2D
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushRepresentationGrayscale2D *New();

  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushRepresentationGrayscale2D,
                       vtkKWEPaintbrushRepresentation2D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();

  // Description:
  // Methods required by vtkProp superclass.
  virtual int  RenderOverlay(vtkViewport *viewport);
  virtual int  RenderOpaqueGeometry(vtkViewport *viewport);
#if VTKEdge_VTK_VERSION_DATE > 20070305
  virtual int  RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual int  HasTranslucentPolygonalGeometry();
#else
  // Legacy method to support VTK source versions prior to 2007/03/05
  virtual int  RenderTranslucentGeometry(vtkViewport *viewport);
#endif

  // Description:
  // Get the actors maintained by the representation, that need to be rendered.
  virtual void GetActors(vtkPropCollection *);

  // Description:
  // Set/Get the isovalue for contours on the grayscale brush data
  vtkSetMacro( IsoValue, double );
  vtkGetMacro( IsoValue, double );

  // Description:
  // See superclass documentation
  virtual void SetPaintbrushDrawing( vtkKWEPaintbrushDrawing * );

  // Descirption:
  // Deep copy.. synchronizes states etc..
  virtual void DeepCopy(vtkWidgetRepresentation *r);

  // Description:
  // INTERNAL - Do not use
  virtual void InstallPipeline()   {}
  virtual void UnInstallPipeline() {}

protected:
  vtkKWEPaintbrushRepresentationGrayscale2D();
  ~vtkKWEPaintbrushRepresentationGrayscale2D();

  double                IsoValue;
  vtkCollection       * ContourFilters;
  vtkMapperCollection * ContourPolyDataMappers;
  vtkActorCollection  * ContourPolyDataActors;
  vtkCollection       * ExtractComponents;
  vtkCollection       * ImageClips;

private:
  vtkKWEPaintbrushRepresentationGrayscale2D(
      const vtkKWEPaintbrushRepresentationGrayscale2D&);  //Not implemented
  void operator=(const vtkKWEPaintbrushRepresentationGrayscale2D&); //Not implemented

  // Given an actor, it finds the sketch that its a rendition of.
  vtkKWEPaintbrushSketch * GetSketch( vtkActor * );
};

#endif

