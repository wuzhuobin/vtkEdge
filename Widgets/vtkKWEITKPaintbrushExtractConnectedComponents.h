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
