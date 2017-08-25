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

// Fragment program with ray cast and 4-dependent-component Minimum Intensity
// Projection (MinIP) method.
// Compilation: header part and the projection part are inserted first.
// pos is defined and initialized in header
// rayDir is defined in header and initialized in the projection part

uniform sampler3D dataSetTexture;
uniform sampler1D opacityTexture;

uniform vec3 lowBounds;
uniform vec3 highBounds;

// Entry position (global scope)
vec3 pos;
// Incremental vector in texture space (global scope)
vec3 rayDir;

float tMax;

// Sub-functions, depending on cropping mode
float initialMinValue();
vec4 initialColor();
void writeColorAndMinScalar(vec4 color,
                            vec4 opacity,
                            float minValue);

void trace(void)
{
  // Max intensity is the lowest value.
  float minValue=initialMinValue();
  vec4 color=initialColor();
  bool inside=true;
  float t=0.0;
  vec4 sample;
  bool changed=false;
  
  // We NEED two nested while loops. It is a trick to work around hardware
  // limitation about the maximum number of loops.
  while(inside)
    {
    while(inside)
      {
      sample=texture3D(dataSetTexture,pos);
      if(sample.w<minValue)
        {
        changed=true;
        minValue=sample.w;
        color=sample;
        }
      pos=pos+rayDir;
      t+=1.0;
      
      // yes, t<tMax && all(greaterThanEqual(pos,lowBounds))
      // && all(lessThanEqual(pos,highBounds));
      // looks better but the latest nVidia 177.80 has a bug...
      inside=t<tMax && pos.x>=lowBounds.x && pos.y>=lowBounds.y
        && pos.z>=lowBounds.z && pos.x<=highBounds.x && pos.y<=highBounds.y
        && pos.z<=highBounds.z;
      }
    }
  
  if(changed)
    {
    vec4 opacity=texture1D(opacityTexture,minValue);
    writeColorAndMinScalar(color,opacity,minValue);
    }
  else
    {
    discard;
    }
}
