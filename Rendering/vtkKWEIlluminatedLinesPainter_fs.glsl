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

// Filename: vtkIlluminatedLinesPainter_fp.glsl
// Filename is useful when using gldb-gui

// Fragement shader. Must only be used when InterpolateScalarsBeforeMapping is
// ON.

#version 110

uniform sampler2D tex;

varying vec4 varColor;
varying vec4 varSpecular;

void main ()
{
  vec4 texColor = texture2D(tex, gl_TexCoord[0].st);
  gl_FragColor = texColor * varColor +  varSpecular;
}
