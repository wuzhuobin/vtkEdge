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
// FileID: $Id: vtkKWEImageGradientMagnitude_3D_fs.glsl 1774 2010-04-20 20:41:44Z avila $

#version 110


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

