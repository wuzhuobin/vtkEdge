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
// .NAME vtkKWEBoundingBoxRepresentation2D 
// .SECTION Description

#ifndef __vtkKWEBoundingBoxRepresentation2D_h
#define __vtkKWEBoundingBoxRepresentation2D_h

#include "VTKEdgeConfigure.h" // Needed for export symbols
#include "vtkWidgetRepresentation.h"

class vtkHandleRepresentation;
class vtkCellArray;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextMapper;
class vtkTextProperty;
class vtkProperty;
class vtkActor2D;
class vtkActor;
class vtkProperty2D;
class vtkProperty;
class vtkImageActorPointPlacer;
class vtkImageActor;
class vtkActor2DCollection;
class vtkCollection;
class vtkImageData;

class VTKEdge_WIDGETS_EXPORT vtkKWEBoundingBoxRepresentation2D : public vtkWidgetRepresentation
{
public:
  // Description:
  // Instantiate the class.
  static vtkKWEBoundingBoxRepresentation2D *New();

  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEBoundingBoxRepresentation2D,vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the image actor on which this widget is placed.
  virtual void SetImageActor( vtkImageActor * );
  vtkImageActor *GetImageActor();

  // Description:
  // This method is used to specify the type of handle representation to use
  // for the four internal vtkHandleRepresentations within
  // vtkKWEBoundingBoxRepresentation2D.  To use this method, create a dummy
  // vtkHandleRepresentation (or subclass), and then invoke this method with
  // this dummy. Then the vtkKWEBoundingBoxRepresentation2D uses this dummy to
  // clone four vtkHandleRepresentations of the same type.
  void SetHandleRepresentation(vtkHandleRepresentation *handle);
  vtkHandleRepresentation* GetHandleRepresentation( int handleIndex );

  // Description:
  // Retrieve the property used to control the appearance of the two
  // orthogonal lines.
  vtkGetObjectMacro(LineProperty,vtkProperty);
  vtkGetObjectMacro(SelectedLineProperty,vtkProperty);
  virtual void SetSelectedHandleProperty( vtkProperty * );
  virtual void SetHoveredHandleProperty( vtkProperty * );
  virtual void SetHandleProperty( vtkProperty * );

  // Description:
  // Get the 3D bounding-box as a polydata.
  vtkPolyData * GetPolyData();

  // Description:
  // Set/Get the extents of the bbox. Returns 0 on failure.
  int GetExtent( int extents[6] );
  void SetExtent( int extents[6] );

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();
  virtual int ComputeInteractionState(int X, int Y, int modify=0);
  virtual void GetActors2D(vtkPropCollection *pc);
  virtual void GetActors(vtkPropCollection *pc);
  virtual void PlaceWidget(double bounds[6]);

  // Description:
  // Methods required by vtkProp superclass.
  virtual void ReleaseGraphicsResources(vtkWindow *w);
  virtual int RenderOverlay(vtkViewport *viewport);
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);

  // Description:
  // INTERNAL. Do not use.
  // This is set from vtkKWEBoundingBoxWidget.
  vtkSetMacro(InteractionState,int);

  //BTX - manage the state of the widget
  enum _InteractionState
  {
    Outside = 0,
    Inside,
    RequestResizeBox,
    Resizing,
    BeginDefining,
    Defining,
    EndDefining
  };
  //ETX
  
  // Description:
  // Specify the format to use for labelling the distance. Note that an empty
  // string results in no label, or a format string without a "%" character
  // will not print the distance value.
  vtkSetStringMacro(LabelFormat);
  vtkGetStringMacro(LabelFormat);  
  
  // Description:
  // Set the image data on which the widget is placed. 
  virtual void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Description:
  // Optionally an ID field may be displayed next to the box
  vtkSetMacro( ID, int );
  vtkGetMacro( ID, int );

  // Description:
  // Get the outline property.
  vtkProperty * GetOutlineProperty();

  // Description:
  // Get the property of the text annotations
  vtkTextProperty * GetWidthTextProperty();
  vtkTextProperty * GetHeightTextProperty();
  vtkTextProperty * GetIDTextProperty();

  // Description:
  // INTERNAL: Do not use.. Must be set by the widget alone.
  // This serves to fade the representation into the background. When on, 
  // the representation will but also show up only with the rectangular 
  // bounding box. The handles and the slice actors don't show up anymore.
  vtkSetMacro( Fade, int );
  vtkGetMacro( Fade, int );
  
  // Description:
  // INTERNAL: Do not use.. Must be set by the widget alone.
  vtkSetMacro( ShowSliceScaleBar, int );

  // Description:
  // INTERNAL: Do not use
  vtkGetMacro( Unplaced, int );

protected:
  vtkKWEBoundingBoxRepresentation2D();
  ~vtkKWEBoundingBoxRepresentation2D();

  // Description:
  // On/Off the visibility of the widget.
  void SetVisibilityInternal( int );
    
  void Translate( const double translation[3], 
                  const double cursorMotionVector[3] );
  void PositionHandles();

  // Highlight the nth handle. Called with an argument of "-1" will set the 
  // property to all 4 handles.
  void SetHandleHighlight( int handleIdx, vtkProperty *property );
  
  // Default place the widget according to the bounds of the image actor
  void PlaceWidget();
  
  // User specified bounds to place the widget.
  int PlaceWidgetInternal(double bounds[6]);

  // Cut "PolyData" to produce "BoxPolyData". The cut will be done by using the 
  // camera parameters. (using the focal point and the normal). 
  // Returns 1 if "BoxPoints" is different from what it used to be.
  int Cut();

  // Compute annotations, if needed.. such as text width, height etc..
  int Annotate();
  int AnnotateScaleBar();
  
  vtkImageActorPointPlacer * PointPlacer;
  vtkPolyData              * PolyData;     // 8 Points
  vtkCollection            * TextMappers;
  vtkActor2DCollection     * TextActors;
  vtkPolyData              * BoxPolyData;
  vtkPolyDataMapper        * BoxMapper;
  vtkActor                 * BoxActor;
  vtkPoints                * BoxPoints;    // 4 Points, a "cut" of PolyData
  vtkProperty              * HandleProperty;
  vtkProperty              * SelectedHandleProperty;
  vtkProperty              * HoveredHandleProperty;
  vtkProperty              * LineProperty;
  vtkProperty              * SelectedLineProperty;
  vtkHandleRepresentation  * HandleRepresentation;
  vtkHandleRepresentation  **HandleRepresentations;
  int                        CurrentHandleIdx;
  double                     MinimumThickness[3];
  double                     PlaceFactor;
  double                     LastEventPosition[2];
  char                     * LabelFormat;
  vtkActor                 * ScaleBarActor;
  vtkActor                 * ScaleBarActor2;
  vtkPolyData              * ScaleBar;
  vtkPolyData              * ScaleBar2;
  int                        ShowSliceScaleBar;
  int                        Unplaced;
  vtkImageData             * ImageData;
  int                        Fade;
  int                        ID;
  
private:
  vtkKWEBoundingBoxRepresentation2D(const vtkKWEBoundingBoxRepresentation2D&);  //Not implemented
  void operator=(const vtkKWEBoundingBoxRepresentation2D&);  //Not implemented

  int  ComputeOrientation();
  int XAxis, YAxis, ZAxis;
  double Point1DisplayPosition[2], Point2DisplayPosition[2];
};

#endif

