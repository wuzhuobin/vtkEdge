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
#include "vtkKWEImageDataLIC2DExtentTranslator.h"

#include "vtkObjectFactory.h"
#include "vtkKWEImageDataLIC2D.h"

vtkStandardNewMacro(vtkKWEImageDataLIC2DExtentTranslator);
vtkCxxRevisionMacro(vtkKWEImageDataLIC2DExtentTranslator, "$Revision: 333 $");
vtkCxxSetObjectMacro(vtkKWEImageDataLIC2DExtentTranslator, InputExtentTranslator, vtkExtentTranslator);
//----------------------------------------------------------------------------
vtkKWEImageDataLIC2DExtentTranslator::vtkKWEImageDataLIC2DExtentTranslator()
{
  this->Algorithm = 0;
  this->InputExtentTranslator = 0;
  this->InputWholeExtent[0] = 
    this->InputWholeExtent[1] = 
    this->InputWholeExtent[2] = 
    this->InputWholeExtent[3] = 
    this->InputWholeExtent[4] = 
    this->InputWholeExtent[5] = 0;
}

//----------------------------------------------------------------------------
vtkKWEImageDataLIC2DExtentTranslator::~vtkKWEImageDataLIC2DExtentTranslator()
{
  this->SetInputExtentTranslator(0);
}

//----------------------------------------------------------------------------
void vtkKWEImageDataLIC2DExtentTranslator::SetAlgorithm(
  vtkKWEImageDataLIC2D* alg)
{
  if (this->Algorithm.GetPointer() != alg)
    {
    this->Algorithm = alg;
    this->Modified();
    }
}

//----------------------------------------------------------------------------
vtkKWEImageDataLIC2D* vtkKWEImageDataLIC2DExtentTranslator::GetAlgorithm()
{
  return this->Algorithm.GetPointer();
}

//----------------------------------------------------------------------------
int vtkKWEImageDataLIC2DExtentTranslator::PieceToExtentThreadSafe(int piece, int numPieces, 
                                     int ghostLevel, int *wholeExtent, 
                                     int *resultExtent, int splitMode, 
                                     int byPoints)
{
  if (!this->Algorithm)
    {
    return this->Superclass::PieceToExtentThreadSafe(piece, numPieces, ghostLevel, wholeExtent,
      resultExtent, splitMode, byPoints);
    }

  // Let the input extent translator do the translation.
  int inExt[6];
  this->InputExtentTranslator->PieceToExtentThreadSafe(piece, numPieces,
    ghostLevel, this->InputWholeExtent, inExt, splitMode, byPoints);
  this->Algorithm->TranslateInputExtent(inExt, this->InputWholeExtent, resultExtent);
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWEImageDataLIC2DExtentTranslator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Algorithm: " << this->Algorithm << endl;
}

