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

// This fragment shader scales and biases a framebuffer passed as a texture.
// Incoming color from the texture is pre-multiplied by alpha.
// It does not affect the alpha component.
// Passing the framebuffer as a texture allows the use of a reduction factor
// compared to the size of the final image.

// Framebuffer to scale.
uniform sampler2D frameBufferTexture;
uniform float scale;
uniform float bias;

void main()
{
  vec4 color=texture2D(frameBufferTexture,gl_TexCoord[0].xy);
  if(color.a==0.0)
    {
    discard;
    }
  // As incoming color is pre-multiplied by alpha, the bias has to be
  // multiplied by alpha before adding it.
  gl_FragColor.r=color.r*scale+bias*color.a;
  gl_FragColor.g=color.g*scale+bias*color.a;
  gl_FragColor.b=color.b*scale+bias*color.a;
  gl_FragColor.a=color.a;
}
