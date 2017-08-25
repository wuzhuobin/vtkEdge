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
// FileID: $Id: vtkKWEImageGradientMagnitude_2D_fs.glsl 359 2008-06-26 14:48:56Z utkarsh $

// Expected uniforms.
uniform sampler2D uInputImage;
uniform vec3 uSpacingReciprocal;
uniform vec3 uInputImageDims;

// Computed constants.
const float step_w = 1.0/uInputImageDims.x;
const float step_h = 1.0/uInputImageDims.y;
vec2 offset[2] = vec2[](vec2(step_w, 0), vec2(0, step_h));

void main(void)
{
  vec4 dX = texture2D(uInputImage, gl_TexCoord[0].st-offset[0]);
  dX -= texture2D(uInputImage, gl_TexCoord[0].st+offset[0]);
  dX *= uSpacingReciprocal.x;
  vec4 dY = texture2D(uInputImage, gl_TexCoord[0].st-offset[1]);
  dY -= texture2D(uInputImage, gl_TexCoord[0].st+offset[1]);
  dY *= uSpacingReciprocal.y;
  gl_FragColor = sqrt(dX*dX+dY*dY);
}

