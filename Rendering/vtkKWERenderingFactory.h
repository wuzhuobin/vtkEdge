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
// .NAME vtkKWERenderingFactory -
// .SECTION Description

#ifndef __vtkKWERenderingFactory_h
#define __vtkKWERenderingFactory_h

#include "vtkObjectFactory.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_RENDERING_EXPORT vtkKWERenderingFactory : public vtkObjectFactory
{
public:
  static vtkKWERenderingFactory *New();
  vtkTypeRevisionMacro(vtkKWERenderingFactory,vtkObjectFactory);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  virtual const char* GetVTKSourceVersion();
  virtual const char* GetDescription();

  // Description:
  // Can be called to register this factory with the vtkObjectFactory singleton.
  static void RegisterFactory();
protected:
  vtkKWERenderingFactory();

private:
  vtkKWERenderingFactory(const vtkKWERenderingFactory&);  // Not implemented.
  void operator=(const vtkKWERenderingFactory&);  // Not implemented.
};



#endif
