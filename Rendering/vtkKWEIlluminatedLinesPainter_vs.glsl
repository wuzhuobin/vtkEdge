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

// Filename: vtkIlluminatedLinesPainter_vp.glsl
// Filename is useful when using gldb-gui

/*
 * Vertex program for illuminated lines using maxumum reflection principle.
 *  IN: tangent(3) as gl_TexCoord[0].stp   
 */

varying vec4 varColor;
varying vec4 varSpecular;

vec3 getViewVector()
{
  // Camera position is obtained by doing:
  //    gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0);
  // However that's same as simply taking the 4th column, hence we can avoid the
  // matrix multiplication -- yea!!!
  vec4 cameraPosition = gl_ModelViewMatrixInverse[3];//* vec4(0.0, 0.0, 0.0, 1.0);
  return cameraPosition.xyz - gl_Vertex.xyz;
}

#define GL_AMBIENT 1
#define GL_DIFFUSE 2
#define GL_SPECULAR 3
#define GL_AMBIENT_AND_DIFFUSE 4
#define GL_EMISSION 5

uniform int uMode;
gl_MaterialParameters getMaterialParameters()
{
  if (uMode == 0)
    {
    return gl_FrontMaterial;
    }

  gl_MaterialParameters materialParams = gl_FrontMaterial;
  if (uMode == GL_AMBIENT)
    {
    materialParams.ambient = gl_Color;
    }
  else if (uMode == GL_DIFFUSE)
    {
    materialParams.diffuse = gl_Color;
    }
  else if (uMode == GL_SPECULAR)
    {
    materialParams.specular = gl_Color;
    }
  else if (uMode == GL_AMBIENT_AND_DIFFUSE)
    {
    materialParams.ambient = gl_Color;
    materialParams.diffuse = gl_Color;
    }
  else if (uMode == GL_EMISSION)
    {
    materialParams.emission = gl_Color;
    }
  return materialParams;
}

#define VTK_MAX_LIGHTS 8
void lighting()
{
  vec3 V = normalize(getViewVector());
  vec3 T = normalize(gl_Vertex.xyz - gl_MultiTexCoord1.stp); // tangent
  float Vt = min(0.9999999, abs(dot(V, T)));

  vec4 vAmbient = vec4(0.0);
  vec4 vDiffuse = vec4(0.0);
  vec4 vSpecular = vec4(0.0);

  gl_MaterialParameters material = getMaterialParameters();
  for (int cc=0; cc < VTK_MAX_LIGHTS; cc++)
    {
    // we are using the light.diffuse.w to convey the enabled state.
    if (gl_LightSource[cc].diffuse.w > 0.0)
      {
      // gl_LightSource gives position in ModelView space, we need to transform it
      // back to world space.
      vec4 worldLightPosition = gl_ModelViewMatrixInverse * gl_LightSource[cc].position;
      vec3 L = normalize(worldLightPosition.xyz - gl_Vertex.xyz);

      float Lt = min(0.9999999, abs(dot(L, T)));

      float Fd = sqrt(1.0 - Lt * Lt);
      float term = sqrt(1.0 - Vt*Vt) * Fd - Vt*Lt;
      float Fs = 0.0;
      if (term > 0.0)
        {
        Fs = pow(term, material.shininess);
        }
      vAmbient += (gl_LightSource[cc].ambient);
      vDiffuse += abs(gl_LightSource[cc].diffuse * Fd);
      vSpecular += abs(gl_LightSource[cc].specular * Fs);
      }
    }
 
  varColor = (vAmbient* material.ambient + vDiffuse * material.diffuse);
  varSpecular = material.specular *  vSpecular;

  vec4 color = varColor + varSpecular;
  gl_FrontColor = color;

  // This is essential, for scalar coloring done using textures to work.
  gl_TexCoord[0] = gl_MultiTexCoord0;
}

void main()
{
  lighting();
  gl_Position = ftransform();
}
