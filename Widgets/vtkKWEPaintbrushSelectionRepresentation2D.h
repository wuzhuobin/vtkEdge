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
// .NAME vtkKWEPaintbrushSelectionRepresentation2D - 
// .SECTION Description
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushSelectionRepresentation2D_h
#define __vtkKWEPaintbrushSelectionRepresentation2D_h

#include "vtkKWEPaintbrushSelectionRepresentation.h"

class vtkImageActor;
class vtkActor;
class vtkPolyData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushSelectionRepresentation2D
                              : public vtkKWEPaintbrushSelectionRepresentation
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEPaintbrushSelectionRepresentation2D *New();

  // Description:
  // Standard VTK methods.
  vtkTypeRevisionMacro(vtkKWEPaintbrushSelectionRepresentation2D,
                       vtkKWEPaintbrushSelectionRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // These are methods that satisfy vtkWidgetRepresentation's API.
  virtual void BuildRepresentation();

  // Description:
  // Set the image actor on which the paintbrush is drawn.
  virtual void SetImageActor( vtkImageActor * );

  // Descirption:
  // Deep copy.. synchronizes states etc..
  virtual void DeepCopy(vtkWidgetRepresentation *r);

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
  
protected:
  vtkKWEPaintbrushSelectionRepresentation2D();
  ~vtkKWEPaintbrushSelectionRepresentation2D();

  vtkActor          * DragActor;
  vtkPolyData       * DragPolyData;

  // Description:
  // Drag and drop merge support.
  virtual int DragAndDropMerge();

private:
  vtkKWEPaintbrushSelectionRepresentation2D(const vtkKWEPaintbrushSelectionRepresentation2D&);  //Not implemented
  void operator=(const vtkKWEPaintbrushSelectionRepresentation2D&);  //Not implemented
};

#endif
