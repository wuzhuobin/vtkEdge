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

#include "vtkKWEGlyphSelectionRenderMode.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkOpenGL.h"
#include "vtkCamera.h"
#include "vtkKWEGlyph3DMapper.h"
#include "vtkKWEVisibleGlyphSelector.h"
#include "vtkPainterDeviceAdapter.h"

vtkCxxRevisionMacro(vtkKWEGlyphSelectionRenderMode, "$Revision: 566 $");
vtkStandardNewMacro(vtkKWEGlyphSelectionRenderMode);
// ----------------------------------------------------------------------------
vtkKWEGlyphSelectionRenderMode::vtkKWEGlyphSelectionRenderMode()
{
}
// ----------------------------------------------------------------------------
vtkKWEGlyphSelectionRenderMode::~vtkKWEGlyphSelectionRenderMode()
{
}

// ----------------------------------------------------------------------------
void vtkKWEGlyphSelectionRenderMode::Render(vtkRenderer *r)
{
  // glyph selection mode.

  r->GetRenderWindow()->MakeCurrent();

  // standard render method 

  this->UpdateCamera(r);

  // set matrix mode for actors 
  glMatrixMode(GL_MODELVIEW);

  this->UpdateGeometry(r);

  // clean up the model view matrix set up by the camera 
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

// ----------------------------------------------------------------------------
void vtkKWEGlyphSelectionRenderMode::UpdateCamera(vtkRenderer *r)
{
  vtkCamera *c=r->GetActiveCamera();
  c->Render(r);
}
// ----------------------------------------------------------------------------
void vtkKWEGlyphSelectionRenderMode::UpdateGeometry(vtkRenderer *r)
{
  vtkProp **propArray;
  int propArrayCount;

  vtkPropCollection *props=r->GetViewProps();

  vtkIdType c=props->GetNumberOfItems();
  if(c>0)
    {
    propArray=new vtkProp*[c];
    }
  else
    {
    propArray=0;
    }
  propArrayCount = 0;
  vtkCollectionSimpleIterator pit;
  vtkProp *aProp=0;
  for ( props->InitTraversal(pit); 
    (aProp = props->GetNextProp(pit)); )
    {
    if ( aProp->GetVisibility() )
      {
      propArray[propArrayCount++] = aProp;
      }
    }

  if ( propArrayCount == 0 )
    {
    vtkDebugMacro( << "There are no visible props!" );
    }

  //change the renderer's background to black, which will indicate a miss
  double origBG[3];
  r->GetBackground(origBG);
  r->SetBackground(0.0,0.0,0.0);
  bool origGrad = r->GetGradientBackground();
  r->GradientBackgroundOff();
  r->Clear();

  vtkPainterDeviceAdapter *device=r->GetRenderWindow()->
    GetPainterDeviceAdapter();
  int origMultisample = device->QueryMultisampling();
  int origLighting = device->QueryLighting();
  int origBlending = device->QueryBlending();

  device->MakeMultisampling(0);
  device->MakeLighting(0);
  device->MakeBlending(0);

  //render the props

  for (int i = 0; i < propArrayCount; i++ )
    {
    vtkProp *prop=propArray[i];
    if(prop->GetVisibility())
      {
      vtkActor *actor = vtkActor::SafeDownCast(prop);
      if (actor && !(actor->IsA("vtkFollower")||actor->IsA("vtkLODActor")))
        {
        vtkKWEGlyph3DMapper *mapper=
          vtkKWEGlyph3DMapper::SafeDownCast(actor->GetMapper());

        if(mapper!=0)
          {
          if(actor->GetPickable())
            {
            if (this->SelectMode == vtkKWEVisibleGlyphSelector::COLOR_BY_PROCESSOR)
              {
              mapper->SetSelectionColorId(this->SelectConst);
              }
            if (this->SelectMode == vtkKWEVisibleGlyphSelector::COLOR_BY_ACTOR)
              {
              mapper->SetSelectionColorId(i+1); // 0 is for miss.
              }
            mapper->SetSelectMode(this->SelectMode);
            prop->RenderOpaqueGeometry(r);
            }
          else
            {
            mapper->SetSelectionColorId(0); // to update the z-buffer
            mapper->SetSelectMode(vtkKWEVisibleGlyphSelector::COLOR_BY_PROCESSOR);
            prop->RenderOpaqueGeometry(r);
            }
          mapper->SetSelectMode(vtkKWEVisibleGlyphSelector::NOT_SELECTING);
          }
        }
      }
    }

  //reset lighting back to the default
  device->MakeBlending(origBlending);
  device->MakeLighting(origLighting);
  device->MakeMultisampling(origMultisample);

  //restore original background
  r->SetBackground(origBG);
  r->SetGradientBackground(origGrad);

  // Clean up the space we allocated before. If the PropArray exists,
  // they all should exist
  if (propArray!=0)
    {
    delete [] propArray;
    }
}

// ----------------------------------------------------------------------------
void vtkKWEGlyphSelectionRenderMode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
