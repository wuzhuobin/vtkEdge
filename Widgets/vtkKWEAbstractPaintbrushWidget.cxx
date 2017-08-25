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

#include "vtkKWEAbstractPaintbrushWidget.h"

#include "vtkKWEWidgetGroup.h"
#include "vtkObjectFactory.h"
#include "vtkGarbageCollector.h"

vtkCxxRevisionMacro(vtkKWEAbstractPaintbrushWidget, "$Revision: 590 $");

//----------------------------------------------------------------------
vtkKWEAbstractPaintbrushWidget::vtkKWEAbstractPaintbrushWidget()
{
  this->WidgetGroup = NULL;
  vtkKWEWidgetGroup * w = vtkKWEWidgetGroup::New();
  w->AddWidget(this);
  w->Delete();
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

