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
// Id: $Id: vtkKWEWireframePainter_fs.glsl 456 2008-07-29 19:04:06Z utkarsh $


uniform sampler2D texDepth;
uniform vec2 uViewSize;
varying vec4 vColor;

const vec2 threshold = vec2(0.00001, 0.005);
void main (void)
{
  float surfaceDepth = texture2D(texDepth, gl_FragCoord.xy/uViewSize).x;
  
  // tolerance increases as depth increases, this ensures that the lines appear
  // smooth and continuous.
  float tolerance = threshold.x + (1.0 - gl_FragCoord.z) * (threshold.y - threshold.x);
  if (surfaceDepth+tolerance < gl_FragCoord.z)
    {
    discard;
    }
  gl_FragColor = vColor;
}
