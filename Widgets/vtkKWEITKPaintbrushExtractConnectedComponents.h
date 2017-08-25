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
// .NAME vtkKWEITKPaintbrushExtractConnectedComponents - Extract connected components in a drawing.
// .SECTION Description
// Takes a drawing as input. Generates a drawing as output. The input drawing
// will have one or more sketches. The user set the sketch for which connected 
// components must be extracted via 'SetSketchIndex' (default is the first
// sketch). The algorithm will produce as output a drawing with 'n' sketches
// where 'n' is the number of connected components. Each connected component 
// is a seperate sketch. 
// .SECTION See Also

#ifndef __vtkKWEITKPaintbrushExtractConnectedComponents_h
#define __vtkKWEITKPaintbrushExtractConnectedComponents_h

#include "vtkAlgorithm.h"
#include "VTKEdgeConfigure.h"

class vtkKWEPaintbrushDrawing;
class vtkImageData;

class VTKEdge_WIDGETS_EXPORT vtkKWEITKPaintbrushExtractConnectedComponents 
                                : public vtkAlgorithm
{
public:
  // Description:
  // Standard VTK methods.
  static vtkKWEITKPaintbrushExtractConnectedComponents *New();
  vtkTypeRevisionMacro(vtkKWEITKPaintbrushExtractConnectedComponents, vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the sketch from the input drawing of which we wish to extract
  // connected components. Defaults to 0.
  vtkSetMacro( SketchIndex, int );
  vtkGetMacro( SketchIndex, int );
  
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
  vtkKWEITKPaintbrushExtractConnectedComponents();
  ~vtkKWEITKPaintbrushExtractConnectedComponents();

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

  // Description:
  // Compute the tightest bounding box that covers each of the connected
  // components.
  void ComputeSegmentExtents( int nSegments, int *e, vtkImageData *image );

  int SketchIndex;

private:
  vtkKWEITKPaintbrushExtractConnectedComponents(
      const vtkKWEITKPaintbrushExtractConnectedComponents&);  //Not implemented
  void operator=(const 
      vtkKWEITKPaintbrushExtractConnectedComponents&);  //Not implemented
};

#endif
