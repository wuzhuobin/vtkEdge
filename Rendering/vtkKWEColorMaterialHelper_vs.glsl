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
// Id: $Id: vtkKWEColorMaterialHelper_vs.glsl 508 2008-08-19 14:21:07Z utkarsh $

#define GL_AMBIENT 1
#define GL_DIFFUSE 2
#define GL_SPECULAR 3
#define GL_AMBIENT_AND_DIFFUSE 4
#define GL_EMISSION 5

uniform int vtkKWEColorMaterialHelper_Mode;
gl_MaterialParameters getMaterialParameters()
{
  if (vtkKWEColorMaterialHelper_Mode == 0)
    {
    return gl_FrontMaterial;
    }

  gl_MaterialParameters materialParams = gl_FrontMaterial;
  if (vtkKWEColorMaterialHelper_Mode == GL_AMBIENT)
    {
    materialParams.ambient = gl_Color;
    }
  else if (vtkKWEColorMaterialHelper_Mode == GL_DIFFUSE)
    {
    materialParams.diffuse = gl_Color;
    }
  else if (vtkKWEColorMaterialHelper_Mode == GL_SPECULAR)
    {
    materialParams.specular = gl_Color;
    }
  else if (vtkKWEColorMaterialHelper_Mode == GL_AMBIENT_AND_DIFFUSE)
    {
    materialParams.ambient = gl_Color;
    materialParams.diffuse = gl_Color;
    }
  else if (vtkKWEColorMaterialHelper_Mode == GL_EMISSION)
    {
    materialParams.emission = gl_Color;
    }
  return materialParams;
}


