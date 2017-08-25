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

// Fragment program part with ray cast and composite method.

uniform sampler3D dataSetTexture;
uniform sampler1D colorTexture;
uniform sampler1D opacityTexture;

uniform vec3 lowBounds;
uniform vec3 highBounds;

// Entry position (global scope)
vec3 pos;
// Incremental vector in texture space (global scope)
vec3 rayDir;

float tMax;

vec4 initialColor();

void trace(void)
{
  // Max intensity is the lowest value.
  vec4 destColor=initialColor();
  float remainOpacity=1.0-destColor.a;

  bool inside=true;
  
  vec4 scalarValue;
  vec4 color;
  vec4 opacity;

  float t=0.0;
  
  // We NEED two nested while loops. It is trick to work around hardware
  // limitation about the maximum number of loops.

  while(inside)
    {  
    while(inside)
      {
      // scalarValue is the sampled texture value in the volume at pos
      scalarValue=texture3D(dataSetTexture,pos);
      // color is the sampled texture value in the 1D color texture at
      // scalarValue
      color=texture1D(colorTexture,scalarValue.x);
      // opacity is the sampled texture value in the 1D opacity texture at
      // scalarValue
      opacity=texture1D(opacityTexture,scalarValue.x);
      
      color=color*opacity.a;
      destColor=destColor+color*remainOpacity;
      remainOpacity=remainOpacity*(1.0-opacity.a);
      
      pos=pos+rayDir;
      t+=1.0;
      inside=t<tMax && all(greaterThanEqual(pos,lowBounds))
        && all(lessThanEqual(pos,highBounds))
        && (remainOpacity>=0.0039); // 1/255=0.0039
      }
    }
  gl_FragColor = destColor;
  gl_FragColor.a = 1.0-remainOpacity;
}
