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
#include "vtkSMKWEIlluminatedLinesRepresentationProxy.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkSMPropertyHelper.h"

vtkStandardNewMacro(vtkSMKWEIlluminatedLinesRepresentationProxy);
vtkCxxRevisionMacro(vtkSMKWEIlluminatedLinesRepresentationProxy, "$Revision: 1774 $");
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


