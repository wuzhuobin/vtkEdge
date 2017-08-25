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

// Filename: vtkGPU2DLICCommon.glsl
// Filename is useful when using gldb-gui

// Provides a set of methods that the shaders can use.

uniform sampler2D texVectorField;
uniform sampler2D texNoise;
uniform vec2 uNoiseScale;
uniform vec2 uVectorTransform;
uniform vec2 uVectorShiftScale;

// Define prototype. 
// This function is compiled in to select the two components that form the
// surface vector.
vec2 getSelectedComponents(vec4 color);

// Returns the vector in "Normalized Image" space.
vec2 getVector(vec2 tcords)
{
  vec4 color = texture2D(texVectorField, tcords);
  vec2 vector = getSelectedComponents(color);
  vector = (vector / uVectorShiftScale.y) - uVectorShiftScale.x;
  return vector * uVectorTransform;
}

vec2 rk4(vec2 xy, float ds)
{
  // putting a condition here if a==vec2(0.0) slows the excution down a tiny
  // bit, so we simply do the computation.
  vec2 a = getVector(xy);
  float length_a = length(a);
  float dt = ds/length_a;
  if (length_a == 0.0)
    {
    dt = 0.0;
    }

  vec2 b = getVector(xy + (dt*a/2.0));
  vec2 c = getVector(xy + (dt*b/2.0));
  vec2 d = getVector(xy + dt*c);
  return (xy + (a + 2.0*b + 2.0*c + d)*dt/6.0);
}

vec2 rk2(vec2 xy, float ds)
{
  vec2 a = getVector(xy);
  float length_a = length(a);
  float dt = ds/length_a;
  if (length_a == 0.0)
    {
    dt = 0.0;
    }

  vec2 b = getVector(xy + dt*a/2.0);
  return (xy + dt*b);
}

vec3 getNoiseColor(vec2 tcoord)
{
  vec2 fraction = vec2(1.0)/uNoiseScale;
  vec2 tcoord2 = mod(tcoord, fraction) * uNoiseScale;
  return texture2D(texNoise, tcoord2).rgb;
}


