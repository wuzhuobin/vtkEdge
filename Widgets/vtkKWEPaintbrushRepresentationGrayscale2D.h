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

