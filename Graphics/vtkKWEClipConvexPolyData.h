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

#ifndef __vtkKWEClipConvexPolyData_h
#define __vtkKWEClipConvexPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkPlaneCollection;
class vtkPlane;
class vtkKWEClipConvexPolyDataInternals;

class VTKEdge_GRAPHICS_EXPORT vtkKWEClipConvexPolyData : public vtkPolyDataAlgorithm
{
public:
  static vtkKWEClipConvexPolyData *New();
  vtkTypeRevisionMacro(vtkKWEClipConvexPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set all the planes at once using a vtkPlanes implicit function.
  // This also sets the D value, so it can be used with GenerateClipConvexPolyData().
  void SetPlanes( vtkPlaneCollection *planes );
  vtkGetObjectMacro( Planes, vtkPlaneCollection );
  
  // Description:
  // Redefines this method, as this filter depends on time of its components
  // (planes)
  virtual unsigned long int GetMTime();
  
protected:
  vtkKWEClipConvexPolyData();
  ~vtkKWEClipConvexPolyData();

  vtkPlaneCollection *Planes;

  vtkKWEClipConvexPolyDataInternals *Internal;

  void ClipWithPlane( vtkPlane *, double tolerance );
  
  int HasDegeneracies( vtkPlane * );

  // The method that does it all...
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  void ClearInternals();
  void ClearNewVertices();
  void RemoveEmptyPolygons();
  
private:
  vtkKWEClipConvexPolyData(const vtkKWEClipConvexPolyData&);  // Not implemented.
  void operator=(const vtkKWEClipConvexPolyData&);  // Not implemented.
};

#endif
