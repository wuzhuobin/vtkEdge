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

#include "vtkSMKWESurfaceLICRepresentationProxy.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMPropertyHelper.h"

vtkStandardNewMacro(vtkSMKWESurfaceLICRepresentationProxy);
vtkCxxRevisionMacro(vtkSMKWESurfaceLICRepresentationProxy, "$Revision: 595 $");
//----------------------------------------------------------------------------
vtkSMKWESurfaceLICRepresentationProxy::vtkSMKWESurfaceLICRepresentationProxy()
{
}

//----------------------------------------------------------------------------
vtkSMKWESurfaceLICRepresentationProxy::~vtkSMKWESurfaceLICRepresentationProxy()
{
}

//----------------------------------------------------------------------------
void vtkSMKWESurfaceLICRepresentationProxy::SetUseLICForLOD(int use)
{
  vtkSMProxy* licPainterLOD = this->GetSubProxy("SurfaceLICPainterLOD");    
  vtkSMPropertyHelper(licPainterLOD, "Enable").Set(use);
  licPainterLOD->UpdateProperty("Enable");
}

//----------------------------------------------------------------------------
bool vtkSMKWESurfaceLICRepresentationProxy::BeginCreateVTKObjects()
{
  if (!this->Superclass::BeginCreateVTKObjects())
    {
    return false;
    }

  vtkSMProxy* licPainter = this->GetSubProxy("SurfaceLICPainter");
  licPainter->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);

  vtkSMProxy* licPainterLOD = this->GetSubProxy("SurfaceLICPainterLOD");
  licPainterLOD->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);

  vtkSMProxy* licDefaultPainter = this->GetSubProxy("SurfaceLICDefaultPainter");
  licDefaultPainter->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);

  vtkSMProxy* licDefaultPainterLOD = this->GetSubProxy("SurfaceLICDefaultPainterLOD");
  licDefaultPainterLOD->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMKWESurfaceLICRepresentationProxy::EndCreateVTKObjects()
{
  vtkSMProxy* licPainter = this->GetSubProxy("SurfaceLICPainter");
  vtkSMProxy* licPainterLOD = this->GetSubProxy("SurfaceLICPainterLOD");
  vtkSMProxy* licDefaultPainter = this->GetSubProxy("SurfaceLICDefaultPainter");
  vtkSMProxy* licDefaultPainterLOD = this->GetSubProxy("SurfaceLICDefaultPainterLOD");

  vtkSMPropertyHelper(licDefaultPainter, "SurfaceLICPainter").Set(licPainter);
  vtkSMPropertyHelper(licDefaultPainterLOD, "SurfaceLICPainter").Set(licPainterLOD);
  licDefaultPainter->UpdateProperty("SurfaceLICPainter");
  licDefaultPainterLOD->UpdateProperty("SurfaceLICPainter");

  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
          << this->Mapper->GetID()
          << "GetPainter"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << vtkClientServerStream::LastResult
          << "GetDelegatePainter"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << licDefaultPainter->GetID()
          << "SetDelegatePainter"
          << vtkClientServerStream::LastResult
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << this->Mapper->GetID()
          << "SetPainter"
          << licDefaultPainter->GetID()
          << vtkClientServerStream::End;

  stream  << vtkClientServerStream::Invoke
          << this->LODMapper->GetID()
          << "GetPainter"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << vtkClientServerStream::LastResult
          << "GetDelegatePainter"
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << licDefaultPainterLOD->GetID()
          << "SetDelegatePainter"
          << vtkClientServerStream::LastResult
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << this->LODMapper->GetID()
          << "SetPainter"
          << licDefaultPainterLOD->GetID()
          << vtkClientServerStream::End;

  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  pm->SendStream(this->GetConnectionID(), 
    vtkProcessModule::RENDER_SERVER | vtkProcessModule::CLIENT,
    stream);
  return this->Superclass::EndCreateVTKObjects();
}

//----------------------------------------------------------------------------
void vtkSMKWESurfaceLICRepresentationProxy::SelectInputVectors(int, int, int, 
  int attributeMode, const char* name)
{
  if (!this->ObjectsCreated)
    {
    vtkErrorMacro("Objects not created yet!");
    return;
    }

  vtkSMProxy* licPainter = this->GetSubProxy("SurfaceLICPainter");
  licPainter->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);

  vtkSMProxy* licPainterLOD = this->GetSubProxy("SurfaceLICPainterLOD");
  licPainterLOD->SetServers(vtkProcessModule::RENDER_SERVER|vtkProcessModule::CLIENT);

  vtkClientServerStream stream;
  stream  << vtkClientServerStream::Invoke
          << licPainter->GetID()
          << "SetInputArrayToProcess"
          << attributeMode
          << name
          << vtkClientServerStream::End;
  stream  << vtkClientServerStream::Invoke
          << licPainterLOD->GetID()
          << "SetInputArrayToProcess"
          << attributeMode
          << name
          << vtkClientServerStream::End;
  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  pm->SendStream(this->GetConnectionID(), 
    vtkProcessModule::RENDER_SERVER | vtkProcessModule::CLIENT,
    stream);
}

//----------------------------------------------------------------------------
void vtkSMKWESurfaceLICRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


