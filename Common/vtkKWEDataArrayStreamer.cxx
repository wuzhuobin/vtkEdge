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
#include "vtkKWEDataArrayStreamer.h"

#include "vtkObjectFactory.h"
#include "assert.h"

vtkStandardNewMacro(vtkKWEDataArrayStreamer);
vtkCxxRevisionMacro(vtkKWEDataArrayStreamer, "$Revision: 1774 $");

// ----------------------------------------------------------------------------
vtkKWEDataArrayStreamer::vtkKWEDataArrayStreamer()
{
  this->NumberOfTuples=0;
  this->Max3DTextureSize=16; // 16 is the minimum value of the OpenGL spec.
  this->MaxTextureSize=64; // 64 is the minimum value of the OpenGL spec.
  this->MaxTextureMemorySizeInBytes=128*1024*1024;
  this->TupleSize=1;
  this->MaxNumberOfTuplesPerBlock=0;

  this->Cursor=0;
  this->Step=0;
  this->ChunkDimensions[0]=0;
  this->ChunkDimensions[1]=0;

  this->Stage2Cursor=0;
  this->Stage2Step=0;
  this->Stage2NumberOfRows=0;

  this->Stage3Cursor=0;
  this->Stage3Step=0;
}

// ----------------------------------------------------------------------------
vtkKWEDataArrayStreamer::~vtkKWEDataArrayStreamer()
{
}

//----------------------------------------------------------------------------
// Description:
// Place the cursor on the first chunk, if any.
void vtkKWEDataArrayStreamer::Begin()
{
  // There are three stages, all of them are optional
  // Stage 1: a bunch of images with the maximum possible size on the GPU
  // Stage 2: an image with the largest width but with the remaining number of
  // rows
  // Stage 3: an image of one row, with the remaining number of colums.


  // Hardware index limits:
  int maxSize=this->MaxTextureSize;
  vtkIdType maxStep=maxSize*maxSize;

  // User limits and datatype:
  if(this->MaxTextureMemorySizeInBytes>0)
    {
      int maxTupleSize=this->TupleSize;
      vtkIdType currentSize=maxStep*maxTupleSize;
      vtkIdType ratio=
        currentSize/this->MaxTextureMemorySizeInBytes; // can be 0.
      if(ratio>1)
        {
          if(maxStep>1)
            {
              if(ratio>maxStep)
                {
                  maxStep=1;
                }
              else
                {
                  maxStep/=ratio;
                }
            }
        }
    }

  if(this->MaxNumberOfTuplesPerBlock>0)
    {
      if(maxStep>this->MaxNumberOfTuplesPerBlock)
        {
          maxStep=this->MaxNumberOfTuplesPerBlock;
        }
    }

  // maxStep is the maximum possible step in the dataArray with all
  // included conditions above.

  // Can we do a full row?
  bool fullRow=maxStep>=maxSize;

  vtkIdType size=this->NumberOfTuples;

  vtkIdType stage1Step;

  if(!fullRow)
    {
      // we can not load a full row,

      // There are first `numberOfStage1Images' 1D texture (maxStep*1)...
      vtkIdType numberOfStage1Images=size/maxStep; // it can be 0
      stage1Step=maxStep;

      // ... followed by a last (stage3Step*1) image.
      this->Stage3Cursor=numberOfStage1Images*stage1Step;
      this->Stage3Step=size%maxStep; // can be 0

      this->ChunkDimensions[0]=maxStep;
      this->ChunkDimensions[1]=1;

       // just to have the variables initialized
      this->Stage2Cursor=this->Stage3Cursor;
      this->Stage2Step=0;
      this->Stage2NumberOfRows=0;
    }
  else
    {
      this->ChunkDimensions[0]=maxSize;

      // Stage 3.
      // last row will have stage3Step elements, one (stage3Step*1) image
      this->Stage3Step=size%maxSize; // can be 0.

      vtkIdType numberOfFullRows=size/maxSize; // can be 0.
      if(numberOfFullRows>0)
        {
          // if maxStep=maxSize*maxSize, maxHeight=maxSize.
        // here we take maxStep into account
          vtkIdType maxHeight=maxStep/maxSize;

          // Stage 1:
          // number of images (maxSize*maxHeight), can be 0
          vtkIdType numberOfFullImages=numberOfFullRows/maxHeight;

          // Stage 2:
          // number of remaining full rows, can be 0.
          // image is (maxSize*Stage2NumberOfRows)
          this->Stage2NumberOfRows=numberOfFullRows%maxHeight;

          stage1Step=maxSize*maxHeight;
          this->Stage2Cursor=numberOfFullImages*(maxSize*maxHeight);
          this->Stage2Step=maxSize*this->Stage2NumberOfRows;
          this->ChunkDimensions[1]=maxHeight;
        }
      else
        {
          stage1Step=0;
          this->Stage2Cursor=0;
          this->Stage2Step=0;
          this->Stage2NumberOfRows=0;
          this->ChunkDimensions[1]=1;
        }
      this->Stage3Cursor=this->Stage2Cursor+this->Stage2Step;
    }


  // start iteration
  this->Cursor=0;
  this->Step=stage1Step;
  if(!IsDone())
    {
      this->UpdateChunkDimensions();
    }

  // naive way:
  // size: size of dataArray in number of tuples
  // maxsize: hw index limit
  // a=size/maxsize=number of complete rows (can be 0)
  // b=size%maxsize=last row will have b elements (can be 0) -> stage 3
  // if(a>0)
  //  c=a/maxsize=number of complete images (can be 0) -> stage1
  //  d=a%maxsize=number of rows in stage 2 (can be 0) -> stage2
}

// ----------------------------------------------------------------------------
// Description:
// Is the iteration over?
bool vtkKWEDataArrayStreamer::IsDone()
{
  // Check for the most outer loop
  return this->Cursor>=this->NumberOfTuples;
}

// ----------------------------------------------------------------------------
// Description:
// Go the next chunk, if any.
// \pre not_done: !IsDone()
void vtkKWEDataArrayStreamer::Next()
{
  assert("pre: not_done" && !this->IsDone());

  this->Cursor+=this->Step;

  if(!this->IsDone())
    {
      this->UpdateChunkDimensions();
    }
}

// ----------------------------------------------------------------------------
// Description:
void vtkKWEDataArrayStreamer::UpdateChunkDimensions()
{
  if(this->Cursor==this->Stage3Cursor)
    {
      this->ChunkDimensions[0]=this->Stage3Step;
      this->ChunkDimensions[1]=1;
      this->Step=this->Stage3Step;
    }
  else
    {
      if(this->Cursor==this->Stage2Cursor)
        {
          this->ChunkDimensions[0]=this->MaxTextureSize;
          this->ChunkDimensions[1]=this->Stage2NumberOfRows;
          this->Step=this->Stage2Step;
        }
    }

  assert("post: dims_match_step"
         && (this->Step==this->ChunkDimensions[0]*this->ChunkDimensions[1]));

}

// ----------------------------------------------------------------------------
void vtkKWEDataArrayStreamer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
