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
#include "vtkSMKWEGlyph3DMapperRepresentationProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSourceProxy.h"
#include "vtkProcessModule.h"
#include "vtkSMRepresentationStrategy.h"
#include "vtkSmartPointer.h"
#include "vtkSMViewProxy.h"

vtkStandardNewMacro(vtkSMKWEGlyph3DMapperRepresentationProxy);
vtkCxxRevisionMacro(vtkSMKWEGlyph3DMapperRepresentationProxy, "$Revision: 594 $");
//----------------------------------------------------------------------------
vtkSMKWEGlyph3DMapperRepresentationProxy::vtkSMKWEGlyph3DMapperRepresentationProxy()
{
  this->GlyphMapper = 0;
  this->SourceOutputPort = 0;
}

//----------------------------------------------------------------------------
vtkSMKWEGlyph3DMapperRepresentationProxy::~vtkSMKWEGlyph3DMapperRepresentationProxy()
{
}

//----------------------------------------------------------------------------
void vtkSMKWEGlyph3DMapperRepresentationProxy::AddInput(
  unsigned int inputPort, vtkSMSourceProxy* input,
  unsigned int outputPort, const char* method)
{
  if (inputPort == 0)
    {
    this->Superclass::AddInput(inputPort, input, outputPort, method);
    return;
    }

  if (!input)
    {
    vtkErrorMacro("Source input cannot be NULL.");
    return;
    }

  input->CreateOutputPorts();
  int numPorts = input->GetNumberOfOutputPorts();

  if (numPorts <= 0 || static_cast<unsigned int>(numPorts) <= outputPort)
    {
    vtkErrorMacro("Input has no output (or required output port). "
      "Cannot create the representation.");
    return;
    }

  this->Source = input;
  this->SourceOutputPort = outputPort;
}

//----------------------------------------------------------------------------
bool vtkSMKWEGlyph3DMapperRepresentationProxy::BeginCreateVTKObjects()
{
  if (!this->Superclass::BeginCreateVTKObjects())
    {
    return false;
    }

  this->GlyphMapper = this->GetSubProxy("GlyphMapper");
  this->GlyphMapper->SetServers(
    vtkProcessModule::CLIENT|vtkProcessModule::RENDER_SERVER);
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMKWEGlyph3DMapperRepresentationProxy::EndCreateVTKObjects()
{
  if (!this->Superclass::EndCreateVTKObjects())
    {
    return false;
    }

  // Switch the mapper to use the GlyphMapper instead of the standard mapper.
  this->Connect(this->GlyphMapper, this->Prop3D, "Mapper");
  this->GlyphMapper->UpdateVTKObjects();
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMKWEGlyph3DMapperRepresentationProxy::InitializeStrategy(vtkSMViewProxy* view)
{
  if (!this->Superclass::InitializeStrategy(view))
    {
    return false;
    }

  vtkSMPropertyHelper(this->GlyphMapper, "Input").Set(
    vtkSMPropertyHelper(this->Mapper, "Input").GetAsProxy());

  // source input is always polydata.
  vtkSmartPointer<vtkSMRepresentationStrategy> strategy;
  strategy.TakeReference(view->NewStrategy(VTK_POLY_DATA));
  if (!strategy.GetPointer())
    {
    vtkErrorMacro("View could not provide a strategy to use. "
      << "Cannot be rendered in this view of type " << view->GetClassName());
    return false;
    }

  // Now initialize the data pipelines involving this strategy.
  // Since representations are not added to views unless their input is set, we
  // can assume that the objects for this proxy have been created.
  // (Look at vtkSMDataRepresentationProxy::AddToView()).

  strategy->SetEnableLOD(false);

  this->Connect(this->Source, strategy, "Input", this->SourceOutputPort);
  this->Connect(strategy->GetOutput(), this->GlyphMapper, "Source");

  // Creates the strategy objects.
  strategy->UpdateVTKObjects();

  this->AddStrategy(strategy);

  this->GlyphMapper->UpdateVTKObjects();
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMKWEGlyph3DMapperRepresentationProxy::GetBounds(double bounds[6])
{
  //this->Superclass::GetBounds(bounds);
  this->GlyphMapper->UpdatePropertyInformation();
  vtkSMPropertyHelper(this->GlyphMapper, "BoundsInfo").Get(bounds, 6);
  //cout << "Bounds: " << bounds[0] << ", " << bounds[1] << ", " << bounds[2]
  //  << bounds[3] << ", " << bounds[4] << ", " << bounds[5] << endl;
  return true;
}

//----------------------------------------------------------------------------
void vtkSMKWEGlyph3DMapperRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


