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
#include "vtkKWEExtentCalculator.h"

#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"

#include "assert.h"
#include "vtkgl.h"

static void vtkInvalidateExtent(int ext[6])
{
  ext[0] = ext[2] = ext[4] = 0;
  ext[1] = ext[3] = ext[5] = -1;
}

static bool vtkIsValid(int ext[6])
{
  return (ext[1] >= ext[0] &&
    ext[3] >= ext[2] &&
    ext[5] >= ext[4]);
}

vtkStandardNewMacro(vtkKWEExtentCalculator);
vtkCxxRevisionMacro(vtkKWEExtentCalculator, "$Revision: 1774 $");
//----------------------------------------------------------------------------
vtkKWEExtentCalculator::vtkKWEExtentCalculator()
{
  vtkInvalidateExtent(this->WholeExtent);
  vtkInvalidateExtent(this->InChunkExtent);
  vtkInvalidateExtent(this->OutChunkExtent);
  this->GhostLevels[0] = this->GhostLevels[1] = this->GhostLevels[2] = 0;
  this->ChunkDescription = XYZ_GRID;

  this->Max3DTextureSize=16; // 16 is the minimum value of the OpenGL spec.
  this->MaxTextureSize=64; // 64 is the minimum value of the OpenGL spec.
  this->MaxTextureMemorySizeInBytes=128*1024*1024;
  this->InTupleSize=1;
  this->OutTupleSize=1;
}

//----------------------------------------------------------------------------
vtkKWEExtentCalculator::~vtkKWEExtentCalculator()
{
}

//----------------------------------------------------------------------------
void vtkKWEExtentCalculator::LoadLimits(vtkRenderWindow* renWin)
{
  renWin->MakeCurrent();

  GLint value;
  glGetIntegerv(vtkgl::MAX_3D_TEXTURE_SIZE,&value);
  this->SetMax3DTextureSize(value);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&value);
  this->SetMaxTextureSize(value);
}

//----------------------------------------------------------------------------
void vtkKWEExtentCalculator::Begin()
{
  if (!::vtkIsValid(this->WholeExtent))
    {
    return;
    }

  this->Order[0]=0;
  this->Order[1]=1;
  this->Order[2]=2;
  this->Max[0]=this->WholeExtent[1]-this->WholeExtent[0]+1;
  this->Max[1]=this->WholeExtent[3]-this->WholeExtent[2]+1;
  this->Max[2]=this->WholeExtent[5]-this->WholeExtent[4]+1;
  this->Cursor[0]=0;
  this->Cursor[1]=0;
  this->Cursor[2]=0;
  this->Step[0]=this->WholeExtent[1]-this->WholeExtent[0]+1;
  this->Step[1]=this->WholeExtent[3]-this->WholeExtent[2]+1;
  this->Step[2]=this->WholeExtent[5]-this->WholeExtent[4]+1;

  // The challenging work is here.

  switch (this->ChunkDescription)
    {
  case XY_PLANE:
    this->Order[0]=2;

    this->Order[1]=1;
    this->Order[2]=0;

    this->Step[2]=1;
    break;

  case YZ_PLANE:
    this->Order[0]=0;

    this->Order[1]=2;
    this->Order[2]=1;

    this->Step[0]=1;
    break;

  case XZ_PLANE:
    this->Order[0]=1;

    this->Order[1]=2;
    this->Order[2]=0;

    this->Step[1]=1;
    break;

  case XYZ_GRID:
    this->Order[0]=2;
    this->Order[1]=1;
    this->Order[2]=0;
    break;
    }

  // Hardware index limits:
  int maxSize=this->Max3DTextureSize;
  if(this->ChunkDescription!=XYZ_GRID && this->GhostLevels[this->Order[0]]==0)
    {
    maxSize=this->MaxTextureSize;
    }

  int comp=0;
  while(comp<3)
    {
    if(this->Step[comp]>maxSize)
      {
      this->Step[comp]=maxSize-2*this->GhostLevels[comp];
      }
    ++comp;
    }

  // User limits and datatype:
  if(this->MaxTextureMemorySizeInBytes>0)
    {
    int maxTupleSize;
    if(this->InTupleSize>this->OutTupleSize)
      {
      maxTupleSize=this->InTupleSize;
      }
    else
      {
      maxTupleSize=this->OutTupleSize;
      }

    int currentSize=this->Step[0]*this->Step[1]*this->Step[2]*maxTupleSize;
    int ratio=currentSize/this->MaxTextureMemorySizeInBytes; // can be 0.
    int remainder=currentSize-ratio*this->MaxTextureMemorySizeInBytes;
    int compToShrink=0;
    while(compToShrink<3 && (ratio>=1 && remainder>0))
      {
      if(this->Step[Order[compToShrink]]>1)
        {
        if(ratio>this->Step[Order[compToShrink]])
          {
          this->Step[Order[compToShrink]]=1;
          }
        else
          {
          this->Step[Order[compToShrink]]/=ratio;
          }
        }
      currentSize=this->Step[0]*this->Step[1]*this->Step[2]*maxTupleSize;
      ratio=currentSize/this->MaxTextureMemorySizeInBytes;
      remainder=currentSize-ratio*this->MaxTextureMemorySizeInBytes;
      ++compToShrink;
      }
    }

  this->ComputeChunkExtents();
}

//----------------------------------------------------------------------------
bool vtkKWEExtentCalculator::IsDone()
{
  // Check for the most outer loop
  return this->Cursor[this->Order[0]]>=this->Max[this->Order[0]];
}

//----------------------------------------------------------------------------
void vtkKWEExtentCalculator::Next()
{
  assert("pre: not_done" && !this->IsDone());

  // Most inner loop first
  this->Cursor[this->Order[2]]+=this->Step[this->Order[2]];
  if(this->Cursor[this->Order[2]]>=this->Max[this->Order[2]])
    {
    this->Cursor[this->Order[2]]=0;
    this->Cursor[this->Order[1]]+=this->Step[this->Order[1]];
    if(this->Cursor[this->Order[1]]>=this->Max[this->Order[1]])
      {
      this->Cursor[this->Order[1]]=0;
      this->Cursor[this->Order[0]]+=this->Step[this->Order[0]];
      }
    }
  this->ComputeChunkExtents();
}

//----------------------------------------------------------------------------
void vtkKWEExtentCalculator::ComputeChunkExtents()
{
  vtkInvalidateExtent(this->InChunkExtent);
  vtkInvalidateExtent(this->OutChunkExtent);
  if (this->IsDone())
    {
    return;
    }

  int coord=0;
  while(coord<3)
    {
    this->InChunkExtent[2*coord]=this->Cursor[coord]-this->GhostLevels[coord];
    if(this->InChunkExtent[2*coord]<this->WholeExtent[2*coord])
      {
      this->InChunkExtent[2*coord]=this->WholeExtent[2*coord];
      }
    this->InChunkExtent[2*coord+1]=this->Cursor[coord]+this->Step[coord]-1
      +this->GhostLevels[coord];
    if(this->InChunkExtent[2*coord+1]>this->WholeExtent[2*coord+1])
      {
      this->InChunkExtent[2*coord+1]=this->WholeExtent[2*coord+1];
      }

    // Should not need to clamp the output. It should be in the right range
    // by construction.
    this->OutChunkExtent[2*coord]=this->Cursor[coord];
    this->OutChunkExtent[2*coord+1]=this->Cursor[coord]+this->Step[coord]-1;
    ++coord;
    }

  assert("post: validate InChunkExtent" && vtkIsValid(this->InChunkExtent));
  assert("post: validate OutChunkExtent" && vtkIsValid(this->OutChunkExtent));
}

//----------------------------------------------------------------------------
void vtkKWEExtentCalculator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
