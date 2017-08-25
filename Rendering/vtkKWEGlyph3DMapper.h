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
// .NAME vtkKWEGlyph3DMapper - vtkGlyph3D on the GPU.
// .SECTION Description
// Do the same job than vtkGlyph3D but on the GPU. For this reason, it is
// a mapper not a vtkPolyDataAlgorithm. Also, some methods of vtkGlyph3D
// don't make sense in vtkKWEGlyph3DMapper: GeneratePointIds, old-style
// SetSource, PointIdsName, IsPointVisible.
// .SECTION Implementation
//
// .SECTION See Also
// vtkGlyph3D

#ifndef __vtkKWEGlyph3DMapper_h
#define __vtkKWEGlyph3DMapper_h

#include "vtkPolyDataMapper.h"
#include "VTKEdgeConfigure.h" // include configuration header
#include "vtkGlyph3D.h" // for the constants (VTK_SCALE_BY_SCALAR, ...).
#include "vtkWeakPointer.h" // needed for vtkWeakPointer.

class vtkKWEGlyph3DMapperArray; // pimp
class vtkPainterPolyDataMapper;
class vtkScalarsToColorsPainter;

class VTKEdge_RENDERING_EXPORT vtkKWEGlyph3DMapper : public vtkMapper
{
public:
  static vtkKWEGlyph3DMapper* New();
  vtkTypeRevisionMacro(vtkKWEGlyph3DMapper, vtkMapper);
  void PrintSelf(ostream& os, vtkIndent indent);
  //BTX
  enum ArrayIndexes
    {
    SCALE = 0,
    SOURCE_INDEX = 1,
    MASK = 2,
    ORIENTATION = 3
    };
  //ETX

  // Description:
  // Specify a source object at a specified table location. New style.
  // Source connection is stored in port 1. This method is equivalent
  // to SetInputConnection(1, id, outputPort).
  void SetSourceConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetSourceConnection(vtkAlgorithmOutput* algOutput)
    {
      this->SetSourceConnection(0, algOutput);
    }
  
  // Description:
  // Specify a source object at a specified table location.
  void SetSource(int id, vtkPolyData *pd);

  // Description:
  // Set the source to use for he glyph. Old style. See SetSourceConnection.
  void SetSource(vtkPolyData *pd);

  // Description:
  // Get a pointer to a source object at a specified table location.
  vtkPolyData *GetSource(int id=0);

  // Description:
  // Turn on/off scaling of source geometry. When turned on, ScaleFactor
  // controls the scale applied. To scale with some data array, ScaleMode should
  // be set accordingly.
  vtkSetMacro(Scaling, bool);
  vtkBooleanMacro(Scaling, bool);
  vtkGetMacro(Scaling, bool);

  // Description:
  // Either scale by individual components (SCALE_BY_COMPONENTS) or magnitude
  // (SCALE_BY_MAGNITUDE) of the chosen array to SCALE with or disable scaling
  // using data array all together (NO_DATA_SCALING). Default is
  // NO_DATA_SCALING.
  vtkSetMacro(ScaleMode, int);
  vtkGetMacro(ScaleMode, int);

  // Description:
  // Specify scale factor to scale object by. This is used only when Scaling is
  // On.
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  //BTX
  enum ScaleModes
    {
    NO_DATA_SCALING = 0,
    SCALE_BY_MAGNITUDE = 1,
    SCALE_BY_COMPONENTS = 2
    };
  //ETX
  void SetScaleModeToScaleByMagnitude()
    { this->SetScaleMode(SCALE_BY_MAGNITUDE); }
  void SetScaleModeToScaleByVectorComponents()
    { this->SetScaleMode(SCALE_BY_COMPONENTS); }
  void SetScaleModeToNoDataScaling()
    { this->SetScaleMode(NO_DATA_SCALING); }
  const char *GetScaleModeAsString();

  // Description:
  // Specify range to map scalar values into.
  vtkSetVector2Macro(Range,double);
  vtkGetVectorMacro(Range,double,2);

  // Description:
  // Turn on/off orienting of input geometry. 
  // When turned on, the orientation array specified
  // using SetOrientationArray() will be used. 
  vtkSetMacro(Orient, bool);
  vtkGetMacro(Orient, bool);
  vtkBooleanMacro(Orient, bool);

  // Description:
  // Orientation mode indicates if the OrientationArray provides the direction
  // vector for the orientation or the rotations around each axes. Default is
  // DIRECTION
  vtkSetClampMacro(OrientationMode, int, DIRECTION, ROTATION);
  vtkGetMacro(OrientationMode, int);
  void SetOrientationModeToDirection()
    { this->SetOrientationMode(vtkKWEGlyph3DMapper::DIRECTION); }
  void SetOrientationModeToRotation()
    { this->SetOrientationMode(vtkKWEGlyph3DMapper::ROTATION); }
  const char* GetOrientationModeAsString();
  //BTX
  enum OrientationModes
    {
    DIRECTION=0,
    ROTATION=1
    };
  //ETX

  // Description:
  // Turn on/off clamping of data values to scale with to the specified range.
  vtkSetMacro(Clamping, bool);
  vtkGetMacro(Clamping, bool);
  vtkBooleanMacro(Clamping, bool);

  // Description:
  // Enable/disable indexing into table of the glyph sources. When disabled,
  // only the 1st source input will be used to generate the glyph. Otherwise the
  // source index array will be used to select the glyph source. The source
  // index array can be specified using SetSourceIndexArray().
  vtkSetMacro(SourceIndexing, bool);
  vtkGetMacro(SourceIndexing, bool);
  vtkBooleanMacro(SourceIndexing, bool);

  // Description:
  // Method initiates the mapping process. Generally sent by the actor 
  // as each frame is rendered.
  // Its behavior depends on the value of SelectMode.
  virtual void Render(vtkRenderer *ren, vtkActor *a);

  // Description:
  // Release any graphics resources that are being consumed by this mapper.
  // The parameter window could be used to determine which graphic
  // resources to release.
  virtual void ReleaseGraphicsResources(vtkWindow *window);

  // Description:
  // Redefined to take into account the bounds of the scaled glyphs.
  virtual double *GetBounds();

  // Description:
  // Same as superclass. Appear again to stop warnings about hidden method.
  virtual void GetBounds(double bounds[6]);

  // Description:
  // Send mapper ivars to sub-mapper.
  // \pre mapper_exists: mapper!=0
  void CopyInformationToSubMapper(vtkPainterPolyDataMapper *mapper);

  // Description:
  // If immediate mode is off, if NestedDisplayLists is false,
  // only the mappers of each glyph use display lists. If true,
  // in addition, matrices transforms and color per glyph are also
  // in a parent display list.
  // Not relevant if immediate mode is on.
  // For debugging/profiling purpose. Initial value is true.
  vtkSetMacro(NestedDisplayLists, bool);
  vtkGetMacro(NestedDisplayLists, bool);
  vtkBooleanMacro(NestedDisplayLists, bool);

  // Description:
  // Tells the mapper to skip glyphing input points that haves false values
  // in the mask array. If there is no mask array (id access mode is set
  // and there is no such id, or array name access mode is set and
  // the there is no such name), masking is silently ignored.
  // A mask array is a vtkBitArray with only one component.
  // Initial value is false.
  vtkSetMacro(Masking, bool);
  vtkGetMacro(Masking, bool);
  vtkBooleanMacro(Masking, bool);

  // Description:
  // Set the name of the point array to use as a mask for generating the glyphs.
  // This is a convenience method. The same effect can be achieved by using
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::MASK, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, maskarrayname)
  void SetMaskArray(const char* maskarrayname);

  // Description:
  // Set the point attribute to use as a mask for generating the glyphs.
  // \c fieldAttributeType is one of the following:
  // \li vtkDataSetAttributes::SCALARS
  // \li vtkDataSetAttributes::VECTORS
  // \li vtkDataSetAttributes::NORMALS
  // \li vtkDataSetAttributes::TCOORDS
  // \li vtkDataSetAttributes::TENSORS
  // This is a convenience method. The same effect can be achieved by using
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::MASK, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType)
  void SetMaskArray(int fieldAttributeType);

  // Description:
  // Tells the mapper to use an orientation array if Orient is true. 
  // An orientation array is a vtkDataArray with 3 components. The first
  // component is the angle of rotation along the X axis. The second
  // component is the angle of rotation along the Y axis. The third
  // component is the angle of rotation along the Z axis. Orientation is
  // specified in X,Y,Z order but the rotations are performed in Z,X an Y.
  // This definition is compliant with SetOrientation method on vtkProp3D.
  // By using vector or normal there is a degree of freedom or rotation
  // left (underconstrained). With the orientation array, there is no degree of
  // freedom left.
  // This is convenience method. The same effect can be achieved by using
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::ORIENTATION, 0, 0, 
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, orientationarrayname);
  void SetOrientationArray(const char* orientationarrayname);

  // Description:
  // Tells the mapper to use an orientation array if Orient is true. 
  // An orientation array is a vtkDataArray with 3 components. The first
  // component is the angle of rotation along the X axis. The second
  // component is the angle of rotation along the Y axis. The third
  // component is the angle of rotation along the Z axis. Orientation is
  // specified in X,Y,Z order but the rotations are performed in Z,X an Y.
  // This definition is compliant with SetOrientation method on vtkProp3D.
  // By using vector or normal there is a degree of freedom or rotation
  // left (underconstrained). With the orientation array, there is no degree of
  // freedom left.
  // \c fieldAttributeType is one of the following:
  // \li vtkDataSetAttributes::SCALARS
  // \li vtkDataSetAttributes::VECTORS
  // \li vtkDataSetAttributes::NORMALS
  // \li vtkDataSetAttributes::TCOORDS
  // \li vtkDataSetAttributes::TENSORS
  // This is convenience method. The same effect can be achieved by using
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::ORIENTATION, 0, 0, 
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType);
  void SetOrientationArray(int fieldAttributeType);

  // Description:
  // Convenience method to set the array to scale with. This is same as calling
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::SCALE, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, scalarsarrayname).
  void SetScaleArray(const char* scalarsarrayname);

  // Description:
  // Convenience method to set the array to scale with. This is same as calling
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::SCALE, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType).
  void SetScaleArray(int fieldAttributeType);

  // Description:
  // Convenience method to set the array to use as index within the sources. 
  // This is same as calling
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::SOURCE_INDEX, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, arrayname).
  void SetSourceIndexArray(const char* arrayname);

  // Description:
  // Convenience method to set the array to use as index within the sources. 
  // This is same as calling
  // SetInputArrayToProcess(vtkKWEGlyph3DMapper::SOURCE_INDEX, 0, 0,
  //    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType).
  void SetSourceIndexArray(int fieldAttributeType);

  // Description:
  // For selection by color id mode (not for end-user, called by
  // vtkKWEGlyphSelectionRenderMode). 0 is reserved for miss. it has to
  // start at 1. Initial value is 1.
  vtkSetMacro(SelectionColorId,unsigned int);
  vtkGetMacro(SelectionColorId,unsigned int);

  // Description:
  // Called by vtkKWEGlyphSelectionRenderMode.
  vtkSetMacro(SelectMode, int);

  // Description:
  // WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
  // DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS
  // Used by vtkHardwareSelector to determine if the prop supports hardware
  // selection.
  virtual bool GetSupportsSelection()
    { return true; }
  //BTX
protected:
  vtkKWEGlyph3DMapper();
  ~vtkKWEGlyph3DMapper();

  virtual int RequestUpdateExtent(vtkInformation *request,
    vtkInformationVector **inInfo,
    vtkInformationVector *outInfo);

  virtual int FillInputPortInformation(int port,
    vtkInformation *info);

  // Description:
  // Take part in garbage collection.
  virtual void ReportReferences(vtkGarbageCollector *collector);

  vtkPolyData *GetSource(int idx, vtkInformationVector *sourceInfo);

  // Description:
  // Convenience methods to get each of the arrays.
  vtkDataArray* GetMaskArray(vtkDataSet* input);
  vtkDataArray* GetSourceIndexArray(vtkDataSet* input);
  vtkDataArray* GetOrientationArray(vtkDataSet* input);
  vtkDataArray* GetScaleArray(vtkDataSet* input);
  vtkUnsignedCharArray* GetColors(vtkDataSet* input);

  // Description:
  // Release display list used for matrices and color.
  void ReleaseList();

  // Description:
  // Called when the PainterInformation becomes obsolete. 
  // It is called before the Render is initiated on the Painter.
  virtual void UpdatePainterInformation();

  bool Scaling; // Determine whether scaling of geometry is performed
  double ScaleFactor; // Scale factor to use to scale geometry
  int ScaleMode; // Scale by scalar value or vector magnitude
  
  double Range[2]; // Range to use to perform scalar scaling
  bool Orient; // boolean controls whether to "orient" data
  bool Clamping; // whether to clamp scale factor
  bool SourceIndexing; // Enable/disable indexing into the glyph table
  bool Masking; // Enable/disable masking.
  int OrientationMode;

  vtkKWEGlyph3DMapperArray *SourceMappers; // array of mappers

  vtkWeakPointer<vtkWindow> LastWindow; // Window used for previous render.

  unsigned int DisplayListId; // GLuint

  bool NestedDisplayLists; // boolean

  vtkScalarsToColorsPainter* ScalarsToColorsPainter;
  vtkInformation* PainterInformation;
  vtkTimeStamp PainterUpdateTime;

  unsigned int SelectionColorId;
  int SelectMode;

private:
  vtkKWEGlyph3DMapper(const vtkKWEGlyph3DMapper&); // Not implemented.
  void operator=(const vtkKWEGlyph3DMapper&); // Not implemented.
  //ETX
};

#endif
