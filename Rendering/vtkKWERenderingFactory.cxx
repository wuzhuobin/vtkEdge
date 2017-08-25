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

#include "vtkKWERenderingFactory.h"

#include "vtkObjectFactory.h"
#include "vtkToolkits.h"
#include "vtkVersion.h"

#include "vtkKWEIlluminatedLinesPainter.h"
#include "vtkKWEWireframePainter.h"
#include "vtkKWEVBOTStripsPainter.h"

vtkStandardNewMacro(vtkKWERenderingFactory);
vtkCxxRevisionMacro(vtkKWERenderingFactory, "$Revision: 1774 $");
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
