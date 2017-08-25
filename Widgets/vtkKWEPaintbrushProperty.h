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

// .NAME vtkKWEPaintbrushProperty
// .SECTION Description

#ifndef __vtkKWEPaintbrushProperty_h
#define __vtkKWEPaintbrushProperty_h

#include "VTKEdgeConfigure.h"
#include "vtkObject.h"

class vtkKWEPaintbrushSketch;
class vtkKWEPaintbrushDrawing;
class vtkProperty;
class vtkKWEPaintbrushPropertyManager;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushProperty : public vtkObject
{
  //BTX
  friend class vtkKWEPaintbrushSketch;
  friend class vtkKWEPaintbrushBlend;
  friend class vtkKWEPaintbrushRepresentationGrayscale2D;
  friend class vtkKWEPaintbrushPropertyManager;
  friend class vtkKWEPaintbrushSelectionRepresentation;
  friend class vtkKWEPaintbrushHighlightActors;
  //ETX
public:
  vtkTypeRevisionMacro(vtkKWEPaintbrushProperty,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // A drawing is always associated with a paintbrush sketch. A
  // vtkKWEPaintbrushSketch upon creation will automatically be assigned a
  // default property internally. You can, using this method, query the sketch
  // to which this property is assigned.
  vtkGetObjectMacro( PaintbrushSketch, vtkKWEPaintbrushSketch );

  // Description:
  // Set a unique identifier string. This may be rendered as annotations etc.
  vtkSetStringMacro( Identifier );
  vtkGetStringMacro( Identifier );

  // Description:
  // Set/Get the object's opacity. 1.0 is totally opaque and 0.0 is completely
  // transparent.
  void SetOpacity( double o );
  double GetOpacity();

  // Description:
  // Set the visibility of the sketch
  vtkSetMacro( Visibility, int );
  vtkGetMacro( Visibility, int );
  vtkBooleanMacro( Visibility, int );

  // Description:
  // Set/Get the width of a Line. The width is expressed in screen units.
  // This is only implemented for OpenGL. The default is 1.0.
  void SetLineWidth( float );
  vtkGetMacro( LineWidth, float );

  // Description:
  // Get the color of the property. Colors are for now, auto assigned to give
  // the most pleasing appearance, and as far as possible ensure that colors
  // for each segmentation are unique. Note that the values are normalized to
  // [0-1] when the arguments passed in are double.
  void GetColor(double rgb[3]);
  void GetColor(unsigned char rgb[3]);
  double *GetColor();

  // Description:
  // Set the color of the property. Colors are for now, auto assigned to give
  // the most pleasing appearance, and as far as possible ensure that colors
  // for each segmentation are unique. The moment you start assign a color to
  // any property in a drawing the property manager will stop autoassigning
  // "quasi-unique" colors. Note that the values are normalized to [0-1].
  void SetColor(double rgb[3]);

  // Description:
  // Return this object's modified time.
  virtual unsigned long GetMTime();

  //BTX
  enum HighlightTypeEnum
    {
    StippledInvertedHighlight,
    ThickHighlight,
    ColorHighlight
    };
  //ETX

  // Description:
  // Three highlight modes are allowed.
  // 1. Stippled inverted highlight looks better when you have an overlay, as
  //    with vtkKWEPaintbrushRepresentation2D.
  // 2. ThickHighlight looks better when you render the brushdata as contour
  //    as is done in vtkKWEPaintbrushRepresentationGrayscale2D
  // 3. Color Highlighting of the edges looks nice on both. Just make sure
  //    that you don't assign the same color as the highlight assigned color
  //    to the sketches you draw or you'll have trouble making out that its
  //    highlighted. Yellow is the default color Highlight.
  // ColorHighlight is the default HighlightType.
  vtkSetMacro( HighlightType, int );
  vtkGetMacro( HighlightType, int );

  // Description:
  // Factor by which the highlighted lines should thicken when a sketch is
  // highlighted. Default is 2.0 (ie. the lines will appear twice as thick
  // if highlighted). Note that this applies only if our highlight mode is
  // ThickHighlight.
  vtkSetClampMacro( HighlightThickness, double, 1.0, 10.0 );
  vtkGetMacro( HighlightThickness, double );

  // Description:
  // HighlightColor.
  // The default HighlightColor is yellow.
  // This makes sense only if HighlightType is ColorHighlight
  vtkSetVector3Macro( HighlightColor, double );
  vtkGetVector3Macro( HighlightColor, double );

  // Description:
  // Are we Highlighted ?
  vtkGetMacro( Highlight, int );


  // Description:
  // Sketches can be deemed immutable or mutable. An immutable sketch cannot
  // be modified by *user* edits. By default all sketches are mutable.
  void SetMutable( int );
  virtual int GetMutable();

protected:
  static vtkKWEPaintbrushProperty *New( vtkKWEPaintbrushSketch * );
  static vtkKWEPaintbrushProperty *New();
  vtkKWEPaintbrushProperty();
  ~vtkKWEPaintbrushProperty();

  // Description:
  // Highlight ?
  void SetHighlight( int );
  vtkBooleanMacro ( Highlight, int );

  // Get the internal property.
  vtkGetObjectMacro( Property, vtkProperty );
  vtkProperty * Property;

  // Description:
  // Has the user specified a color. If this is true, the property manager will
  // stop autoassigning "quasi-unique" colors.
  bool HasUserSpecifiedColorOverride();

  // Description:
  void SetColorInternal( double rgb[3] );

  // Description:
  void SetInteractionEnabled(int i);

private:
  vtkKWEPaintbrushProperty(const vtkKWEPaintbrushProperty&);  // Not implemented.
  void operator=(const vtkKWEPaintbrushProperty&);  // Not implemented.

  vtkKWEPaintbrushSketch * PaintbrushSketch;
  int                   Mutable;
  char                * Identifier;
  bool                  UserSpecifiedColorOverride;
  int                   Highlight;
  float                 LineWidth;
  int                   HighlightType;
  double                HighlightThickness;
  double                Color[3];
  int                   Visibility;
  int                   InteractionEnabled;

  // Used when HighlightType is ColorHighlight
  double                HighlightColor[3];
};

#endif

