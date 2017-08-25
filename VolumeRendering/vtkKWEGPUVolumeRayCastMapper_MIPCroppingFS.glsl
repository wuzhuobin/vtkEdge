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

// Implementation of some functions used by the Maximum Intensity Projection
// (MIP) method when cropping is on.

// max scalar buffer as an input
uniform sampler2D scalarBufferTexture;
// 2D Texture fragment coordinates [0,1] from fragment coordinates
// the scalar frame buffer texture has the size of the plain buffer but
// we use a fraction of it. The texture coordinates is less than 1 if
// the reduction factor is less than 1.
vec2 fragTexCoord;

float initialMaxValue()
{
  return texture2D(scalarBufferTexture,fragTexCoord).r;
}

void writeColorAndMaxScalar(vec4 sample,
                            vec4 opacity,
                            float maxValue)
{
  // color framebuffer
  gl_FragData[0].r =sample.r * opacity.a;
  gl_FragData[0].g =sample.g * opacity.a;
  gl_FragData[0].b =sample.b * opacity.a;
  gl_FragData[0].a=opacity.a;
  
  // max scalar framebuffer
  gl_FragData[1].r=maxValue;
  gl_FragData[1].g=0.0;
  gl_FragData[1].b=0.0;
  gl_FragData[1].a=0.0;
}
