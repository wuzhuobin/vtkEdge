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

// .NAME vtkKWEVolumeRenderingFactory - 
// .SECTION Description

#ifndef __vtkKWEVolumeRenderingFactory_h
#define __vtkKWEVolumeRenderingFactory_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h"

//#include "KWEVolumeRenderingConfigure.h" // include configuration header

class VTKEdge_VOLUMERENDERING_EXPORT vtkKWEVolumeRenderingFactory : public vtkObject
{
public:
  static vtkKWEVolumeRenderingFactory *New();
  vtkTypeRevisionMacro(vtkKWEVolumeRenderingFactory,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create and return an instance of the named vtk object.
  // This method first checks the vtkObjectFactory to support
  // dynamic loading. 
  static vtkObject* CreateInstance(const char* vtkclassname);

protected:
  vtkKWEVolumeRenderingFactory() {};

private:
  vtkKWEVolumeRenderingFactory(const vtkKWEVolumeRenderingFactory&);  // Not implemented.
  void operator=(const vtkKWEVolumeRenderingFactory&);  // Not implemented.
};

#endif
