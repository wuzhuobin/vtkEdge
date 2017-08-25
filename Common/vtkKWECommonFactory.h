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
// .NAME vtkKWECommonFactory - 
// .SECTION Description

#ifndef __vtkKWECommonFactory_h
#define __vtkKWECommonFactory_h

#include "vtkObjectFactory.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_COMMON_EXPORT vtkKWECommonFactory : public vtkObjectFactory
{
public:
  static vtkKWECommonFactory *New();
  vtkTypeRevisionMacro(vtkKWECommonFactory,vtkObjectFactory);
  virtual void PrintSelf(ostream& os, vtkIndent indent);
  
  virtual const char* GetVTKSourceVersion();
  virtual const char* GetDescription();
  
  // Description:
  // Create and return an instance of the named vtk object.
  // This method first checks the vtkObjectFactory to support
  // dynamic loading. 
  static vtkObject *CreateInstance(const char *vtkclassname);
  
protected:
  vtkKWECommonFactory();

private:
  vtkKWECommonFactory(const vtkKWECommonFactory&);  // Not implemented.
  void operator=(const vtkKWECommonFactory&);  // Not implemented.
};



#endif
