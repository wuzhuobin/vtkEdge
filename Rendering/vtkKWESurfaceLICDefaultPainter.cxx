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
#include "vtkKWESurfaceLICDefaultPainter.h"

#include "vtkGarbageCollector.h"
#include "vtkKWESurfaceLICPainter.h"
#include "vtkObjectFactory.h"
#include "vtkScalarsToColorsPainter.h"

vtkStandardNewMacro(vtkKWESurfaceLICDefaultPainter);
vtkCxxRevisionMacro(vtkKWESurfaceLICDefaultPainter, "$Revision: 414 $");
vtkCxxSetObjectMacro(vtkKWESurfaceLICDefaultPainter, SurfaceLICPainter, vtkKWESurfaceLICPainter);
//----------------------------------------------------------------------------
vtkKWESurfaceLICDefaultPainter::vtkKWESurfaceLICDefaultPainter()
{
  this->SurfaceLICPainter = vtkKWESurfaceLICPainter::New();
}

//----------------------------------------------------------------------------
vtkKWESurfaceLICDefaultPainter::~vtkKWESurfaceLICDefaultPainter()
{
  this->SetSurfaceLICPainter(0);
}

//----------------------------------------------------------------------------
void vtkKWESurfaceLICDefaultPainter::BuildPainterChain()
{
  this->Superclass::BuildPainterChain();
  
  // Now insert the SurfaceLICPainter after the scalar to colors painter.
  vtkPainter* stc = this->GetScalarsToColorsPainter();
  
  this->SurfaceLICPainter->SetDelegatePainter(stc->GetDelegatePainter());
  stc->SetDelegatePainter(this->SurfaceLICPainter);
}

//----------------------------------------------------------------------------
void vtkKWESurfaceLICDefaultPainter::ReportReferences(vtkGarbageCollector *collector)
{
  this->Superclass::ReportReferences(collector);
  vtkGarbageCollectorReport(collector, this->SurfaceLICPainter,
    "SurfaceLICPainter");
}

//----------------------------------------------------------------------------
void vtkKWESurfaceLICDefaultPainter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "SurfaceLICPainter: " << this->SurfaceLICPainter << endl;
}

