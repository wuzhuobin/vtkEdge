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

//-----------------------------------------------------------------------------
// .NAME vtkKWEEventForwarderCommand - listen for events and forward them on 
// .SECTION Description
// This class can be used as an observer callback when you need to forward
// events. For example, if class A uses a class B, and someone is listening
// to class A for progress events, then class A must listen to class B for
// progress events and invoke its own progress event. 
//
// .SECTION see also
// vtkKWEGPUVolumeRayCastMapper
//-----------------------------------------------------------------------------

#ifndef __vtkKWEEventForwarderCommand_h
#define __vtkKWEEventForwarderCommand_h

#include "vtkObject.h"
#include "vtkCommand.h"
#include "VTKEdgeConfigure.h" // include configuration header


class VTKEdge_COMMON_EXPORT vtkKWEEventForwarderCommand : public vtkCommand
{
public:
  // Description:
  // Standard new method - no object factor, just return this object.
  // vtkCommand is not a subclass of vtkObject so we can't use the standard
  // new macro- we'll just implement it here.
  static vtkKWEEventForwarderCommand *New() {return new vtkKWEEventForwarderCommand;}
  vtkTypeRevisionMacro(vtkKWEEventForwarderCommand,vtkCommand);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void Execute(vtkObject *caller, unsigned long event, void *callData);

  void SetForwardingClass( vtkObject *c );
  vtkObject *GetForwardingClass() {return this->ForwardingClass;}
  
  
protected:
  vtkKWEEventForwarderCommand();
  ~vtkKWEEventForwarderCommand();
  
  vtkObject *ForwardingClass;
  
private:
  vtkKWEEventForwarderCommand(const vtkKWEEventForwarderCommand&);  // Not implemented.
  void operator=(const vtkKWEEventForwarderCommand&);  // Not implemented.
};

#endif

