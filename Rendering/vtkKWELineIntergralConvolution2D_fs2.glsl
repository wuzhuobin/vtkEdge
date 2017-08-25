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

// Filename: vtkGPU2DLICPass2fp.glsl
// Filename is useful when using gldb-gui

#extension GL_ARB_draw_buffers : enable

uniform sampler2D texTCoords;
uniform sampler2D texLIC;

uniform sampler2D texVectorField;
uniform sampler2D texNoise;

uniform int reset; // when set the texTCoords is not used.
uniform int uDirection; // +1 -- forward, -1 -- backward.
uniform float stepSize; // integration step size in parametric space

uniform int uNumSteps; // number of steps in each direction.
int numSamples = 2*uNumSteps+1;

// Define protoypes for methods defined in vtkGPU2DLICCommon.glsl
vec2 rk4(vec2 xy, float h);
vec2 rk2(vec2 xy, float h);
vec3 getNoiseColor(vec2 tcoord);

// Then on, use tcoord from texCoords;
void main(void)
{
  vec2 tcoord = (float(reset) * gl_TexCoord[1].st)  +
    (float(1-reset) * texture2D(texTCoords, gl_TexCoord[0].st).rg);

  vec3 color = getNoiseColor(tcoord)/float(numSamples);
  vec2 nextTcoord = rk4(tcoord, float(uDirection) * stepSize);
  vec3 accumulatedColor = texture2D(texLIC, gl_TexCoord[0].st).rgb;
  color += accumulatedColor;

  gl_FragData[0] = vec4(color, 1.0);
  gl_FragData[1] = vec4(nextTcoord.s, nextTcoord.t, 0.0, 1.0);
}
