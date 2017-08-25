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

// .NAME vtkKWEPaintbrushSelectionRepresentation -
// .SECTION Description

#ifndef __vtkKWEPaintbrushSelectionRepresentation_h
#define __vtkKWEPaintbrushSelectionRepresentation_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkWidgetRepresentation.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushDrawing;
class vtkPointPlacer;
class vtkKWEPaintbrushSketch;
class vtkKWEPaintbrushSelectionWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushSelectionRepresentation 
                                       : public vtkWidgetRepresentation
{
  //BTX
  friend class vtkKWEPaintbrushSelectionWidget;
  //ETX                                                          
public:
  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro( vtkKWEPaintbrushSelectionRepresentation,
                        vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX -- States the representation can set itself into
  enum 
    {
    PaintbrushRequestSketchSelect,
    PaintbrushSketchSelect,
    PaintbrushSketchUnselect,
    PaintbrushRequestSketchMerge,
    PaintbrushSketchMerge,
    PaintbrushDisabled,
    None
   };
  //ETX

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual int  ComputeInteractionState(int X, int Y, int modifier=0);

  // Description:
  // Set the Paintbrush drawing
  virtual void SetPaintbrushDrawing( vtkKWEPaintbrushDrawing * );
  vtkGetObjectMacro( PaintbrushDrawing, vtkKWEPaintbrushDrawing );

  // Descirption:
  // Set / get the Point Placer. The point placer is responsible for 
  // converting display coordinates into world coordinates according 
  // to some constraints, and for validating world positions. 
  // By writing an appopriate point placer, you can disable/enable selection
  // on regions in world space.
  void SetPointPlacer( vtkPointPlacer * );
  vtkGetObjectMacro( PointPlacer, vtkPointPlacer );

  // Descirption:
  // Deep copy.. synchronizes states etc..
  virtual void DeepCopy(vtkWidgetRepresentation *);
  
  // Description:
  // Check if display point is inside the sketch "s".
  virtual int DisplayPositionIsInside( vtkKWEPaintbrushSketch *s, 
                                       double displayPos[2]);

  // Description:
  // Check if world point is inside the sketch "s".
  virtual int WorldPositionIsInside( vtkKWEPaintbrushSketch *s, 
                                     double worldPos[3]);

  // Description:
  // Check if display position is inside the drawing and return the first
  // drawing that falls within the display position. Returns NULL if the
  // position doesn't lie on any sketch in the drawing.
  //virtual vtkKWEPaintbrushSketch* DisplayPositionIsInside( double displayPos[2] );

  // Description:
  // Get the selected sketch.
  vtkGetObjectMacro( SelectedSketch, vtkKWEPaintbrushSketch );

  // Description:
  // Add sketch to the list of selected sketches.
  virtual void AddSketchToSelection( vtkKWEPaintbrushSketch * );

  // Description:
  // Remove sketch from the list of selected sketches.
  virtual void RemoveSketchFromSelection( vtkKWEPaintbrushSketch * );

  // Description:
  // Deletes selected sketches. Returns the number of sketches removed.
  virtual int DeleteSelectedSketches();

  // Description:
  // Select/UnSelect all the sketches in the drawing
  virtual void SelectAllSketches();
  virtual void UnSelectAllSketches();

  //BTX
  // Description:
  // Get the selected sketches.
  const vtkstd::vector< vtkKWEPaintbrushSketch * > & GetSelectedSketches()
    { return this->SelectedSketches; }
  //ETX

  // Description:
  // Merge the selected sketches.
  // You may specify a sketch, from the drawing, to merge into. If NULL, the 
  // first selected sketch will contain the merge.
  // By default sketches merged are removed. The removeSketches ivar can be
  // used to control this.
  virtual int MergeSelectedSketches( vtkKWEPaintbrushSketch *mergedSketch = NULL,
                                     int removeSketches = 1 );
  
protected:
  vtkKWEPaintbrushSelectionRepresentation();
  ~vtkKWEPaintbrushSelectionRepresentation();

  // Description:
  // Set the interaction state.
  vtkSetMacro( InteractionState, int );

  // Description:
  // Drag and drop merge support. Returns 1 if successful.
  virtual int DragAndDropMerge();
  
  vtkKWEPaintbrushDrawing        *PaintbrushDrawing;

  // Description:
  // The point placer determines the world coordinate based on screen 
  // coordinate and allows you to set constraints too.
  vtkPointPlacer             * PointPlacer;
  
  // Description:
  // Bookkeeping of the last display position.
  int    LastDisplayPosition[2]; 

  // Description:
  // The sketch just selected.
  vtkKWEPaintbrushSketch * SelectedSketch;

  // Description:
  // List of selected sketches.
  typedef vtkstd::vector< vtkKWEPaintbrushSketch * > SelectedSketchesType;
  SelectedSketchesType  SelectedSketches;
  int                   PrevInteractionState;
  double                DragBeginPoint[3], DragEndPoint[3];
  vtkKWEPaintbrushSketch * DragAndDropDestination;
  
private:
  vtkKWEPaintbrushSelectionRepresentation(const vtkKWEPaintbrushSelectionRepresentation&);  //Not implemented
  void operator=(const vtkKWEPaintbrushSelectionRepresentation&);  //Not implemented
};

#endif

