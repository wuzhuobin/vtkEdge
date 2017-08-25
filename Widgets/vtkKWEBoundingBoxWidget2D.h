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
// .NAME vtkKWEBoundingBoxWidget2D - Manages a 3D bounding box on a 2D image actor
// .SECTION Description
// The widget can be placed on one of the orthogonal (axial/sagittal/coronal)
// image actors and be used to specify bounds (or extents). The widget
// includes a slice scale (with 2 handles) to manipulate the extents along Z
// and 4 handles to manipulate the bounding box.
//
// If bounds are changed, the widget invokes a BoundsChangedEvent along with
// a pointer to 6 tuple double containing the new bounds.
//
// If extents are changed, the widget invokes a ExtentsChangedEvent along with
// a pointer to 6 tuple integer containing the new extent.
//
// Needless to say, a BoundsChangedEvent will be invoked every time an
// ExtentsChangedEvent is invoked. The converse is not always true.

#ifndef __vtkKWEBoundingBoxWidget2D_h
#define __vtkKWEBoundingBoxWidget2D_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkAbstractWidget.h"

class vtkKWEBoundingBoxRepresentation2D;
class vtkHandleWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEBoundingBoxWidget2D : public vtkAbstractWidget
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEBoundingBoxWidget2D *New();

  // Description:
  // Standard methods for a VTK class.
  vtkTypeRevisionMacro(vtkKWEBoundingBoxWidget2D,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The method for activiating and deactiviating this widget. This method
  // must be overridden because it is a composite widget and does more than
  // its superclass' vtkAbstractWidget::SetEnabled() method.
  virtual void SetEnabled(int);

  // Description:
  // See superclass doc
  virtual void SetProcessEvents( int );

  // Description:i
  // This serves to fade the representation into the background. If faded,
  // ProcessEvents is automatically turned off, ie the widget will not
  // respond to events.
  virtual void SetFade( int );
  virtual int GetFade();
  vtkBooleanMacro(Fade, int);

  // Description:
  // Events emitted by the widget
  //BTX
  enum
  {
    BeginResizeEvent = 1000,
    ResizingEvent,
    EndResizeEvent,
    BoundsChangedEvent,
    ExtentsChangedEvent
  };
  //ETX

  // Description:
  // Set the representation
  virtual void SetRepresentation( vtkKWEBoundingBoxRepresentation2D * );

  // Description:
  // See superclass for documentation
  virtual void CreateDefaultRepresentation();

  // Description:
  // If set, it must be set prior to enabling the widget. Default is ON.
  virtual void SetShowSliceScaleBar(int);
  vtkGetMacro( ShowSliceScaleBar, int );
  vtkBooleanMacro( ShowSliceScaleBar, int );

protected:
  vtkKWEBoundingBoxWidget2D();
  ~vtkKWEBoundingBoxWidget2D();

  // Description:
  static void OnMouseMoveCallback     ( vtkAbstractWidget* );
  static void OnLeftButtonDownCallback  ( vtkAbstractWidget* );
  static void OnLeftButtonUpCallback  ( vtkAbstractWidget* );
  void SetCursor( int );

  int ShowSliceScaleBar;
  void SetEnabledStateOfSliceScaleBar();

  double LastComputedBounds[6];
  int    LastComputedExtent[6];

private:
  vtkKWEBoundingBoxWidget2D(const vtkKWEBoundingBoxWidget2D&);  //Not implemented
  void operator=(const vtkKWEBoundingBoxWidget2D&);  //Not implemented

  vtkHandleWidget ** HandleWidgets;
};

#endif

