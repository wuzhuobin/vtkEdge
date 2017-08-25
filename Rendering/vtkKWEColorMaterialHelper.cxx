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
#include "vtkKWEColorMaterialHelper.h"

#include "vtkShaderProgram2.h"
#include "vtkObjectFactory.h"
#include "vtkShader2.h"
#include "vtkShader2Collection.h"
#include "vtkUniformVariables.h"
#include "vtkgl.h"
extern const char* vtkKWEColorMaterialHelper_vs;

vtkStandardNewMacro(vtkKWEColorMaterialHelper);
vtkCxxRevisionMacro(vtkKWEColorMaterialHelper, "$Revision: 644 $");
vtkCxxSetObjectMacro(vtkKWEColorMaterialHelper, Shader, vtkShaderProgram2);
//----------------------------------------------------------------------------
vtkKWEColorMaterialHelper::vtkKWEColorMaterialHelper()
{
  this->Shader = 0;
}

//----------------------------------------------------------------------------
vtkKWEColorMaterialHelper::~vtkKWEColorMaterialHelper()
{
  this->SetShader(0);
}

//----------------------------------------------------------------------------
void vtkKWEColorMaterialHelper::Initialize(vtkShaderProgram2* pgm)
{
  if (this->Shader != pgm)
    {
    this->SetShader(pgm);
    if (pgm)
      {
      vtkShader2 *s=vtkShader2::New();
      s->SetSourceCode(vtkKWEColorMaterialHelper_vs);
      s->SetType(VTK_SHADER_TYPE_VERTEX);
      s->SetContext(pgm->GetContext());
      pgm->GetShaders()->AddItem(s);
      s->Delete();
      }
    }
}
//----------------------------------------------------------------------------
void vtkKWEColorMaterialHelper::PrepareForRendering()
{
  if (!this->Shader)
    {
    vtkErrorMacro("Please Initialize() before calling PrepareForRendering().");
    return ;
    }

  this->Mode = vtkKWEColorMaterialHelper::DISABLED;
  if (glIsEnabled(GL_COLOR_MATERIAL))
    {
    GLint colorMaterialParameter;
    glGetIntegerv(GL_COLOR_MATERIAL_PARAMETER, &colorMaterialParameter);
    switch (colorMaterialParameter)
      {
    case GL_AMBIENT:
      this->Mode = vtkKWEColorMaterialHelper::AMBIENT;
      break;

    case GL_DIFFUSE:
      this->Mode = vtkKWEColorMaterialHelper::DIFFUSE;
      break;

    case GL_SPECULAR:
      this->Mode = vtkKWEColorMaterialHelper::SPECULAR;
      break;

    case GL_AMBIENT_AND_DIFFUSE:
      this->Mode = vtkKWEColorMaterialHelper::AMBIENT_AND_DIFFUSE;
      break;

    case GL_EMISSION:
      this->Mode = vtkKWEColorMaterialHelper::EMISSION;
      break;
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEColorMaterialHelper::Render()
{
  if (!this->Shader)
    {
    vtkErrorMacro("Please Initialize() before calling Render().");
    return;
    }
  
  int value=this->Mode;
  this->Shader->GetUniformVariables()->SetUniformi("vtkKWEColorMaterialHelper_Mode",1,&value);
}

//----------------------------------------------------------------------------
void vtkKWEColorMaterialHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Shader: " << this->Shader << endl;
}
