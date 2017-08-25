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
#include "vtkKWEWidgetGroup.h"

#include "vtkObjectFactory.h"
#include "vtkGarbageCollector.h"
#include "vtkKWEAbstractPaintbrushWidget.h" // REMOVE

vtkCxxRevisionMacro(vtkKWEWidgetGroup, "$Revision: 726 $");
vtkStandardNewMacro(vtkKWEWidgetGroup);

//----------------------------------------------------------------------
vtkKWEWidgetGroup::vtkKWEWidgetGroup()
{
}

//----------------------------------------------------------------------
vtkKWEWidgetGroup::~vtkKWEWidgetGroup()
{
  for (WidgetIteratorType it  = this->Widget.begin(); 
                          it != this->Widget.end()  ; ++it)
    {
    if (*it)
      {
      this->RemoveWidget( *it );
      (*it)->Delete();
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::Register( vtkObjectBase *o )
{
  // Enable garbage collection due to ref counting cycles with vtkAbstractWidget
  this->RegisterInternal(o, 1);
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::UnRegister( vtkObjectBase *o )
{
  // Enable garbage collection due to ref counting cycles with vtkAbstractWidget
  this->UnRegisterInternal(o, 1);
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::SetEnabled(int enabling)
{
  for (WidgetIteratorType it  = this->Widget.begin(); 
                          it != this->Widget.end()  ; ++it)
    {
    (*it)->SetEnabled(enabling);
    } 
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::AddWidget( vtkAbstractWidget *w )
{
  for ( unsigned int i = 0; i < this->Widget.size(); i++)
    {
    if (this->Widget[i] == w)
      {
      return;
      }
    }

  // TODO : Won't be necessary if we move this to the AbstractWidget.. superclass  
  vtkKWEAbstractPaintbrushWidget *obj=
    static_cast<vtkKWEAbstractPaintbrushWidget *>(w);
  
  // If the widget was attached to someother widget set, remove it from that.  
  if (vtkKWEWidgetGroup * otherWidgetGroup = obj->WidgetGroup)
    {
    otherWidgetGroup->RemoveWidget(w);
    }
    
  // Here is where we introduce the ref-counting cycle.
  w->Register(this);
  this->Register(w);
  
  this->Widget.push_back(w);
  obj->WidgetGroup = this;
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::RemoveWidget( vtkAbstractWidget * w)
{
  for (WidgetIteratorType it  = this->Widget.begin(); 
                          it != this->Widget.end()  ; ++it)
    {
    if (*it == w)
      {
      this->Widget.erase(it);
      static_cast<vtkKWEAbstractPaintbrushWidget *>(w)->WidgetGroup = NULL;
      w->UnRegister(this);
      this->UnRegister(w);
      break;
      }
    }
}

//----------------------------------------------------------------------
vtkAbstractWidget *
vtkKWEWidgetGroup::GetNthWidget( unsigned int i )
{
  return this->Widget[i];
}

//----------------------------------------------------------------------
unsigned int vtkKWEWidgetGroup::GetNumberOfWidgets()
{
  return static_cast< unsigned int >(this->Widget.size());
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::ReportReferences(vtkGarbageCollector* collector)
{
  const unsigned int n = this->GetNumberOfWidgets();
  if (n)
    {
    this->Superclass::ReportReferences(collector);
    for (unsigned int i = 0; i < n; i++)
      {
      vtkGarbageCollectorReport(collector, this->Widget[i], 
          "A widget from the WidgetGroup");    
      }
    }
}

//----------------------------------------------------------------------
void vtkKWEWidgetGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}

