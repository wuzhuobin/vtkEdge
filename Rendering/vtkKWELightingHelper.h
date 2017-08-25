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
// .NAME vtkKWELightingHelper - helper to assist in simulating lighting similar
// to default OpenGL pipeline.
// .SECTION Description
// vtkKWELightingHelper is an helper to assist in simulating lighting similar
// to default OpenGL pipeline. Look at vtkKWELightingHelper_s for available
// GLSL functions.

#ifndef __vtkKWELightingHelper_h
#define __vtkKWELightingHelper_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkShader2.h" // for vtkShader2Type

class vtkShaderProgram2;

class VTKEdge_RENDERING_EXPORT vtkKWELightingHelper : public vtkObject
{
public:
  static vtkKWELightingHelper* New();
  vtkTypeRevisionMacro(vtkKWELightingHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get/Set the shader program to which we want to add the lighting kernels.
  // mode = VTK_SHADER_TYPE_VERTEX or VTK_SHADER_TYPE_FRAGMENT
  // depending on whether the vertex lighting or fragment lighting is to be
  // used.
  void Initialize(vtkShaderProgram2 *shader, 
                  vtkShader2Type mode);
  vtkGetObjectMacro(Shader, vtkShaderProgram2);

  // Description:
  // Updates any lighting specific information needed. 
  // This must be called before the shader program is bound.
  void PrepareForRendering();

//BTX
protected:
  vtkKWELightingHelper();
  ~vtkKWELightingHelper();

  void SetShader(vtkShaderProgram2 *shader);
  vtkShaderProgram2 *Shader;

private:
  vtkKWELightingHelper(const vtkKWELightingHelper&); // Not implemented.
  void operator=(const vtkKWELightingHelper&); // Not implemented.
//ETX
};

#endif


