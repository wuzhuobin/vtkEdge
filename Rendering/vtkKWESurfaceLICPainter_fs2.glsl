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

// Filename: vtkSurfaceLICPainterPass2fp.glsl
// Filename is useful when using gldb-gui

uniform sampler2D texLIC;
uniform sampler2D texGeometry;
uniform sampler2D texDepth;

uniform float uLICIntensity;
void main()
{
  vec3 lic = texture2D(texLIC, gl_TexCoord[0].st).rgb;
  vec4 color = texture2D(texGeometry, gl_TexCoord[1].st);
  float depth = texture2D(texDepth, gl_TexCoord[1].st).b;
  if (depth == 0.0)
    {
    discard;
    }

  gl_FragColor = vec4((uLICIntensity*lic + (1.0-uLICIntensity)*color.xyz), color.a);
  gl_FragDepth = depth;
}
