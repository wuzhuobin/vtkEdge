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
// .NAME vtkKWEImageDataLIC2DExtentTranslator 
// .SECTION Description

#ifndef __vtkKWEImageDataLIC2DExtentTranslator_h
#define __vtkKWEImageDataLIC2DExtentTranslator_h

#include "vtkExtentTranslator.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkKWEImageDataLIC2D;

class VTKEdge_HYBRID_EXPORT vtkKWEImageDataLIC2DExtentTranslator : public vtkExtentTranslator
{
public:
  static vtkKWEImageDataLIC2DExtentTranslator* New();
  vtkTypeRevisionMacro(vtkKWEImageDataLIC2DExtentTranslator, vtkExtentTranslator);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the vtkKWEImageDataLIC2D algorithm for which this extent translator is
  // being used.
  void SetAlgorithm(vtkKWEImageDataLIC2D*);
  vtkKWEImageDataLIC2D* GetAlgorithm();

  void SetInputExtentTranslator(vtkExtentTranslator*);
  vtkGetObjectMacro(InputExtentTranslator, vtkExtentTranslator);

  vtkSetVector6Macro(InputWholeExtent, int);
  vtkGetVector6Macro(InputWholeExtent, int);

  virtual int PieceToExtentThreadSafe(int piece, int numPieces, 
                                     int ghostLevel, int *wholeExtent, 
                                     int *resultExtent, int splitMode, 
                                     int byPoints);

//BTX
protected:
  vtkKWEImageDataLIC2DExtentTranslator();
  ~vtkKWEImageDataLIC2DExtentTranslator();

  int InputWholeExtent[6];
  vtkExtentTranslator* InputExtentTranslator;
  vtkWeakPointer<vtkKWEImageDataLIC2D> Algorithm;
private:
  vtkKWEImageDataLIC2DExtentTranslator(const vtkKWEImageDataLIC2DExtentTranslator&); // Not implemented.
  void operator=(const vtkKWEImageDataLIC2DExtentTranslator&); // Not implemented.
//ETX
};

#endif


