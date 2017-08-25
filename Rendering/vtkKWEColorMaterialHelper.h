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
// .NAME vtkKWEColorMaterialHelper - a helper to assist in similating the
// ColorMaterial behaviour of the default OpenGL pipeline.
// .SECTION Description
// vtkKWEColorMaterialHelper is a helper to assist in similating the
// ColorMaterial behaviour of the default OpenGL pipeline. Look at
// vtkKWEColorMaterialHelper_s for available GLSL functions.

#ifndef __vtkKWEColorMaterialHelper_h
#define __vtkKWEColorMaterialHelper_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkShaderProgram2;

class VTKEdge_RENDERING_EXPORT vtkKWEColorMaterialHelper : public vtkObject
{
public:
  static vtkKWEColorMaterialHelper* New();
  vtkTypeRevisionMacro(vtkKWEColorMaterialHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  void Initialize(vtkShaderProgram2*);
  vtkGetObjectMacro(Shader, vtkShaderProgram2);

  // Description:
  // Prepares the shader i.e. reads color material paramters state from OpenGL. 
  // This must be called before the shader is bound. 
  void PrepareForRendering();

  // Description:
  // Uploads any uniforms needed. This must be called only
  // after the shader has been bound, but before rendering the geometry.
  void Render();

//BTX
protected:
  vtkKWEColorMaterialHelper();
  ~vtkKWEColorMaterialHelper();

  void SetShader(vtkShaderProgram2*);
  vtkShaderProgram2* Shader;

  enum eMaterialParamater
    {
    DISABLED = 0,
    AMBIENT = 1,
    DIFFUSE = 2,
    SPECULAR = 3,
    AMBIENT_AND_DIFFUSE = 4,
    EMISSION = 5
    };
  eMaterialParamater Mode;

private:
  vtkKWEColorMaterialHelper(const vtkKWEColorMaterialHelper&); // Not implemented.
  void operator=(const vtkKWEColorMaterialHelper&); // Not implemented.
//ETX
};

#endif
