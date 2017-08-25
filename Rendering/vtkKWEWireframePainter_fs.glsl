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
// Id: $Id: vtkKWEWireframePainter_fs.glsl 1774 2010-04-20 20:41:44Z avila $
#version 110

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
