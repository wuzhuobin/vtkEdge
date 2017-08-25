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

#include "vtkKWERenderingFactory.h"

#include "vtkObjectFactory.h"
#include "vtkToolkits.h"
#include "vtkVersion.h"

#include "vtkKWEIlluminatedLinesPainter.h"
#include "vtkKWEWireframePainter.h"
#include "vtkKWEVBOTStripsPainter.h"

vtkStandardNewMacro(vtkKWERenderingFactory);
vtkCxxRevisionMacro(vtkKWERenderingFactory, "$Revision: 549 $");
//----------------------------------------------------------------------------
VTK_FACTORY_INTERFACE_IMPLEMENT(vtkKWERenderingFactory);

VTK_CREATE_CREATE_FUNCTION(vtkKWEIlluminatedLinesPainter);
VTK_CREATE_CREATE_FUNCTION(vtkKWEWireframePainter);
VTK_CREATE_CREATE_FUNCTION(vtkKWEVBOTStripsPainter);

//----------------------------------------------------------------------------
vtkKWERenderingFactory::vtkKWERenderingFactory()
{
  this->RegisterOverride("vtkLinesPainter",
    "vtkKWEIlluminatedLinesPainter",
    "KWERendering",
    1,
    vtkObjectFactoryCreatevtkKWEIlluminatedLinesPainter);

  this->RegisterOverride("vtkRepresentationPainter",
    "vtkKWEWireframePainter",
    "KWERendering",
    1,
    vtkObjectFactoryCreatevtkKWEWireframePainter);
  
  this->RegisterOverride("vtkTStripsPainter",
    "vtkKWEVBOTStripsPainter",
    "KWERendering",
    1,
    vtkObjectFactoryCreatevtkKWEVBOTStripsPainter);
}

//----------------------------------------------------------------------------
void vtkKWERenderingFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
const char* vtkKWERenderingFactory::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

//----------------------------------------------------------------------------
const char* vtkKWERenderingFactory::GetDescription()
{
  return "VTKEdge Rendering Support Factory";
}


//----------------------------------------------------------------------------
void vtkKWERenderingFactory::RegisterFactory()
{
  vtkKWERenderingFactory* factory = vtkKWERenderingFactory::New();
  vtkObjectFactory::RegisterFactory(factory);
  factory->Delete();
}
