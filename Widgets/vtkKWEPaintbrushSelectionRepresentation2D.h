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

