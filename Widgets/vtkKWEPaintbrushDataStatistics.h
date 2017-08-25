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

