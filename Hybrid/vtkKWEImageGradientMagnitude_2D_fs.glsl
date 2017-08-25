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
// FileID: $Id: vtkKWEImageGradientMagnitude_2D_fs.glsl 1774 2010-04-20 20:41:44Z avila $

#version 110

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

