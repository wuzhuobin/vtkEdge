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
#include "pqKWESurfaceLICDisplayPanelDecorator.h"
#include "ui_pqKWESurfaceLICDisplayPanelDecorator.h"

// Server Manager Includes.
#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkSmartPointer.h"
#include "vtkSMProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMPVRepresentationProxy.h"

// Qt Includes.
#include <QVBoxLayout>

// ParaView Includes.
#include "pqDisplayProxyEditor.h"
#include "pqRepresentation.h"
#include "pqFieldSelectionAdaptor.h"
#include "pqPropertyLinks.h"

class pqKWESurfaceLICDisplayPanelDecorator::pqInternals : 
  public Ui::pqKWESurfaceLICDisplayPanelDecorator
{
public:
  pqPropertyLinks Links;
  vtkSMProxy* Representation;
  QWidget* Frame;
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;

  pqInternals()
    {
    this->Representation = 0;
    this->Frame = 0;
    this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    }
};

//-----------------------------------------------------------------------------
pqKWESurfaceLICDisplayPanelDecorator::pqKWESurfaceLICDisplayPanelDecorator(
  pqDisplayProxyEditor* panel):Superclass(panel)
{
  this->Internals = 0;

  vtkSMProxy* repr = panel->getRepresentation()->getProxy();
  vtkSMProperty* prop = repr->GetProperty("SelectLICVectors");
  if (prop)
    {
    this->Internals = new pqInternals();
    this->Internals->Representation = repr;
    QWidget* wid = new QWidget(panel);
    this->Internals->Frame = wid;
    this->Internals->setupUi(wid);
    QVBoxLayout* l = qobject_cast<QVBoxLayout*>(panel->layout());
    l->addWidget(wid);

    pqFieldSelectionAdaptor* adaptor= new pqFieldSelectionAdaptor(
      this->Internals->Vectors, prop);

    this->Internals->Links.addPropertyLink(
      adaptor, "attributeMode", SIGNAL(selectionChanged()),
      repr, prop, 0);
    this->Internals->Links.addPropertyLink(
      adaptor, "scalar", SIGNAL(selectionChanged()),
      repr, prop, 1);
    this->Internals->Links.addPropertyLink(
      this->Internals->NumberOfSteps, "value", SIGNAL(valueChanged(int)),
      repr, repr->GetProperty("LICNumberOfSteps"));
    this->Internals->Links.addPropertyLink(
      this->Internals->StepSize, "value", SIGNAL(valueChanged(double)),
      repr, repr->GetProperty("LICStepSize"));
    this->Internals->Links.addPropertyLink(
      this->Internals->LICIntensity, "value", SIGNAL(valueChanged(double)),
      repr, repr->GetProperty("LICIntensity"));
    this->Internals->Links.addPropertyLink(
      this->Internals->UseLICForLOD, "checked", SIGNAL(toggled(bool)),
      repr, repr->GetProperty("UseLICForLOD"));

    repr->GetProperty("Input")->UpdateDependentDomains();
    prop->UpdateDependentDomains();

    this->Internals->VTKConnect->Connect(repr->GetProperty("Representation"),
      vtkCommand::ModifiedEvent,
      this, SLOT(representationTypeChanged()));
    this->representationTypeChanged();

    QObject::connect(&this->Internals->Links, SIGNAL(smPropertyChanged()),
      panel, SLOT(updateAllViews()), Qt::QueuedConnection);
    }

}

//-----------------------------------------------------------------------------
pqKWESurfaceLICDisplayPanelDecorator::~pqKWESurfaceLICDisplayPanelDecorator()
{
  delete this->Internals;
  this->Internals = 0;
}

//-----------------------------------------------------------------------------
void pqKWESurfaceLICDisplayPanelDecorator::representationTypeChanged()
{
  if (this->Internals)
    {
    int reprType = 
      vtkSMPropertyHelper(this->Internals->Representation,
      "Representation").GetAsInt();
    if (reprType == vtkSMPVRepresentationProxy::USER_DEFINED+1)
      {
      this->Internals->Frame->setEnabled(true);
      vtkSMPropertyHelper(this->Internals->Representation,
        "InterpolateScalarsBeforeMapping").Set(0);
      this->Internals->Representation->UpdateVTKObjects();
      }
    else
      {
      this->Internals->Frame->setEnabled(false);
      }
    }
}

