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

// .NAME vtkKWEPaintbrushMergeSketches - Merge all sketches in a drawing into a single sketch
// .SECTION Description
// Takes a drawing as input. Generates a drawing as output. The input drawing
// will have one or more sketches. The output drawing will have a single
// sketch. All sketches from the input will be merged into a single sketch.

#ifndef __vtkKWEPaintbrushMergeSketches_h
#define __vtkKWEPaintbrushMergeSketches_h

#include "vtkAlgorithm.h"
#include "VTKEdgeConfigure.h"

class vtkKWEPaintbrushDrawing;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushMergeSketches 
                                : public vtkAlgorithm
{
public:
  // Description:
  // Standard VTK methods.
  static vtkKWEPaintbrushMergeSketches *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushMergeSketches, vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set a paintbrush drawing as input
  virtual void SetInput( vtkKWEPaintbrushDrawing * );

  // Description:
  // see vtkAlgorithm for details
  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);

  // Description:
  // Get the output data object of this algorithm.
  vtkKWEPaintbrushDrawing* GetOutput();  

protected:
  vtkKWEPaintbrushMergeSketches();
  ~vtkKWEPaintbrushMergeSketches();

  // Description:
  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);
  virtual int FillOutputPortInformation(int, vtkInformation*);
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  virtual void RequestInformation (vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);

  // Description:
  // Creates the same output type as the input type.
  virtual int RequestDataObject(vtkInformation* request,
                                vtkInformationVector** inputVector,
                                vtkInformationVector* outputVector);

private:
  vtkKWEPaintbrushMergeSketches(
      const vtkKWEPaintbrushMergeSketches&);  //Not implemented
  void operator=(const 
      vtkKWEPaintbrushMergeSketches&);  //Not implemented
};

#endif

