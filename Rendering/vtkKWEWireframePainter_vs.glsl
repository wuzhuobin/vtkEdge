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
// Id: $Id: vtkKWEWireframePainter_vs.glsl 1774 2010-04-20 20:41:44Z avila $

#version 110
varying vec4 vColor;

// from vtkKWEColorMaterialHelper
gl_MaterialParameters getMaterialParameters();

// from vtkKWELightingHelper
vec4 singleColor(gl_MaterialParameters m,
  vec3 surfacePosEyeCoords, vec3 n);

vec4 colorFrontFace()
{
 vec4 heyeCoords = gl_ModelViewMatrix*gl_Vertex;
 vec3 eyeCoords = heyeCoords.xyz/heyeCoords.w;
 vec3 n = normalize(gl_NormalMatrix*gl_Normal);
 return singleColor(getMaterialParameters(),eyeCoords,n);
}

void main (void)
{
  gl_Position = ftransform();
  vColor = colorFrontFace();
}
