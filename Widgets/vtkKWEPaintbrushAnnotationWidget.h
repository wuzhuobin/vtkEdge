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

// .NAME vtkKWEPaintbrushAnnotationWidget
// .SECTION Description
// .SECTION See Also
// vtkKWEPaintbrushRepresentation, vtkKWEPaintbrushRepresentation2D, vtkKWEPaintbrushSketch,
// vtkKWEPaintbrushShape.
//
#ifndef __vtkKWEPaintbrushAnnotationWidget_h
#define __vtkKWEPaintbrushAnnotationWidget_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkAbstractWidget.h"

class vtkKWEPaintbrushAnnotationRepresentation;
class vtkKWEPaintbrushWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushAnnotationWidget : public vtkAbstractWidget
{
  //BTX
  friend class vtkKWEPaintbrushWidget;
  //ETX
public:

  // Description:
  // Standard VTK methods.
  static vtkKWEPaintbrushAnnotationWidget *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushAnnotationWidget,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Annotation methods.
  void AnnotatePaintbrushMode();
  void AnnotateNewSketch();
  void AnnotateIncrementSketch();
  void AnnotateDecrementSketch();
  void AnnotateSize();
  void AnnotateString( const char * );

protected:
  vtkKWEPaintbrushAnnotationWidget();
  ~vtkKWEPaintbrushAnnotationWidget();

  // The widget can be in any of the following states.
//BTX
  enum { Start = 0, BeginTimer, EndTimer, Timing, TimedOut };
//ETX

  // Description:
  // Set the widget state. You must set it to either
  // BeginTimer or EndTimer only.
  virtual void SetWidgetState(int);
  vtkGetMacro( WidgetState, int );

  // Description:
  // The method for activiating and deactiviating this widget.
  virtual void SetEnabled(int);

  // Description:
  // Callback interface to tie ends with the CallbackMapper for the events
  // invoked by this widget.
  static void MoveCallback                     ( vtkAbstractWidget* );
  static void HoverCallback                    ( vtkAbstractWidget* );

  // Description:
  // Method to satisfy the superclass API.
  // Create the default vtkKWEPaintbrushRepresentation if one is not set.
  virtual void CreateDefaultRepresentation();

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkKWEPaintbrushAnnotationRepresentation *r);

private:
  vtkKWEPaintbrushAnnotationWidget(const vtkKWEPaintbrushAnnotationWidget&);  //Not implemented
  void operator=(const vtkKWEPaintbrushAnnotationWidget&);  //Not implemented

  // Description:
  int                   Movement;
  int                   WidgetState;
  int                   TimerId;
  int                   TimerDuration;
  vtkKWEPaintbrushWidget * PaintbrushWidget;
};

#endif
