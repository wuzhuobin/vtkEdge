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

#include "vtkKWETessellatedBoxSource.h"
#include "vtkXMLPolyDataWriter.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkXMLHierarchicalBoxDataReader.h"
#include "vtkStructuredPoints.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolume.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkRegressionTestImage.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkTestUtilities.h"
#include "vtkHierarchicalBoxDataSet.h"
#include "vtkUniformGrid.h"
#include "vtkOutlineFilter.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCellDataToPointData.h"
#include "vtkKWEClipConvexPolyData.h"
#include "vtkPlaneCollection.h"
#include "vtkPlane.h"

int TestTessellatedBoxSource(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  vtkKWETessellatedBoxSource *boxSource=vtkKWETessellatedBoxSource::New();
  boxSource->SetBounds(0,1,0,1,0,1);
  boxSource->QuadsOn();
//  boxSource->DuplicateSharedPointsOn();
  boxSource->SetLevel(4);
  boxSource->Update();
  vtkXMLPolyDataWriter *writer=vtkXMLPolyDataWriter::New();
  writer->SetInputConnection(boxSource->GetOutputPort());
  boxSource->Delete();
  writer->SetFileName("box.vtp");
  writer->SetDataModeToAscii();
  writer->Update();
  
  vtkKWEClipConvexPolyData *clip=vtkKWEClipConvexPolyData::New();
  clip->SetInputConnection(boxSource->GetOutputPort());
  
  vtkPlaneCollection *planes=vtkPlaneCollection::New();
  clip->SetPlanes(planes);
  planes->Delete();
  
  vtkPlane *p=vtkPlane::New();
  planes->AddItem(p);
  p->Delete();
  
  double origin[3]={0.5,0.5,0.5};
  double direction[3]={0,0,1};
  
  p->SetOrigin( origin );
  p->SetNormal( direction );
  planes->AddItem(p);
  
  vtkXMLPolyDataWriter *writer2=vtkXMLPolyDataWriter::New();
  writer2->SetInputConnection(clip->GetOutputPort());
  clip->Delete();
  writer2->SetFileName("clipbox.vtp");
  writer2->SetDataModeToAscii();
  writer2->Update();
  writer2->Delete();
  
  writer->Delete();  

  return 0; // 0==success.
}
