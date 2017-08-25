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
// .NAME vtkKWEPaintbrushPropertyManager - Manages a default set of properties corresponding to the collection of sketches
// .SECTION Description
// This class is not meant to be used by the user and hence is not exposed
// to the user. It is used internally by the paintbrush infrastructure to 
// assign a default set of properties as new sketches are added, so that 
// you get unique colors for each segmentation etc. It is also used by 
// the blender for optmizations in blending a label map containing 
// many sketches into a final overlay.

#ifndef __vtkKWEPaintbrushPropertyManager_h
#define __vtkKWEPaintbrushPropertyManager_h

#include "VTKEdgeConfigure.h"
#include "vtkKWEPaintbrushEnums.h"
#include "vtkObject.h"
#include <vtkstd/map>

class vtkKWEPaintbrushSketch;
class vtkKWEPaintbrushDrawing;
class vtkKWEPaintbrushProperty;
class vtkKWEPaintbrushBlend;
class vtkProperty;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushPropertyManager : public vtkObject
{
  //BTX
  friend class vtkKWEPaintbrushDrawing;
  friend class vtkKWEPaintbrushBlend;
  //ETX
public:
  vtkTypeRevisionMacro(vtkKWEPaintbrushPropertyManager,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // A drawing is always associated with a paintbrush sketch. A 
  // vtkKWEPaintbrushSketch upon creation will automatically be assigned a 
  // default property internally. You can, using this method, query the sketch 
  // to which this property is assigned.
  vtkGetObjectMacro( PaintbrushDrawing, vtkKWEPaintbrushDrawing );
  
  // Description:
  // Two highlight modes are allowed. 
  // Please see vtkKWEPaintbrushProperty::HighlightTypeEnum
  // Stippled inverted highlight looks better when you have an overlay, as with
  // vtkKWEPaintbrushRepresentation2D. ThickHighlight looks better when you render 
  // brushdata as contours as in vtkKWEPaintbrushRepresentationGrayscale2D.
  vtkSetMacro( HighlightType, int );
  vtkGetMacro( HighlightType, int );
  
  // Description:
  // Assign colors to sketches in the drawing, using some strategy to ensure, 
  // as far as possible some sort of uniqueness
  void Update();

  // Description:
  // INTERNAL - Do not Use.
  // Used to figure out used colors and assign "pleasing" defaultcolors to new
  // sketches. The representations typically Initialize the manager, 
  // AddUsedColors for all the sketches that have colors already and then 
  // request a color for the new sketch.
  void Initialize();
  void AddUsedColor( vtkProperty * p );
  void RetrieveSketchColor( vtkKWEPaintbrushSketch *s, unsigned char c[3] );
  vtkKWEPaintbrushProperty * RequestColorForSketch( vtkKWEPaintbrushSketch *s );

  // Description:
  // INTERNAL - Do not use
  class vtkKWEPaintbrushLabelMapColor 
    { 
    public:
      unsigned char Color[3];
      double        Opacity;
      vtkKWEPaintbrushLabelMapColor() { Color[0] = Color[1] = Color[2] = 0; }
      vtkKWEPaintbrushLabelMapColor( unsigned char r, 
                                  unsigned char g, 
                                  unsigned char b, 
                                  double opacity ) 
        { Color[0] = r; Color[1] = g; Color[2] = b; Opacity = opacity; }
      vtkKWEPaintbrushLabelMapColor( unsigned char rgba[3], double o ) 
        { Color[0] = rgba[0]; Color[1] = rgba[1]; Color[2] = rgba[2]; Opacity=o; }
      bool operator==(const vtkKWEPaintbrushLabelMapColor &a) const
        { return ( this->Color[0] == a.Color[0] && 
                   this->Color[1] == a.Color[1] &&
                   this->Color[2] == a.Color[2] ); }
    };
  typedef vtkstd::map< vtkKWEPaintbrushEnums::LabelType, 
               vtkKWEPaintbrushLabelMapColor > LabelToColorMapType;

protected:
  static vtkKWEPaintbrushPropertyManager *New( vtkKWEPaintbrushDrawing * );
  static vtkKWEPaintbrushPropertyManager *New();
  vtkKWEPaintbrushPropertyManager();
  ~vtkKWEPaintbrushPropertyManager();

  // Description:
  // Called from Update(). This is done only if the Drawing type is LabelMap
  void UpdateLabelToColorMap();

  // To provide default colors
  class vtkKWEPaintbrushColors
  {
  public:
    double R, G, B, Opacity; unsigned int Idx; 
    vtkKWEPaintbrushColors() { Opacity = 1.0; Idx = 0; }
    vtkKWEPaintbrushColors( double r, double g, double b, int idx = 0 ) 
      { R=r; G=g; B=b; Idx=idx; Opacity=0.0; }
    bool operator==(const vtkKWEPaintbrushColors &a) const
      { return (fabs(this->R - a.R) < 0.01 && 
                fabs(this->G - a.G) < 0.01 && 
                fabs(this->B - a.B) < 0.01); }
  };
  
  typedef vtkKWEPaintbrushColors ColorType;
  
  void AddUsedColor( const ColorType & );
  ColorType RequestColor();
  
private:
  vtkKWEPaintbrushPropertyManager(const vtkKWEPaintbrushPropertyManager&);
  void operator=(const vtkKWEPaintbrushPropertyManager&);
  
  vtkKWEPaintbrushDrawing * PaintbrushDrawing;
  struct vtkKWEPaintbrushColorsCompare 
    {
    bool operator()(const ColorType& s1, const ColorType& s2) const
      {
      return s1.Idx < s2.Idx;
      }    
    };

  typedef vtkstd::map< vtkKWEPaintbrushSketch *, 
                       ColorType > SketchToColorMapType;
  typedef vtkstd::map< ColorType, 
                       unsigned int, 
                       vtkKWEPaintbrushColorsCompare > ColorsMapType;
  SketchToColorMapType SketchToColorMap;
  ColorsMapType        Colors;
  int                  HighlightType;

  // Everything below this is used only if we our representation is a label map.
  //
  // When the LabelType isn't unsigned char, we are forced to use an std::map
  // to store the label to color lookup table. This will introduce some lag,
  // but what can we do ?
  unsigned char        LabelToColorMapUC[256][3];
  double               LabelToOpacityMapUC[256];
  LabelToColorMapType  LabelToColorMap;
};

#endif

