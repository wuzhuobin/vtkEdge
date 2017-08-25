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

#include "vtkKWEAbstractPaintbrushWidget.h"

#include "vtkKWEWidgetGroup.h"
#include "vtkKWEPaintbrushWidgetCallbackMapper.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkObjectFactory.h"
#include "vtkGarbageCollector.h"

vtkCxxRevisionMacro(vtkKWEAbstractPaintbrushWidget, "$Revision: 3231 $");

//----------------------------------------------------------------------
vtkKWEAbstractPaintbrushWidget::vtkKWEAbstractPaintbrushWidget()
{
  this->WidgetGroup = NULL;
  vtkKWEWidgetGroup * w = vtkKWEWidgetGroup::New();
  w->AddWidget(this);
  w->Delete();
  this->Payload = NULL;
}

//----------------------------------------------------------------------
void vtkKWEAbstractPaintbrushWidget::Register( vtkObjectBase *o )
{
  // Enable garbage collection. There are ref counting cycles with vtkKWEWidgetGroup
  this->RegisterInternal(o, 1);
}

//----------------------------------------------------------------------
void vtkKWEAbstractPaintbrushWidget::UnRegister( vtkObjectBase *o )
{
  // Enable garbage collection. There are ref counting cycles with vtkKWEWidgetGroup
  this->UnRegisterInternal(o, 1);
}

//----------------------------------------------------------------------
void vtkKWEAbstractPaintbrushWidget::
SetCallbackMapper( vtkKWEPaintbrushWidgetCallbackMapper *o )
{
  if (o == this->CallbackMapper)
    {
    return;
    }

  // Enable garbage collection. There are ref counting cycles with vtkKWEWidgetGroup
  if (this->CallbackMapper)
    {
    this->CallbackMapper->SetEventTranslator(NULL);
    this->CallbackMapper->Delete();
    this->CallbackMapper = NULL;
    }
  this->CallbackMapper = o;
  if (o)
    {
    this->CallbackMapper->Register(this);
    o->SetPaintbrushWidget(this); // no ref couting here.
    o->Bindings();
    }
}

//----------------------------------------------------------------------------
void vtkKWEAbstractPaintbrushWidget::ReportReferences(vtkGarbageCollector* collector)
{
  this->Superclass::ReportReferences(collector);
  vtkGarbageCollectorReport(collector, this->WidgetGroup, "WidgetGroup");
}

//----------------------------------------------------------------------
void vtkKWEAbstractPaintbrushWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

