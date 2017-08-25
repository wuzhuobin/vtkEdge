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
#include "vtkKWEVoxelAlignedImageActorPointPlacer.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"

vtkCxxRevisionMacro(vtkKWEVoxelAlignedImageActorPointPlacer, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEVoxelAlignedImageActorPointPlacer);

//----------------------------------------------------------------------
vtkKWEVoxelAlignedImageActorPointPlacer::vtkKWEVoxelAlignedImageActorPointPlacer()
{
}

//----------------------------------------------------------------------
vtkKWEVoxelAlignedImageActorPointPlacer::~vtkKWEVoxelAlignedImageActorPointPlacer()
{
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::ComputeWorldPosition( vtkRenderer *ren,
                                                    double  displayPos[2],
                                                    double *refWorldPos,
                                                    double  worldPos[3],
                                                    double  worldOrient[9] )
{
  return (this->Superclass::ComputeWorldPosition( ren, displayPos,
                            refWorldPos, worldPos, worldOrient )
      && this->InternalValidateWorldPosition(worldPos));
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::InternalValidateWorldPosition(
                                                    double worldPos[3])
{
  double spacing[3], origin[3];
  int extent[6], imagePosition[3];
  this->ImageActor->GetInput()->GetExtent(extent);
  this->ImageActor->GetInput()->GetSpacing(spacing);
  this->ImageActor->GetInput()->GetOrigin(origin);

  imagePosition[0] = static_cast<int>((worldPos[0]-origin[0])/spacing[0] + 0.5);
  imagePosition[1] = static_cast<int>((worldPos[1]-origin[1])/spacing[1] + 0.5);
  imagePosition[2] = static_cast<int>((worldPos[2]-origin[2])/spacing[2] + 0.5);

  if (imagePosition[0] < extent[0] || imagePosition[0] > extent[1] ||
      imagePosition[1] < extent[2] || imagePosition[1] > extent[3] ||
      imagePosition[2] < extent[4] || imagePosition[2] > extent[5])
    {
    return 0;
    }

  worldPos[0] = imagePosition[0] * spacing[0] + origin[0];
  worldPos[1] = imagePosition[1] * spacing[1] + origin[1];
  worldPos[2] = imagePosition[2] * spacing[2] + origin[2];

  return 1;
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::ComputeWorldPosition( vtkRenderer *ren,
                                                    double displayPos[2],
                                                    double worldPos[3],
                                                    double worldOrient[9] )
{
  return (this->Superclass::ComputeWorldPosition(
             ren, displayPos, worldPos, worldOrient ) &&
          this->InternalValidateWorldPosition(worldPos));
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::ValidateWorldPosition(
  double worldPos[3],
  double *worldOrient)
{
  return this->Superclass::ValidateWorldPosition( worldPos, worldOrient ) &&
    this->InternalValidateWorldPosition(worldPos);
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::ValidateWorldPosition( double worldPos[3] )
{
  return this->Superclass::ValidateWorldPosition( worldPos ) &&
                                this->InternalValidateWorldPosition(worldPos);
}

//----------------------------------------------------------------------
int vtkKWEVoxelAlignedImageActorPointPlacer::UpdateWorldPosition( vtkRenderer *ren,
                                                   double worldPos[3],
                                                   double worldOrient[9] )
{
  return this->Superclass::UpdateWorldPosition( ren, worldPos, worldOrient )
      && this->InternalValidateWorldPosition(worldPos);
}

//----------------------------------------------------------------------
void vtkKWEVoxelAlignedImageActorPointPlacer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

