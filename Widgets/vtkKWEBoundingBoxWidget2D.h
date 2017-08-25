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

