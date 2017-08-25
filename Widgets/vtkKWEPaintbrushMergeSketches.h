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

