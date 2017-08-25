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

// .NAME vtkKWEPaintbrushLabelData - An abstract class used to support segmentations
// .SECTION Description
//
// .SECTION see also
// vtkImageLabelSource vtkImageLabel

#ifndef __vtkKWEPaintbrushLabelData_h
#define __vtkKWEPaintbrushLabelData_h

#include "vtkKWEPaintbrushData.h"
#include <vtkstd/set>

class vtkImageData;
class vtkKWEPaintbrushSketch;
class vtkKWEPaintbrushStencilData;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushLabelData
                                 : public vtkKWEPaintbrushData
{
  //BTX
  friend class vtkKWEPaintbrushSketch;
  //ETX
public:
  static vtkKWEPaintbrushLabelData *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushLabelData, vtkKWEPaintbrushData);
  void PrintSelf(ostream& os, vtkIndent indent);

  void DeepCopy(vtkDataObject *o);
  void ShallowCopy(vtkDataObject *f);

  // Description:
  // Set/Get the label map.
  // This method should be used if you wish to initialize the class from an
  // existing label map.
  virtual void SetLabelMap( vtkImageData * );
  vtkGetObjectMacro( LabelMap, vtkImageData );

  // Description:
  // Minkowski operations.
  // The arguments can be both vtkKWEPaintbrushStencilData or
  // vtkKWEPaintbrushLabelData
  virtual int  Add(      vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Subtract( vtkKWEPaintbrushData *, bool forceMutable=false );
  virtual int  Replace( vtkKWEPaintbrushData *, bool forceMutable=false );

  // Description:
  // Clip self with supplied extents. Return 1 if something changed
  virtual int Clip( int extent[6] );

  // Description:
  // Allocate and fill. This will wipe out any existing data. It is assumed
  // that the extents have been set.
  virtual void Allocate(double fillValue = vtkKWEPaintbrushLabelData::NoLabelValue);

  // Description:
  // Set the spacing.
  virtual void SetSpacing( double spacing[3] );
  virtual void GetSpacing( double spacing[3] );

  // Description:
  // Set the origin.
  virtual void SetOrigin( double origin[3] );
  virtual void GetOrigin( double origin[3] );

  // Description:
  // Set the extent of the data.
  virtual void SetExtent(int extent[6]);
  virtual void SetExtent(int x1, int x2, int y1, int y2, int z1, int z2);
  virtual void GetExtent(int extent[6]);

  // Description:
  // Is the point "p" inside ?
  // The value returned is the label value. A value of 0 indicates that there
  // is nothing at the current location.
  virtual int IsInside( double p[3] );

  // Description:
  // Get the label data as an image data. This is present merely to satisfy the
  // superclass requirement. It is preferred that you use the method above
  // instead.
  virtual void GetPaintbrushDataAsImageData( vtkImageData * );

  // Description:
  // See vtkObject/vtkDataObject for doc
  virtual unsigned long GetMTime();

  // Description:
  // Override these to handle origin, spacing, scalar type, and scalar
  // number of components.  See vtkDataObject for details.
  virtual void CopyInformationToPipeline(vtkInformation* request,
                                         vtkInformation* input,
                                         vtkInformation* output,
                                         int forceCopy);
  virtual void CopyInformationFromPipeline(vtkInformation* request);

  //BTX
  // Description:
  // Retrieve an instance of this class from an information object.
  // See vtkObject/vtkDataObject for doc
  static vtkKWEPaintbrushLabelData* GetData(vtkInformation* info);
  static vtkKWEPaintbrushLabelData* GetData(vtkInformationVector* v, int i=0);
  //ETX

  // Description:
  // Restore data object to initial state. See vtkDataObject for details
  virtual void Initialize();

  // Description:
  // Clear data corresponding to a label.
  // This sets all the voxels with the supplied label value to 0.
  virtual void Clear( vtkKWEPaintbrushEnums::LabelType label );

  // Description:
  // The value 'NoLabelValue' is used to represent the absence of any label.
  // Default value is 0.
  static void SetNoLabelValue( vtkKWEPaintbrushEnums::LabelType label );
  static vtkKWEPaintbrushEnums::LabelType NoLabelValue;

  //BTX
  // Description:
  // Get the labels that are present in this image. This will visit every single
  // voxel in the LabelMap and add it to the resulting set.
  typedef vtkstd::set< vtkKWEPaintbrushEnums::LabelType > LabelSetType;
  LabelSetType GetLabels();
  //ETX

  // Description:
  // Convenience method to collapse the labels in a label map. Consider a label
  // map with labels of 32, 64, 125, 255 and a NoLabelValue of 0. This will
  // collapse this data into labels of 1, 2, 3, 4.
  virtual void RelabelDataToContiguousLabels();

protected:
  vtkKWEPaintbrushLabelData();
  ~vtkKWEPaintbrushLabelData();

  // Description:
  // Get important info from pipeline.
  void CopyOriginAndSpacingFromPipeline();

  // Description:
  // The actual 'data' is stored here.
  vtkImageData *LabelMap;

  int AddStencil( vtkKWEPaintbrushStencilData * s, bool forceMutable );

private:
  vtkKWEPaintbrushLabelData(const vtkKWEPaintbrushLabelData&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushLabelData&);  // Not implemented.

  // Description:
  // Called only from vtkKWEPaintbrushSketch.
  // Is the supplied label mutable ?
  void SetMutable( int isMutable, vtkKWEPaintbrushEnums::LabelType label );
  int IsMutable( vtkKWEPaintbrushEnums::LabelType label );

  //BTX
  // Is a label immutable ? If not present in the map, it is assumed to be
  // mutable.
  LabelSetType ImmutableLabels;
  //ETX
};

#endif


