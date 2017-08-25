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
#include "vtkKWERenderingSerializationHelper.h"

#include "vtkCamera.h"
#include "vtkKWESerializationHelperMap.h"
#include "vtkKWESerializer.h"
#include "vtkKWEXMLElement.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkKWERenderingSerializationHelper, "$Revision: 752 $");
vtkStandardNewMacro(vtkKWERenderingSerializationHelper);

//-----------------------------------------------------------------------------
vtkKWERenderingSerializationHelper::vtkKWERenderingSerializationHelper()
{
}

//-----------------------------------------------------------------------------
int vtkKWERenderingSerializationHelper::Serialize(vtkObject *object, 
                                                  vtkKWESerializer *serializer)
{
  if (vtkCamera::SafeDownCast(object))
    {
    this->SerializeCamera(vtkCamera::SafeDownCast(object), serializer);
    return 1;
    }

  return 0;
}

//-----------------------------------------------------------------------------
void vtkKWERenderingSerializationHelper::RegisterWithHelperMap() 
{
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkCamera", this);
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkOpenGLCamera", this);
  vtkKWESerializationHelperMap::RegisterHelperForClass("vtkMesaCamera", this);
}

//-----------------------------------------------------------------------------
void vtkKWERenderingSerializationHelper::UnRegisterWithHelperMap() 
{
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkCamera", this);
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkOpenGLCamera", this);
  vtkKWESerializationHelperMap::UnRegisterHelperForClass("vtkMesaCamera", this);
}

//-----------------------------------------------------------------------------
void vtkKWERenderingSerializationHelper::SerializeCamera(vtkCamera *camera,
                                                         vtkKWESerializer *serializer)
{
  if (serializer->IsWriting())
    {
    // vectors of length 3
    unsigned int length = 3;
    double *position = camera->GetPosition();
    serializer->Serialize("Position", position, length);
    double *focalPt = camera->GetFocalPoint();
    serializer->Serialize("FocalPoint", focalPt, length);
    double *viewUp = camera->GetViewUp();
    serializer->Serialize("ViewUp", viewUp, length);

    // vectors of length 2
    length = 2;
    double *clippingRange = camera->GetClippingRange();
    serializer->Serialize("ClippingRange", clippingRange, length);
    double *windowCenter = camera->GetWindowCenter();
    serializer->Serialize("WindowCenter", windowCenter, length);

    // scalars
    double viewAngle = camera->GetViewAngle();
    serializer->Serialize("ViewAngle", viewAngle);
    int parallelProjection = camera->GetParallelProjection();
    serializer->Serialize("ParallelProjection", parallelProjection);
    int useHorizontalViewAngle = camera->GetUseHorizontalViewAngle();
    serializer->Serialize("UseHorizontalViewAngle", useHorizontalViewAngle);
    }
  else 
    {
    unsigned int length = 0;

    // vectors of length 3
    double *position = 0, *focalPt = 0, *viewUp = 0;
    serializer->Serialize("Position", position, length);
    if (length > 0)
      {
      camera->SetPosition(position);
      delete [] position;
      }
    serializer->Serialize("FocalPoint", focalPt, length);
    if (length > 0)
      {
      camera->SetFocalPoint(focalPt);
      delete [] focalPt;
      }
    serializer->Serialize("ViewUp", viewUp, length);
    if (length > 0)
      {
      camera->SetViewUp(viewUp);
      delete [] viewUp;
      }

    // vectors of length 2
    double *clippingRange = 0, *windowCenter = 0;
    serializer->Serialize("ClippingRange", clippingRange, length);
    if (length > 0)
      {
      camera->SetClippingRange(clippingRange);
      delete [] clippingRange;
      }
    serializer->Serialize("WindowCenter", windowCenter, length);
    if (length > 0)
      {
      camera->SetWindowCenter(windowCenter[0], windowCenter[1]);
      delete [] windowCenter;
      }

    // scalars
    double viewAngle = camera->GetViewAngle(); // default value
    serializer->Serialize("ViewAngle", viewAngle);
    camera->SetViewAngle(viewAngle);

    int parallelProjection = camera->GetParallelProjection();
    serializer->Serialize("ParallelProjection", parallelProjection);
    camera->SetParallelProjection(parallelProjection);

    int useHorizontalViewAngle = camera->GetUseHorizontalViewAngle();
    serializer->Serialize("UseHorizontalViewAngle", useHorizontalViewAngle);
    camera->SetUseHorizontalViewAngle(useHorizontalViewAngle);
    }
}


//-----------------------------------------------------------------------------
const char *vtkKWERenderingSerializationHelper::GetSerializationType(vtkObject *object)
{
  if (vtkCamera::SafeDownCast(object))
    {
    return "vtkCamera";
    }
  return 0;
}

//-----------------------------------------------------------------------------
void vtkKWERenderingSerializationHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Supported ClassTypes:\n";
  os << indent.GetNextIndent() << "vtkCamera\n";
  os << indent.GetNextIndent() << "vtkOpenGLCamera\n";
  os << indent.GetNextIndent() << "vtkMesaCamera\n";
}
