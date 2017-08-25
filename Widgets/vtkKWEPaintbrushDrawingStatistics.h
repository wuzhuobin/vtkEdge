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

// .NAME vtkKWEPaintbrushDrawingStatistics - Compute volume of sketches in a drawing
// .SECTION Description
// Takes a drawing as input.
// .SECTION See Also

#ifndef __vtkKWEPaintbrushDrawingStatistics_h
#define __vtkKWEPaintbrushDrawingStatistics_h

#include "vtkAlgorithm.h"
#include "VTKEdgeConfigure.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushDrawing;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushDrawingStatistics
                                : public vtkAlgorithm
{
public:
  // Description:
  // Standard VTK methods.
  static vtkKWEPaintbrushDrawingStatistics *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushDrawingStatistics, vtkAlgorithm);
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
  // Get the volume of the drawing. This will the sum of the volumes of all
  // the sketches in the drawing
  double GetVolume();

  // Description:
  // Get the volume of the n'th sketch in the drawing.
  double GetVolume( int n );

protected:
  vtkKWEPaintbrushDrawingStatistics();
  ~vtkKWEPaintbrushDrawingStatistics();

  // Description:
  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  vtkstd::vector< double > Volumes;
  double                   Volume;

private:
  vtkKWEPaintbrushDrawingStatistics( const vtkKWEPaintbrushDrawingStatistics& );
  void operator=(const vtkKWEPaintbrushDrawingStatistics&);
};

#endif

