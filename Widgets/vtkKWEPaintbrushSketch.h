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
// .NAME vtkKWEPaintbrushSketch
// .SECTION Description
// .SECTION See Also

#ifndef __vtkKWEPaintbrushSketch_h
#define __vtkKWEPaintbrushSketch_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEPaintbrushEnums.h"
#include "vtkObject.h"
#include <vtkstd/vector>

class vtkKWEPaintbrushOperation;
class vtkKWEPaintbrushStroke;
class vtkImageData;
class vtkKWEPaintbrushData;
class vtkKWEPaintbrushLabelData;
class vtkKWEPaintbrushProperty;
class vtkKWEPaintbrushDrawing;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushSketch : public vtkObject
{
  //BTX
  friend class vtkKWEPaintbrushDrawing;
  friend class vtkKWEPaintbrushProperty;
  //ETX
public:

  // Description:
  // Standard VTK methods
  static vtkKWEPaintbrushSketch *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushSketch, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the operation. Must be set.
  virtual void SetPaintbrushOperation(    vtkKWEPaintbrushOperation * );
  vtkGetObjectMacro( PaintbrushOperation, vtkKWEPaintbrushOperation );

  // Description:
  // Set/Get the image data over which the strokes are overlayed. This is the
  // background image.
  virtual void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Description:
  // Set/Get the paintbrush data that we've already sketched out. If one is
  // not set, the canvas will be created automatically for you.
  vtkGetObjectMacro( PaintbrushData, vtkKWEPaintbrushData );
  virtual void SetPaintbrushData( vtkKWEPaintbrushData * );

  // Description:
  // Initializes the datastructures, creates a canvas (vtkKWEPaintbrushData) if
  // one hasn't been set already. If called, it must be called only after the
  // operation and the ImageData have been set.
  virtual void Initialize();

  // Description:
  // Alternate function signature for initializing the sketch. Must be called
  // only after the operation and ImageData and the label value have been set.
  // The former two will be automatically set if you have added this to the
  // drawing already.
  virtual void Initialize( vtkKWEPaintbrushLabelData * labelMap );

  // Description:
  // See documentation of vtkKWEPaintbrushEnums::BrushType.
  vtkSetMacro(Representation, int);
  vtkGetMacro(Representation, int);
  virtual void SetRepresentationToGrayscale()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Grayscale); }
  virtual void SetRepresentationToBinary()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Binary); }
  virtual void SetRepresentationToLabel()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Label); }

  // Description:
  // This must be set if you are setting the representation to 'Label'. This
  // indicates the value in the label map that this sketch represents.
  vtkSetMacro( Label, vtkKWEPaintbrushEnums::LabelType );
  vtkGetMacro( Label, vtkKWEPaintbrushEnums::LabelType );

  // Description:
  // The sketch maintains a history of strokes made, to facilitate undo/redo
  // of edits. A larger history length implies more memory. A length of 0
  // implies that no undo-redo functionality is possible.
  // Defaults to 4.
  // If you choose to set this, do so once before using the sketch. DO NOT
  // change it midway.
  vtkSetClampMacro( HistoryLength, int, 0, 10 );
  vtkGetMacro( HistoryLength, int );

  // Description:
  // Get the property of the sketch. You may use this to change colors etc.
  vtkGetObjectMacro( PaintbrushProperty, vtkKWEPaintbrushProperty );

  // Description:
  // Add a new stroke.
  //
  // The first argument specifies if it is a draw or an erase stroke.
  //
  // The second argument is optional. You can initialize this new stroke
  // data, let's say you wanted to compose an external segmentation to the
  // existing ones. Your initialization will automatically be added to the
  // sketch's history.
  //
  // The third argument is optional. By default, each stroke has extents that
  // match that of the canvas image. Although there are efficient
  // datastructures to more compactly represent this, it can induce some
  // memory drag. If you are dead sure that your stroke is going to have
  // certain bounding extents, you can set this. The widgets internally use
  // this under certain conditions to speed things up.
  virtual int AddNewStroke(
    int polarity = vtkKWEPaintbrushEnums::Draw,
    vtkKWEPaintbrushData * initialization = NULL,
    int *extents = NULL,
    bool forceMutable = false );

  // Description:
  // Add another sketch's data to this sketch
  void Add( vtkKWEPaintbrushSketch * sketch );

  // Description:
  // Delete the last added stroke
  virtual int DeleteLastStroke();

  // Description:
  // Used to maintain the undo/redo functionality.. Pop sequence pops a
  // sequence out of the list during undo. Push pushes the last undone
  // sequence back into the list. (essentially does a redo).
  virtual int PopStroke();
  virtual int PushStroke();

  // Description:
  // Strokes between start and end, both inclusive, are erased.
  virtual void EraseStrokes( int start, int end );

  // Description:
  // This method can be used to copy a sketch from a source extent to a
  // target extent. Only voxels that belong to this sketch will be copied
  // over. Use cases for this method are slice by slice propagation of a
  // sketch.
  void CopySketchFromExtentToExtent(int sourceExtent[6], int targetExtent[6]);

  // Description:
  // See vtkObject for doc
  virtual unsigned long GetMTime();

  // Description:
  // INTERNAL - Do not use.
  // This method merges strokes from start to end.
  virtual void ComposeStrokes();

protected:
  vtkKWEPaintbrushSketch();
  ~vtkKWEPaintbrushSketch();

   vtkKWEPaintbrushOperation              *PaintbrushOperation;
   vtkKWEPaintbrushData                   *PaintbrushData;
   vtkKWEPaintbrushProperty               *PaintbrushProperty;
   vtkImageData                        *ImageData;
   int                                  CurrentStroke;
   int                                  Representation;
   int                                  HistoryLength;
   vtkKWEPaintbrushDrawing                *PaintbrushDrawing;

  // This is used only if our sketch's representation is of type: Label
  vtkKWEPaintbrushEnums::LabelType         Label;

  // Description:
  // INTERNAL - Do not use.
  // Invoked by a user draw/erase interaction.
  // Adds a shape to the current stroke at position "p"
  virtual void AddShapeToCurrentStroke(
       double p[3],
       vtkKWEPaintbrushData * auxData = NULL);

  // Description:
  // By default, the sketch's extents will be same as that of the canvas,
  // ImageData that's set as input. You should never need to explicitly
  // call this method, but if called, must be called after Initialize().
  virtual void AllocatePaintbrushData();

  // Description:
  // Collapses the history of the sketch. This does not affect the history
  // length. This simply collapses all existing history.
  void CollapseHistory();

private:
  vtkKWEPaintbrushSketch(const vtkKWEPaintbrushSketch&);  //Not implemented
  void operator=(const vtkKWEPaintbrushSketch&);  //Not implemented

  // Mutability of the sketch. This is set by the PaintbrushProperty.
  void SetMutable( int m );

  //BTX
  // Set of strokes contained by this sketch (ordered by time)
  vtkstd::vector<vtkKWEPaintbrushStroke*> Strokes;

  // Description:
  // Get all the strokes upto the current stroke.
  void GetStrokes(vtkstd::vector< vtkKWEPaintbrushStroke * > &strokes);
  //ETX
};

#endif
