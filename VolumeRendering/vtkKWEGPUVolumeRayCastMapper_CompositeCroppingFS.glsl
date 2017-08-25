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

// Implementation of some function used by the composite method when cropping
// is on.

// color buffer as an input
uniform sampler2D frameBufferTexture;
// 2D Texture fragment coordinates [0,1] from fragment coordinates
// the frame buffer texture has the size of the plain buffer but
// we use a fraction of it. The texture coordinates is less than 1 if
// the reduction factor is less than 1.
vec2 fragTexCoord;

vec4 initialColor()
{
  return texture2D(frameBufferTexture,fragTexCoord);
}
