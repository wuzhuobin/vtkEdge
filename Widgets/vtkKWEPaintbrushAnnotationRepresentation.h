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

// .NAME vtkKWEPaintbrushAnnotationRepresentation - An abstract base class for 2D/3D paintbrush widget representations
// .SECTION Description
// See vtkKWEPaintbrushAnnotationRepresentation2D for more details.
// The ShapePlacer must be set prior to use.
// 
// Before using the class, you must set the ImageActor on which the brush is
// being drawn.

#ifndef __vtkKWEPaintbrushAnnotationRepresentation_h
#define __vtkKWEPaintbrushAnnotationRepresentation_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkWidgetRepresentation.h"
#include <vtksys/stl/string>

class vtkTimeStamp;
class vtkKWEPaintbrushRepresentation;
class vtkActor2D;
class vtkTextMapper;
class vtkTextProperty;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushAnnotationRepresentation : public vtkWidgetRepresentation
{
  //BTX
  friend class vtkKWEPaintbrushRepresentation;
  //ETX
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushAnnotationRepresentation *New();

  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushAnnotationRepresentation,vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();

  // Description:
  // Methods required by vtkProp superclass.   
  virtual void ReleaseGraphicsResources(vtkWindow *w);
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
  // Override superclass method and propagate to all sub-props.
  virtual void SetVisibility(int);
  
  // Description:
  // Annotations will by default be placed whereever the brush currently is.
  // If you would like to have annotations appearing at a certain spot, you
  // can override this. The position is in Normalized display coords
  vtkSetVector2Macro( AnnotationPosition, double );
  vtkGetVector2Macro( AnnotationPosition, double );

  // Description:
  // Set/Get the string to render. This is automatically set from 
  // vtkKWEPaintbrushAnnotationWidget.
  vtkSetStringMacro( Annotation );
  vtkGetStringMacro( Annotation );
  
  // Description:
  // Get the text property used to render the annotation.
  vtkGetObjectMacro( TextProperty, vtkTextProperty );
  
protected:
  vtkKWEPaintbrushAnnotationRepresentation();
  ~vtkKWEPaintbrushAnnotationRepresentation();

  vtkTimeStamp                *BuildTime;
  vtkActor2D                  *TextActor;
  vtkTextProperty             *TextProperty;
  vtkTextMapper               *TextMapper;  
  char                        *Annotation;
  vtkKWEPaintbrushRepresentation *PaintbrushRepresentation;
  vtksys_stl::string           LastDisplayedAnnotation;
  int                          LastDisplayPosition[2];
  double                       AnnotationPosition[2];
  
private:
  vtkKWEPaintbrushAnnotationRepresentation(const vtkKWEPaintbrushAnnotationRepresentation&);  //Not implemented
  void operator=(const vtkKWEPaintbrushAnnotationRepresentation&);  //Not implemented
};

#endif

