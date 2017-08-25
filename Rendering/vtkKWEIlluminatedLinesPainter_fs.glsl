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

// Filename: vtkIlluminatedLinesPainter_fp.glsl
// Filename is useful when using gldb-gui

// Fragement shader. Must only be used when InterpolateScalarsBeforeMapping is
// ON.

uniform sampler2D tex;

varying vec4 varColor;
varying vec4 varSpecular;

void main ()
{
  vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
  gl_FragColor = texColor * varColor +  varSpecular;
}