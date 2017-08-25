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
// FileID: $Id: vtkKWEImageGradientMagnitude_3D_fs.glsl 359 2008-06-26 14:48:56Z utkarsh $

// Expected uniforms.
uniform sampler3D uInputImage;
uniform vec3 uSpacingReciprocal;
uniform vec3 uInputImageDims;

// Computed constants.
const float step_w = 1.0/uInputImageDims.x;
const float step_h = 1.0/uInputImageDims.y;
const float step_d = 1.0/uInputImageDims.z;
vec3 offset[3] = vec3[](vec3(step_w, 0, 0), vec3(0, step_h,0), vec3(0, 0, step_d));
  
void main(void)
{
  vec4 dX = texture3D(uInputImage, gl_TexCoord[0].stp-offset[0]);
  dX -= texture3D(uInputImage, gl_TexCoord[0].stp+offset[0]);
  dX *= uSpacingReciprocal.x;
  vec4 dY = texture3D(uInputImage, gl_TexCoord[0].stp-offset[1]);
  dY -= texture3D(uInputImage, gl_TexCoord[0].stp+offset[1]);
  dY *= uSpacingReciprocal.y;
  vec4 dZ = texture3D(uInputImage, gl_TexCoord[0].stp-offset[2]);
  dZ -= texture3D(uInputImage, gl_TexCoord[0].stp+offset[2]);
  dZ *= uSpacingReciprocal.z;
  gl_FragColor = sqrt(dX*dX+dY*dY+dZ*dZ);
} 

