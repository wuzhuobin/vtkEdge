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
#include "vtkSMKWEIlluminatedLinesRepresentationProxy.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMPropertyHelper.h"

vtkStandardNewMacro(vtkSMKWEIlluminatedLinesRepresentationProxy);
vtkCxxRevisionMacro(vtkSMKWEIlluminatedLinesRepresentationProxy, "$Revision: 467 $");
//----------------------------------------------------------------------------
vtkSMKWEIlluminatedLinesRepresentationProxy::vtkSMKWEIlluminatedLinesRepresentationProxy()
{
}

//----------------------------------------------------------------------------
vtkSMKWEIlluminatedLinesRepresentationProxy::~vtkSMKWEIlluminatedLinesRepresentationProxy()
{
}

//----------------------------------------------------------------------------
bool vtkSMKWEIlluminatedLinesRepresentationProxy::EndCreateVTKObjects()
{
  if (!this->Superclass::EndCreateVTKObjects())
    {
    return false;
    }

  vtkClientServerStream stream;
  vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
  vtkClientServerID helperID = pm->NewStreamObject("vtkKWEIlluminatedLinesPainter", stream);
  stream  << vtkClientServerStream::Invoke
          << helperID
          << "UpdateMapper"
          << this->Mapper->GetID()
          << vtkClientServerStream::End;
  pm->DeleteStreamObject(helperID, stream);

  pm->SendStream(this->GetConnectionID(),
    vtkProcessModule::RENDER_SERVER | vtkProcessModule::CLIENT, stream);
  return true;
}

//----------------------------------------------------------------------------
void vtkSMKWEIlluminatedLinesRepresentationProxy::UpdateShadingParameters()
{
  double diffuse = this->Diffuse;
  double specular = this->Specular;
  double ambient = this->Ambient;

  vtkSMPropertyHelper(this->Property,"Ambient").Set(ambient);
  vtkSMPropertyHelper(this->Property,"Diffuse").Set(diffuse);
  vtkSMPropertyHelper(this->Property,"Specular").Set(specular);
  this->Property->UpdateVTKObjects();
}

//----------------------------------------------------------------------------
void vtkSMKWEIlluminatedLinesRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


