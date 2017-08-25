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
class vtkKWEPaintbrushWidgetCallbackMapper;

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

  // Description:
  // Set the mouse and keyboard bindings through a callback mapper. If you want to 
  // set your own bindings, you would create your own subclass of 
  // vtkKWEPaintbrushWidgetCallbackMapper and override the Bindings() method.
  virtual void SetCallbackMapper( vtkKWEPaintbrushWidgetCallbackMapper * );
  vtkGetObjectMacro( CallbackMapper, vtkWidgetCallbackMapper );

  // Description:
  // Get the widget group that contains this paintbrush widget
  vtkGetObjectMacro( WidgetGroup, vtkKWEWidgetGroup );

protected:
  vtkKWEAbstractPaintbrushWidget();
  ~vtkKWEAbstractPaintbrushWidget() {};

  vtkKWEWidgetGroup* WidgetGroup;

  // We need to break reference count loops. The WidgetGroup ref counts us
  // and we ref-count the WidgetGroup.
  virtual void ReportReferences(vtkGarbageCollector* collector);

  void * Payload;

private:
  vtkKWEAbstractPaintbrushWidget(const vtkKWEAbstractPaintbrushWidget&);  //Not implemented
  void operator=(const vtkKWEAbstractPaintbrushWidget&);  //Not implemented
};

#endif

