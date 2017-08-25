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

// Perspective projection.

// Entry position (global scope)
vec3 pos;
// Incremental vector in texture space (global scope)
vec3 rayDir;

// Camera position in texture space
uniform vec3 cameraPosition;
// Sample distance in world space
uniform float sampleDistance;
// Matrix coefficients: diagonal (a11,a22,a33)
uniform vec3 matrix1;
// Matrix coefficients: others (2a12,2a23,2a13)
uniform vec3 matrix2;

// Defined in the right projection method.
void incrementalRayDirection()
{
  // Direction of the ray in texture space, not normalized.
  rayDir=pos-cameraPosition;
  
  // x^2, y^2, z^2
  vec3 normDir=rayDir*rayDir;
  normDir.x=dot(normDir,matrix1);
  
  // xy,yz,zx
  vec3 coefs=rayDir*rayDir.yxz;
  coefs.x=dot(coefs,matrix2);

  // n^2
  normDir.x=normDir.x+coefs.x;
  
  // 1/n
  // normDir=1/sqrt(normDir)
  normDir.x=inversesqrt(normDir.x);
  
  // Final scale factor for the ray direction in texture space
  // normDir=normDir*sampleDistance
  normDir.x=normDir.x*sampleDistance;
  // Now, rayDir is the incremental direction in texture space
  rayDir=rayDir*normDir.x;
}
