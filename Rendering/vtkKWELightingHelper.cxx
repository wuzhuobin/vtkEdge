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
#include "vtkKWELightingHelper.h"

#include "vtkObjectFactory.h"
#include "vtkgl.h"
#include "vtkShaderProgram2.h"
#include "vtkShader2Collection.h"

extern const char* vtkKWELightingHelper_s;

vtkStandardNewMacro(vtkKWELightingHelper);
vtkCxxRevisionMacro(vtkKWELightingHelper, "$Revision: 644 $");
vtkCxxSetObjectMacro(vtkKWELightingHelper, Shader, vtkShaderProgram2);
//----------------------------------------------------------------------------
vtkKWELightingHelper::vtkKWELightingHelper()
{
  this->Shader = 0;
}

//----------------------------------------------------------------------------
vtkKWELightingHelper::~vtkKWELightingHelper()
{
  this->SetShader(0);
}

//----------------------------------------------------------------------------
void vtkKWELightingHelper::Initialize(vtkShaderProgram2* pgm, 
  vtkShader2Type mode)
{
  if (this->Shader != pgm)
    {
    this->SetShader(pgm);
    if (pgm)
      {
      vtkShader2 *s=vtkShader2::New();
      s->SetSourceCode(vtkKWELightingHelper_s);
      s->SetType(mode);
      s->SetContext(this->Shader->GetContext());
      this->Shader->GetShaders()->AddItem(s);
      s->Delete();
      }
    }
}

//----------------------------------------------------------------------------
#define VTK_MAX_LIGHTS 8
void vtkKWELightingHelper::PrepareForRendering()
{
  GLint ivalue;
  glGetIntegerv(vtkgl::CURRENT_PROGRAM, &ivalue);
  if (ivalue != 0)
    {
    vtkErrorMacro("PrepareForRendering() cannot be called after a shader program has been bound.");
    return;
    }

  for (int cc=0; cc < VTK_MAX_LIGHTS; cc++)
    {
    // use the light's 4th diffuse component to store an enabled bit  
    GLfloat lightDiffuse[4];
    glGetLightfv(GL_LIGHT0 + cc, GL_DIFFUSE, lightDiffuse);

    // enable/disable the light for fixed function  
    if (glIsEnabled(GL_LIGHT0 + cc))
      {
      lightDiffuse[3] = 1;
      }
    else
      {
      lightDiffuse[3] = 0;
      }
    glLightfv(GL_LIGHT0 + cc, GL_DIFFUSE, lightDiffuse);
    }
}

//----------------------------------------------------------------------------
void vtkKWELightingHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Shader: " << this->Shader << endl;
}

