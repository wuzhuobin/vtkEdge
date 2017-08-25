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

// .NAME vtkKWEPaintbrushDrawing - a list of Sketches
// .SECTION Description
// vtkKWEPaintbrushDrawing represents and provides methods to manipulate a list of
// Sketches. The list is unsorted and duplicate entries are not prevented.
//
// .SECTION see also
// vtkKWEPaintbrushSketch

#ifndef __vtkKWEPaintbrushDrawing_h
#define __vtkKWEPaintbrushDrawing_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkCollection.h"
#include "vtkDataObject.h"
#include "vtkKWEPaintbrushSketch.h" // For inline methods
#include "vtkImageData.h"

class vtkKWEPaintbrushOperation;
class vtkKWEPaintbrushPropertyManager;
class vtkKWEPaintbrushLabelData;
class vtkKWEPaintbrushWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushDrawing : public vtkDataObject
{
  //BTX
  friend class vtkKWEPaintbrushSketch;
  friend class vtkKWEPaintbrushRepresentation;
  //ETX
public:
  static vtkKWEPaintbrushDrawing *New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushDrawing,vtkDataObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Add an Sketch to the list.
  void AddItem(vtkKWEPaintbrushSketch *a);

  // Description:
  // Add a new sketch to the list. Also initialize it with the usual parameters.
  // Returns the newly added sketch.
  vtkKWEPaintbrushSketch * AddItem();

  // Description:
  // Remove all sketches in the drawing.
  void RemoveAllItems();

  // Description:
  // Remove the specified sketch. Returns 1 if the item was removed.
  int RemoveItem( vtkKWEPaintbrushSketch * );

  // Description:
  // Get the Nth sketch from the drawing.
  vtkKWEPaintbrushSketch *GetItem(int i);

  // Description:
  // Get the sketch with the specified label. Note that is method is relevant,
  // only if our representation is a LabelMap.
  // Returns NULL if there is no sketch with that label.
  vtkKWEPaintbrushSketch *GetItemWithLabel( vtkKWEPaintbrushEnums::LabelType );

  // Is a sketch present ? Returns 1 if yes.
  int IsItemPresent( vtkKWEPaintbrushSketch * s );

  // Description:
  // Set the image we are painting on.
  void SetImageData( vtkImageData * );
  vtkGetObjectMacro( ImageData, vtkImageData );

  // Description:
  // Set the LabelMap we are painting on to an existing labelmap.  Set
  // before calling InitializeData.  Only used if the representation type
  // is set to labelmap.
  vtkSetObjectMacro( LabelMap, vtkImageData );
  vtkGetObjectMacro( LabelMap, vtkImageData );

  // Description:
  // Set the PaintbrushData for the drawing. You must use this method only
  // if your data is a label map. If not, the data of each sketch is managed
  // independently and you should set the PaintbrushData on each sketch
  // seperately using the method above.
  void SetPaintbrushData( vtkKWEPaintbrushLabelData * );

  // Description:
  // Set/Get the paintbrush data that we've sketched out. This is the cumlated
  // paintbrush data of all sketches.
  vtkGetObjectMacro( PaintbrushData, vtkKWEPaintbrushData );

  // Description:
  // Initialize the drawing data based on the image metadata.
  void InitializeData();

  // Description:
  // There are two ways to initialize sketches in a drawing. They may be
  // initialized individually using vtkKWEPaintbrushSketch::Initialize(..)
  //  or
  // they may be initliazed more efficiently by the following method. This
  // method is more efficient if you have several sketches in your drawing
  // since, the drawing data is parsed for seperation into sketches only
  // once. Note that you must have a collection of sketches in this drawing.
  // Then we parse the paintbrush label data for labels that correspond to
  // each of those sketches and initialize those sketches with the initial
  // label map data as a stroke.
  void InitializeSketches();

  // Description:
  // Unlike the above method, this method creates sketches for each label
  // prsent in the label map. The subtle difference is that InitializeSketches
  // initializes existing sketches in the drawing from the labels that
  // these sketches represent that may be present in the label map. CreateSketches
  // will read in a label map and create a set of sketches for you based on
  // any label that is present in the label map.
  void CreateSketches();

  // Description:
  // Set the PaintbrushOperation
  void SetPaintbrushOperation( vtkKWEPaintbrushOperation * );
  vtkGetObjectMacro( PaintbrushOperation, vtkKWEPaintbrushOperation );

  // Description:
  // Add and delete a stroke. A stroke can hold multiple points.
  // If type is 1, the stroke is a positive (draw) stroke. If type
  // is 0, its a negative (erase) stroke.
  //   You can also add a new stroke with your own paintbrush data as input,
  // let's say you wanted to compose an external segmentation to the
  // existing ones.
  virtual int AddNewStroke( int n,
                            int type = vtkKWEPaintbrushEnums::Draw,
                            vtkKWEPaintbrushData *initialization = NULL);

  //BTX
  // Description:
  // Retrieve an instance of this class from an information object.
  static vtkKWEPaintbrushDrawing* GetData(vtkInformation* info);
  static vtkKWEPaintbrushDrawing* GetData(vtkInformationVector* v, int i=0);
  //ETX

  // Description:
  // Data objects are composite objects and need to check each part for MTime.
  // The information object also needs to be considered.
  virtual unsigned long GetMTime();

  // Description:
  // Return the number of sketches in the drawing.
  int  GetNumberOfItems() { return this->Collection->GetNumberOfItems(); }

  // Description:
  // Get the index of the item in the list. Returns -1 if the item
  // isn't present.
  int GetIndexOfItem( vtkKWEPaintbrushSketch * s );

  // Description:
  // The user must set this using the class. For details, see documentation of
  // vtkKWEPaintbrushEnums::BrushType.
  vtkSetMacro(Representation, int);
  vtkGetMacro(Representation, int);
  virtual void SetRepresentationToGrayscale()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Grayscale); }
  virtual void SetRepresentationToBinary()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Binary); }
  virtual void SetRepresentationToLabel()
    { this->SetRepresentation(vtkKWEPaintbrushEnums::Label); }

  // Description:
  // You can set an upper limit on the number of sketches that can be added
  // to the drawing. By default there is no limit.
  // As a caveat, note that if you already have more sketches than the limit,
  // setting this ivar will not remove any existing sketches.
  vtkSetMacro( MaximumNumberOfSketches, int );
  vtkGetMacro( MaximumNumberOfSketches, int );

  // Description:
  // Collapse the history of all sketches in the drawing. This simply calls
  // the CollapseHistory() method for each sketch in this drawing.
  virtual void CollapseHistory();

  // Description:
  // Shallow copy.  This copies the data, but not any of the pipeline
  // connections.
  virtual void ShallowCopy(vtkDataObject *src);

  // Description:
  // INTERNAL - Do not use.
  // Get the property manager, used to assign unique colors etc.
  vtkGetObjectMacro( PaintbrushPropertyManager, vtkKWEPaintbrushPropertyManager );

  // Description:
  // Compose strokes from all sketches together. This will visit each stroke
  // on the stack from each sketch. Sort them by MTime. Then compose them all
  // together.
  void ComposeStrokes();

protected:
  vtkKWEPaintbrushDrawing();
  ~vtkKWEPaintbrushDrawing();

  vtkKWEPaintbrushData             *PaintbrushData;
  vtkImageData                     *ImageData;
  vtkImageData                     *LabelMap;
  vtkKWEPaintbrushOperation        *PaintbrushOperation;
  vtkKWEPaintbrushPropertyManager  *PaintbrushPropertyManager;
  vtkCollection                    *Collection;
  int                               Representation;
  int                               MaximumNumberOfSketches;

  // After setting the ImageData on which to draw the strokes, allocate
  // paintbrush data of the appropriate size.
  virtual void AllocatePaintbrushData();

  // Description:
  // Helper function to assign a unique label to a newly created sketch. This
  // makes sense only if the representation is "Label"
  void AssignUniqueLabelToSketch( vtkKWEPaintbrushSketch *s );

  // Description:
  // This method will be invoked from the widget during user interaction
  virtual void AddShapeToCurrentStroke( int n, double p[3] );

private:
  vtkKWEPaintbrushDrawing(const vtkKWEPaintbrushDrawing&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushDrawing&);  // Not implemented.
};

#endif
