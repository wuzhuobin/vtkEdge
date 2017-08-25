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
// .NAME vtkKWEExtentCalculator - facilitates iterating over structured data.
// .SECTION Description
// vtkKWEExtentCalculator facilitates iterating over structured data.
// Used to compute input/output image extents.

#ifndef __vtkKWEExtentCalculator_h
#define __vtkKWEExtentCalculator_h

#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkObject.h"
#include "vtkStructuredData.h" // for VTK_XY_PLANE, ...

class vtkRenderWindow;
class VTKEdge_HYBRID_EXPORT vtkKWEExtentCalculator : public vtkObject
{
public:
  static vtkKWEExtentCalculator* New();
  vtkTypeRevisionMacro(vtkKWEExtentCalculator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Input extent to split.
  vtkSetVector6Macro(WholeExtent, int);
  vtkGetVector6Macro(WholeExtent, int);

  // Description:
  // Rule governing how to split the data into chunks.
  vtkSetMacro(ChunkDescription, int);
  vtkGetMacro(ChunkDescription, int);

  //BTX
  enum ChunkDescriptions
    {
    XY_PLANE = VTK_XY_PLANE,
    YZ_PLANE = VTK_YZ_PLANE,
    XZ_PLANE = VTK_XZ_PLANE,
    XYZ_GRID = VTK_XYZ_GRID,
    DEFAULT = XYZ_GRID
    };
  //ETX

  // Description:
  // Set ghost levels for each pass.
  vtkSetVector3Macro(GhostLevels, int);
  vtkGetVector3Macro(GhostLevels, int);

  // Description:
  // Maximum size of any dimension of a 3D texture. Initial value is 16,
  // the minimal value of the OpenGL specifications.
  vtkSetMacro(Max3DTextureSize, int);
  vtkGetMacro(Max3DTextureSize, int);

  // Description:
  // Maximum size of any dimension of a 1D or 2D texture. Initial value is 64,
  // the minimal value of the OpenGL specifications.
  vtkSetMacro(MaxTextureSize, int);
  vtkGetMacro(MaxTextureSize, int);

  // Description:
  // User-defined maximum size in bytes of a texture in GPU memory.
  // Initial value is 134217728 bytes (128*2^20=128Mb).
  // As this ivar is a signed int, it cannot encode more than 2GB.
  // A negative value means no limit.
  vtkSetMacro(MaxTextureMemorySizeInBytes,int);
  vtkGetMacro(MaxTextureMemorySizeInBytes,int);

  // Description:
  // Size of a tuple of the incoming data in bytes.
  // Initial value is 1.
  vtkSetMacro(InTupleSize,int);
  vtkGetMacro(InTupleSize,int);

  // Description:
  // Size of a tuple of the output data in bytes.
  // Initial value is 1.
  vtkSetMacro(OutTupleSize,int);
  vtkGetMacro(OutTupleSize,int);

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
  // Returns the extent for the input for the current piece. This will be the
  // output extent plus the ghost levels, if any.
  // \pre  not_done: !IsDone()
  vtkGetVector6Macro(InChunkExtent, int);

  // Description:
  // Returns the extent for the output for the current piece.
  // \pre not_done: !IsDone()
  vtkGetVector6Macro(OutChunkExtent, int);

  // Description:
  // Load texture limits using the context.
  void LoadLimits(vtkRenderWindow* context);
//BTX
protected:
  vtkKWEExtentCalculator();
  ~vtkKWEExtentCalculator();

  int WholeExtent[6];
  int ChunkDescription;
  int GhostLevels[3];
  int InChunkExtent[6];
  int OutChunkExtent[6];

  int Max3DTextureSize;
  int MaxTextureSize;
  int MaxTextureMemorySizeInBytes;
  int InTupleSize;
  int OutTupleSize;

  //  int Piece;
  //  int NumberOfPieces;
  int Cursor[3]; // current position in i,j,k
  int Max[3]; // i goes from 0 to Max[0]-1, j goes from 0 Max[1]-1, ...
  int Order[3]; // Tells if we iterate over i then j then k (0,1,2),
  // over i,k,j (0,2,1), over j,k,i (1,2,0), j,i,k (1,0,2), k,i,j (2,0,1)
  // or k,j,i (2,1,0) Order[0]!=Order[1]!=Order[2]
  int Step[3]; // step for i, j  and k at each iteration step

  void ComputeChunkExtents();
private:
  vtkKWEExtentCalculator(const vtkKWEExtentCalculator&); // Not implemented.
  void operator=(const vtkKWEExtentCalculator&); // Not implemented.
//ETX
};

#endif


