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

// Implementation of some functions used by the Minimum Intensity projection
// (MinIP) method when cropping is off.

float initialMinValue()
{
  return 1.0;
}

void writeColorAndMinScalar(vec4 sample,
                            vec4 opacity,
                            float minValue)
{
  // we don't need to write minValue to a buffer when there is no cropping.
  // color framebuffer
  gl_FragColor.r =sample.r * opacity.a;
  gl_FragColor.g =sample.g * opacity.a;
  gl_FragColor.b =sample.b * opacity.a;
  gl_FragColor.a=opacity.a;
}
