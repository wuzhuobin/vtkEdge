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
// .NAME vtkKWEVoxelAlignedImageActorPointPlacer - point placer that forces the placement of points to the nearest voxel center
// .SECTION Description
// vtkKWEVoxelAlignedImageActorPointPlacer is a point placer that works on
// image actors and automatically forces the placement of the point to
// the nearest voxel center. This is useful when using voxelized editing
// widgets and you'd like to see the shape be placed exactly over a voxel
// rather than in between two voxels.

#ifndef __vtkKWEVoxelAlignedImageActorPointPlacer_h
#define __vtkKWEVoxelAlignedImageActorPointPlacer_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkImageActorPointPlacer.h"

class VTKEdge_WIDGETS_EXPORT vtkKWEVoxelAlignedImageActorPointPlacer
                            : public vtkImageActorPointPlacer
{
public:
  // Description:
  // Instantiate this class.
  static vtkKWEVoxelAlignedImageActorPointPlacer *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(vtkKWEVoxelAlignedImageActorPointPlacer,
                       vtkImageActorPointPlacer);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Given and renderer and a display position in pixels,
  // find a world position and orientation. In this class
  // an internal vtkBoundedPlanePointPlacer is used to compute
  // the world position and orientation. The internal placer
  // is set to use the plane of the image actor and the bounds
  // of the image actor as the constraints for placing points.
  int ComputeWorldPosition( vtkRenderer *ren,
                            double displayPos[2],
                            double worldPos[3],
                            double worldOrient[9] );

  // Description:
  // This method is identical to the one above since the
  // reference position is ignored by the bounded plane
  // point placer.
  int ComputeWorldPosition( vtkRenderer *ren,
                            double displayPos[2],
                            double refWorldPos[2],
                            double worldPos[3],
                            double worldOrient[9] );

  // Description:
  // This method validates a world position by checking to see
  // if the world position is valid according to the constraints
  // of the internal placer (essentially - is this world position
  // on the image?)
  int ValidateWorldPosition( double worldPos[3] );

  // Description:
  // This method is identical to the one above since the bounded
  // plane point placer ignores orientation
  int ValidateWorldPosition( double worldPos[3],
                             double worldOrient[9]);


  // Description:
  // Update the world position and orientation according the
  // the current constraints of the placer. Will be called
  // by the representation when it notices that this placer
  // has been modified.
  int UpdateWorldPosition( vtkRenderer *ren,
                           double worldPos[3],
                           double worldOrient[9]);

protected:
  vtkKWEVoxelAlignedImageActorPointPlacer();
  ~vtkKWEVoxelAlignedImageActorPointPlacer();

  int InternalValidateWorldPosition(double worldPos[3]);

private:
  vtkKWEVoxelAlignedImageActorPointPlacer(
                 const vtkKWEVoxelAlignedImageActorPointPlacer&); //Not implemented
  void operator=(const vtkKWEVoxelAlignedImageActorPointPlacer&); //Not implemented
};

#endif
