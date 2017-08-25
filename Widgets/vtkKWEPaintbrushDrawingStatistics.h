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

// .NAME vtkKWEPaintbrushDrawingStatistics - Compute volume of sketches in a drawing
// .SECTION Description
// Takes a drawing as input.
// .SECTION See Also

#ifndef __vtkKWEPaintbrushDrawingStatistics_h
#define __vtkKWEPaintbrushDrawingStatistics_h

#include "vtkAlgorithm.h"
#include "VTKEdgeConfigure.h"
#include "vtkKWEPaintbrushEnums.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushDrawing;
class vtkKWEPaintbrushSketch;

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

  // Description:
  // Get the volume of this sketch in the drawing
  double GetVolume( vtkKWEPaintbrushSketch * );

  // Description:
  // Get the volume of the sketch with this label
  double GetLabelVolume( vtkKWEPaintbrushEnums::LabelType );

  // Description:
  // Get the input to this filter
  vtkKWEPaintbrushDrawing * GetDrawing();
  
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
  unsigned long *          VolumesArray;
  unsigned long            MaximumLabelValue;

private:
  vtkKWEPaintbrushDrawingStatistics( const vtkKWEPaintbrushDrawingStatistics& );
  void operator=(const vtkKWEPaintbrushDrawingStatistics&);
};

#endif

