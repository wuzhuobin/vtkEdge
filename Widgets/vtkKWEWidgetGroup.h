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
// .NAME vtkKWEWidgetGroup - Synchronize a collection on vtkWidgets drawn on different renderwindows using the Callback - Dispatch Action mechanism.
//
// .SECTION Description
// The class synchronizes a set of vtkAbstractWidget(s). Widgets typically
// invoke "Actions" that drive the geometry/behaviour of their representations
// in response to interactor events. Interactor interactions on a render window
// are mapped into "Callbacks" by the widget, from which "Actions" are
// dispatched to the entire set. This architecture allows us to tie widgets
// existing in different render windows together. For instance a HandleWidget
// might exist on the sagittal view. Moving it around should update the
// representations of the corresponding handle widget that lies on the axial
// and coronal and volume views as well.
//
// .SECTION User API
// A user would use this class as follows.
// \code
// vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();
// vtkKWEPaintbrushWidget *w1 = vtkKWEPaintbrushWidget::New();
// set->AddWidget(w1);
// w1->SetInteractor(axialRenderWindow->GetInteractor());
// vtkKWEPaintbrushWidget *w2 = vtkKWEPaintbrushWidget::New();
// set->AddWidget(w2);
// w2->SetInteractor(coronalRenderWindow->GetInteractor());
// vtkKWEPaintbrushWidget *w3 = vtkKWEPaintbrushWidget::New();
// set->AddWidget(w3);
// w3->SetInteractor(sagittalRenderWindow->GetInteractor());
// set->SetEnabled(1);
// \endcode
//
// .SECTION Motivation
// The motivation for this class is really to provide a usable API to tie
// widgets drawn on multiple render windows. To enable this, subclasses
// of vtkAbstractWidget, must be written as follows:
//   They will generally have callback methods mapped to some user
// interaction such as:
// \code
// this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
//                       vtkEvent::NoModifier, 0, 0, NULL,
//                       vtkKWEPaintbrushWidget::ResizeEvent,
//                       this, vtkKWEPaintbrushWidget::ResizeCallback);
// \endcode
//   The callback invoked when the right button is pressed looks like:
// \code
// void vtkKWEPaintbrushWidget::ResizeCallback(vtkAbstractWidget *w)
// {
//   vtkKWEPaintbrushWidget *self = vtkKWEPaintbrushWidget::SafeDownCast(w);
//   self->WidgetGroup->DispatchAction(self, &vtkKWEPaintbrushWidget::ResizeAction);
// }
// \endcode
//   The actual code for resizing is written in the ResizeAction method.
// \code
// void vtkKWEPaintbrushWidget::ResizeAction( vtkKWEPaintbrushWidget *dispatcher)
// {
//   // Here dispatcher is the widget that was interacted with, the one that
//   // dispatched an action to all the other widgets in its group. You may, if
//   // necessary find it helpful to get parameters from it.
//   if (this == dispatcher)
//     {
//     this->WidgetRep->Resize( eventPos, lastEventPos );
//
//     // The active widget can use the return value to signify an abort.
//     // The other widgets in the set won't have the action dispatched to
//     // them at all.
//     if (newSize == oldSize) return 0;
//     }
//   else
//     {
//     double *newsize = dispatcher->GetRepresentation()->GetSize();
//     this->WidgetRep->SetSize(newsize);
//     }
// }
// \endcode
//
// .SECTION Caveats
// Actions are always dispatched first to the activeWidget, the one calling
// the set, and then to the other widgets in the set.
//
#ifndef __vtkKWEWidgetGroup_h
#define __vtkKWEWidgetGroup_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkObject.h"
#include <vtkstd/vector>

class vtkAbstractWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEWidgetGroup : public vtkObject
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEWidgetGroup *New();

  // Description:
  // Standard methods for a VTK class.
  vtkTypeRevisionMacro(vtkKWEWidgetGroup,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Method for activiating and deactiviating all widgets in the group.
  virtual void SetEnabled(int);
  vtkBooleanMacro(Enabled, int);

  // Description:
  // Add a widget to the set.
  void AddWidget(vtkAbstractWidget *);

  // Description:
  // Remove a widget from the set
  void RemoveWidget(vtkAbstractWidget *);

  // Description:
  // Get number of widgets in the set.
  unsigned int GetNumberOfWidgets();

  // Description:
  // Get the Nth widget in the set.
  vtkAbstractWidget *GetNthWidget( unsigned int );

  //BTX
  // TODO: Move this to the protected section. The class vtkAbstractWidget
  //       should be a friend of this class.
  typedef vtkstd::vector< vtkAbstractWidget * >   WidgetContainerType;
  typedef WidgetContainerType::iterator           WidgetIteratorType;
  typedef WidgetContainerType::const_iterator     WidgetConstIteratorType;
  WidgetContainerType                             Widget;

  // Description:
  // Pointer to a member function that takes a vtkAbstractWidget (the active
  // child) and another vtkAbstractWidget (the widget to dispatch an action)
  // to. All "Action" funtions in a widget must conform to this signature.
  // A return value of 0 from the active widget indicates that there is no
  // need to for the widget set to dispatch the action to other widgets in
  // the set.
  template< class TWidget > struct ActionFunction
    {
    typedef int (TWidget::*TActionFunctionPointer)(TWidget *dispatcher);
    };

  // Description:
  // Dispatch an "Action" to every widget in this set. This is meant be be
  // invoked from a "Callback" in a widget.
  template < class TWidget >
  void DispatchAction(TWidget *caller,
                      typename ActionFunction< TWidget >::TActionFunctionPointer action)
    {
    // Dispatch action to the caller first.
    for (WidgetIteratorType it  = this->Widget.begin();
                            it != this->Widget.end()  ; ++it)
      {
      TWidget *w = static_cast<TWidget *>(*it);
      if (caller == w)
        {
        if (((*w).*(action))(caller) == 0) return;
        break;
        }
      }

    // Dispatch action to all other widgets
    for (WidgetIteratorType it  = this->Widget.begin();
                            it != this->Widget.end()  ; ++it)
      {
      TWidget *w = static_cast<TWidget *>(*it);
      if (caller != w) ((*w).*(action))(caller);
      }
    }
  //ETX

  // Description:
  // Enable garbage collection due to ref counting cycles with vtkAbstractWidget
  virtual void Register(vtkObjectBase* o);
  virtual void UnRegister(vtkObjectBase* o);

  // Description:
  // Is this widget present in the group
  int HasWidget( vtkAbstractWidget * );

  // Description:
  // Render all the widgets in the group.
  void Render();

protected:
  vtkKWEWidgetGroup();
  ~vtkKWEWidgetGroup();

  // We need to break reference count loops. The widgets held by us refcount us
  // too
  virtual void ReportReferences(vtkGarbageCollector* collector);

private:
  vtkKWEWidgetGroup(const vtkKWEWidgetGroup&);  //Not implemented
  void operator=(const vtkKWEWidgetGroup&);  //Not implemented
};

#endif

