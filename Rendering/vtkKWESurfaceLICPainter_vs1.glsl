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
// Filename: vtkSurfaceLICPainterPass1vp.glsl
// Filename is useful when using gldb-gui

#version 120 
varying vec4 vColor;
varying vec2 vProjectedVF;

// from vtkKWEColorMaterialHelper
gl_MaterialParameters getMaterialParameters();

// from vtkKWELightingHelper
vec4 singleColor(gl_MaterialParameters m,
  vec3 surfacePosEyeCoords, vec3 n);

// Projects "vector" onto the surface.
vec3 projectOnSurface(vec3 vector)
{
  vec3 normal = normalize(gl_Normal);
  float k = dot(normal, vector);
  return (vector - (k*normal));
}

vec4 colorFrontFace()
{
 vec4 heyeCoords = gl_ModelViewMatrix*gl_Vertex;
 vec3 eyeCoords = heyeCoords.xyz/heyeCoords.w;
 vec3 n = normalize(gl_NormalMatrix*gl_Normal);
 return singleColor(getMaterialParameters(),eyeCoords,n);
}

void main()
{
  vec3 vf = projectOnSurface(gl_MultiTexCoord0.stp);
  vProjectedVF = (gl_NormalMatrix * vf).xy;
  vColor = colorFrontFace();
  gl_Position = ftransform();
}
