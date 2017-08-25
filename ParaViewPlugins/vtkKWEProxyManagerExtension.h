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
// .NAME vtkKWEProxyManagerExtension 
// .SECTION Description

#ifndef __vtkKWEProxyManagerExtension_h
#define __vtkKWEProxyManagerExtension_h

#include "vtkSMProxyManagerExtension.h"

class vtkKWEProxyManagerExtension : public vtkSMProxyManagerExtension
{
public:
  static vtkKWEProxyManagerExtension* New();
  vtkTypeRevisionMacro(vtkKWEProxyManagerExtension, vtkSMProxyManagerExtension);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is called when an extension is registered with the proxy manager to
  // ensure that the extension is compatible with the proxy manager version.
  // Incompatible extensions are not registered with the proxy manager.
  virtual bool CheckCompatibility(int major, int minor, int patch);

  // Description:
  // Given the proxy name and group name, returns the XML element for
  // the proxy.
  virtual vtkPVXMLElement* GetProxyElement(const char* groupName,
    const char* proxyName, vtkPVXMLElement* currentElement);

//BTX
protected:
  vtkKWEProxyManagerExtension();
  ~vtkKWEProxyManagerExtension();

private:
  vtkKWEProxyManagerExtension(const vtkKWEProxyManagerExtension&); // Not implemented.
  void operator=(const vtkKWEProxyManagerExtension&); // Not implemented.

  class vtkMapOfElements;
  vtkMapOfElements* MapOfElements;
//ETX
};

#endif


