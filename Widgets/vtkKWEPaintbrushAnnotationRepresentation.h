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

