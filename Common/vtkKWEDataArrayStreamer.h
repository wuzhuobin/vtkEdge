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
// .NAME vtkKWEDataArrayStreamer
// .SECTION Description
// Compute 2D texture image extent to transfer any data array as 2D image
// chunks to GPU memory. It is also used to transfer back 2D texture chunk
// as part of a data array.
// .SECTION See Also
// Used by the vtkKWEGPUArrayCalculator.

#ifndef __vtkKWEDataArrayStreamer_h
#define __vtkKWEDataArrayStreamer_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_COMMON_EXPORT vtkKWEDataArrayStreamer : public vtkObject
{
 public:
  static vtkKWEDataArrayStreamer *New();
  vtkTypeRevisionMacro(vtkKWEDataArrayStreamer,vtkObject);
  void PrintSelf(ostream &os,vtkIndent indent);

  // Description:
  // Data array to split. Initial value is 0.
  vtkSetMacro(NumberOfTuples, vtkIdType);
  vtkGetMacro(NumberOfTuples, vtkIdType);

  // Description:
  // Maximum size of any dimension of a 3D texture. Initial value is 16,
  // the minimal value of the OpenGL specifications.
  vtkSetMacro(Max3DTextureSize,int);
  vtkGetMacro(Max3DTextureSize,int);
  
  // Description:
  // Maximum size of any dimension of a 1D or 2D texture. Initial value is 64,
  // the minimal value of the OpenGL specifications.
  vtkSetMacro(MaxTextureSize,int);
  vtkGetMacro(MaxTextureSize,int);
  
  // Description:
  // User-defined maximum size in bytes of a texture in GPU memory.
  // Initial value is 134217728 bytes (128*2^20=128Mb).
  // In a 32-bit build, this ivar can encode up to 4GB.
  // A null value means no limit.
  vtkSetMacro(MaxTextureMemorySizeInBytes,vtkIdType);
  vtkGetMacro(MaxTextureMemorySizeInBytes,vtkIdType);
  
  // Description:
  // Size of a tuple in bytes (number of components*size of type).
  // If the streamer is used by several dataarray at the same time,
  // the user should set its value to the maximum tuple size.
  // Initial value is 1.
  vtkSetMacro(TupleSize,int);
  vtkGetMacro(TupleSize,int);

  // Description:
  // User-defined maximum number of tuples per block. If 0, this value is
  // ignored. Initial value is 0.
  vtkSetMacro(MaxNumberOfTuplesPerBlock,vtkIdType);
  vtkGetMacro(MaxNumberOfTuplesPerBlock,vtkIdType);

  // Description:
  // Place the cursor on the first chunk, if any.
  void Begin();
  
  // Description:
  // Is the iteration over?
  bool IsDone();
  
  // Description:
  // Go the next chunk, if any.
  // \pre not_done: !IsDone()
  void Next();

  // Description:
  // Number of colums and number of rows of the current image chunk.
  // \pre  not_done: !IsDone()
  vtkGetVector2Macro(ChunkDimensions,vtkIdType);

  // Description:
  // Current starting tuple position in the data array.
  // \pre  not_done: !IsDone()
  vtkGetMacro(Cursor,vtkIdType);

//BTX
protected:
  vtkKWEDataArrayStreamer();
  ~vtkKWEDataArrayStreamer();

  void UpdateChunkDimensions();

  vtkIdType NumberOfTuples;

  int Max3DTextureSize;
  int MaxTextureSize;
  vtkIdType MaxTextureMemorySizeInBytes;
  int TupleSize;
  vtkIdType MaxNumberOfTuplesPerBlock;

  vtkIdType ChunkDimensions[2];
  vtkIdType Cursor;
  vtkIdType Step;

  vtkIdType Stage2Cursor;
  vtkIdType Stage2Step;
  vtkIdType Stage3Cursor;
  vtkIdType Stage3Step;
  vtkIdType Stage2NumberOfRows;

private:
  vtkKWEDataArrayStreamer(const vtkKWEDataArrayStreamer&); // Not implemented.
  void operator=(const vtkKWEDataArrayStreamer&); // Not implemented.
//ETX
};

#endif
