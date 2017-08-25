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

// .NAME vtkKWEDensifyPolyData - Densify the input by adding points at the
// centroid
//
// .SECTION Description
// The filter takes any polygonal data as input and will tessellate cells that 
// are planar polygons present by fanning out triangles from its centroid. 
// Other cells are simply passed through to the output.  PointData, if present,
// is interpolated via linear interpolation. CellData for any tessellated cell
// is simply copied over from its parent cell. Planar polygons are assumed to 
// be convex. Funny things will happen if they are not.
// 
// The number of subdivisions can be controlled by the parameter
// NumberOfSubdivisions.

#ifndef __vtkKWEDensifyPolyData_h
#define __vtkKWEDensifyPolyData_h

#include "vtkPolyDataAlgorithm.h"
#include "VTKEdgeConfigure.h" // export symbols

class VTKEdge_GRAPHICS_EXPORT vtkKWEDensifyPolyData : public vtkPolyDataAlgorithm
{
public:
  vtkTypeRevisionMacro(vtkKWEDensifyPolyData,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkKWEDensifyPolyData *New();

  // Description:
  // Number of recursive subdivisions. Initial value is 1.
  vtkSetMacro( NumberOfSubdivisions, unsigned int );
  vtkGetMacro( NumberOfSubdivisions, unsigned int );

protected:
  vtkKWEDensifyPolyData();
  ~vtkKWEDensifyPolyData();

  virtual int RequestData(vtkInformation *, 
                          vtkInformationVector **, 
                          vtkInformationVector *);

  unsigned int NumberOfSubdivisions;

private:
  virtual int FillInputPortInformation(int, vtkInformation*);
  
  vtkKWEDensifyPolyData(const vtkKWEDensifyPolyData&);  // Not implemented.
  void operator=(const vtkKWEDensifyPolyData&);  // Not implemented.  
};

#endif


