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

// .NAME vtkKWEPaintbrushDataStatistics - Compute volume of a vtkKWEPaintbrushData
// .SECTION Description
// Computes the volume of a grayscale or binary brush data. Simply use the
// class as
//
//   vtkKWEPaintbrushDataStatistics *algo = vtkKWEPaintbrushDataStatistics::New();
//   algo->SetInput( paintbrushData );
//   double volume = algo->GetVolume();
//
// .SECTION See Also

#ifndef __vtkKWEPaintbrushDataStatistics_h
#define __vtkKWEPaintbrushDataStatistics_h

#include "vtkAlgorithm.h"
#include "VTKEdgeConfigure.h"

class vtkKWEPaintbrushData;
class vtkContourFilter;
class vtkImageConstantPad;
class vtkTriangleFilter;
class vtkMassProperties;
class vtkKWEPaintbrushStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushDataStatistics
                                : public vtkAlgorithm
{
public:
  // Description:
  // Standard VTK methods.
  static vtkKWEPaintbrushDataStatistics *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushDataStatistics, vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set a paintbrush data as input
  virtual void SetInput( vtkKWEPaintbrushData * );

  // Description:
  // see vtkAlgorithm for details
  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);

  // Description:
  // Get the volume
  double GetVolume();

  // Description:
  // Get the overlapped volume between two binary paintbrush data's. I would
  // have added the implementation for grayscale ones too, but its too
  // complicated to do that.
  // It is assumed that both the stencils have the same specing and origin,
  // although they need not have the same extent.
  static double GetOverlapVolume( vtkKWEPaintbrushStencilData *,
                                  vtkKWEPaintbrushStencilData * );

protected:
  vtkKWEPaintbrushDataStatistics();
  ~vtkKWEPaintbrushDataStatistics();

  // Description:
  // see vtkAlgorithm for docs.
  virtual int FillInputPortInformation(int, vtkInformation*);
  virtual int RequestData(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);

  vtkContourFilter           * ContourFilter;
  vtkImageConstantPad        * PadFilter;
  vtkTriangleFilter          * TriangleFilter;
  vtkMassProperties          * MassProperties;
  double                       Volume;

private:
  vtkKWEPaintbrushDataStatistics( const vtkKWEPaintbrushDataStatistics& );
  void operator=(const vtkKWEPaintbrushDataStatistics&);
};

#endif

