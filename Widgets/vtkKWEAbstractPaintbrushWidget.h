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

// .NAME vtkKWEAbstractPaintbrushWidget - Abstract base class that enables the use of WidgetGroups.
// .SECTION Description
// The vtkKWEAbstractPaintbrushWidget is used to enable the use of a vtkKWEWidgetGroup
// by adding the appropriate friend declarations. Its role is trivial and is
// scheduled to be moved into vtkAbstractWidget. Until then we will keep this
// class around to avoid modifications to vtkAbstractWidget.

#ifndef __vtkKWEAbstractPaintbrushWidget_h
#define __vtkKWEAbstractPaintbrushWidget_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkAbstractWidget.h"

class vtkKWEPaintbrushRepresentation;
class vtkKWEPaintbrushAnnotationWidget;
class vtkKWEWidgetGroup;

class VTKEdge_WIDGETS_EXPORT vtkKWEAbstractPaintbrushWidget : public vtkAbstractWidget
{
  //BTX
  // TODO: Move this to the superclass.
  friend class vtkKWEWidgetGroup;
  //ETX
public:

  // Description:
  // Standard methods for a VTK class.
  vtkTypeRevisionMacro(vtkKWEAbstractPaintbrushWidget,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Enable garbage collection. There are ref counting cycles with vtkKWEWidgetGroup
  virtual void Register(vtkObjectBase* o);
  virtual void UnRegister(vtkObjectBase* o);

protected:
  vtkKWEAbstractPaintbrushWidget();
  ~vtkKWEAbstractPaintbrushWidget() {};

  vtkKWEWidgetGroup* WidgetGroup;

  // We need to break reference count loops. The WidgetGroup ref counts us
  // and we ref-count the WidgetGroup.
  virtual void ReportReferences(vtkGarbageCollector* collector);

private:
  vtkKWEAbstractPaintbrushWidget(const vtkKWEAbstractPaintbrushWidget&);  //Not implemented
  void operator=(const vtkKWEAbstractPaintbrushWidget&);  //Not implemented
};

#endif

