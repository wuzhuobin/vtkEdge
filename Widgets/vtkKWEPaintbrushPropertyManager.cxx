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
#include "vtkKWEPaintbrushPropertyManager.h"

#include "vtkKWEPaintbrushSketch.h"
#include "vtkKWEPaintbrushDrawing.h"
#include "vtkKWEPaintbrushProperty.h"
#include "vtkKWEPaintbrushData.h"
#include "vtkProperty.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEPaintbrushPropertyManager, "$Revision: 3416 $");

//----------------------------------------------------------------------------
vtkKWEPaintbrushPropertyManager* vtkKWEPaintbrushPropertyManager::New()
{
  return vtkKWEPaintbrushPropertyManager::New(NULL);
}

//----------------------------------------------------------------------------
// Implement the standard form of the New() method.
vtkKWEPaintbrushPropertyManager* vtkKWEPaintbrushPropertyManager::New(vtkKWEPaintbrushDrawing * s)
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkKWEPaintbrushPropertyManager");
  if(ret)
    {
    static_cast<vtkKWEPaintbrushPropertyManager*>(ret)->PaintbrushDrawing = s;
    return static_cast<vtkKWEPaintbrushPropertyManager*>(ret);
    }
  vtkKWEPaintbrushPropertyManager * p = new vtkKWEPaintbrushPropertyManager;
  p->PaintbrushDrawing = s;
  return p;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushPropertyManager::vtkKWEPaintbrushPropertyManager()
{
  this->PaintbrushDrawing = NULL;
  this->HighlightType = vtkKWEPaintbrushProperty::ColorHighlight;

  this->Colors[ ColorType( 1.0, 0.4, 0.4, 0  )] = 0;
  this->Colors[ ColorType( 0.4, 1.0, 0.4, 1  )] = 0;
  this->Colors[ ColorType( 0.4, 0.4, 1.0, 2  )] = 0;
  this->Colors[ ColorType( 1.0, 0.0, 0.4, 3  )] = 0;
  this->Colors[ ColorType( 1.0, 0.4, 1.0, 4  )] = 0;
  this->Colors[ ColorType( 0.4, 1.0, 1.0, 5  )] = 0;
  this->Colors[ ColorType( 1.0, 0.2, 0.5, 6  )] = 0;
  this->Colors[ ColorType( 0.3, 0.7, 0.0, 7  )] = 0;
  this->Colors[ ColorType( 0.5, 0.2, 1.0, 8  )] = 0;
  this->Colors[ ColorType( 1.0, 0.5, 0.0, 9  )] = 0;
  this->Colors[ ColorType( 0.0, 1.0, 0.0, 10  )] = 0;
  this->Colors[ ColorType( 0.0, 0.0, 1.0, 11  )] = 0;
  this->Colors[ ColorType( 1.0, 0.2, 0.8, 12  )] = 0;
  this->Colors[ ColorType( 0.2, 1.0, 0.8, 13  )] = 0;
  this->Colors[ ColorType( 0.2, 0.4, 0.8, 14  )] = 0;
  this->Colors[ ColorType( 0.9, 0.5, 0.2, 15  )] = 0;
  this->Colors[ ColorType( 0.4, 0.9, 0.2, 16  )] = 0;
  this->Colors[ ColorType( 0.5, 0.2, 0.9, 17  )] = 0;
  this->Colors[ ColorType( 0.7, 0.3, 0.9, 18  )] = 0;
  this->Colors[ ColorType( 0.9, 0.3, 0.5, 19  )] = 0;
  this->Colors[ ColorType( 0.3, 0.9, 1.0, 20  )] = 0;

  // Clear our fast lookup label map to color table. Clearly we need to do this
  // only if we are using a UCHAR label map, since that's the only case when
  // fast lookup is exercised.
  if (vtkKWEPaintbrushEnums::GetLabelType() == VTK_UNSIGNED_CHAR)
    {
    for (int i = 0; i < 256; i++)
      {
      for (int j = 0; j < 3; j++)
        {
        this->LabelToColorMapUC[i][j] = 0;
        }
      this->LabelToOpacityMapUC[i] = 1.0;
      }
    }
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushPropertyManager::~vtkKWEPaintbrushPropertyManager()
{
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::Initialize()
{
  for( ColorsMapType::iterator iter = Colors.begin();
            iter != Colors.end(); ++iter ) iter->second = 0;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::AddUsedColor( vtkProperty * p )
{
  double rgb[3];
  p->GetColor( rgb );
  ColorType c( rgb[0], rgb[1], rgb[2] );
  AddUsedColor(c);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::AddUsedColor( const ColorType &c )
{
  if (c.R > 0.0 || c.G > 0.0 || c.B > 0.0)
    {
    for( ColorsMapType::iterator iter = Colors.begin();
                         iter != Colors.end(); ++iter )
      {
      if (iter->first == c) ++iter->second;
      }
    }
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushPropertyManager::ColorType vtkKWEPaintbrushPropertyManager::RequestColor()
{
  unsigned int minVal = VTK_INT_MAX;
  ColorType c(0.0,0.0,0.0,0);
  for( ColorsMapType::iterator iter = Colors.begin();
                         iter != Colors.end(); ++iter )
    {
    if (iter->second < minVal)
      {
      c = iter->first;
      minVal = iter->second;
      }
    }
  ++Colors[c];
  return c;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::
RetrieveSketchColor( vtkKWEPaintbrushSketch *s, unsigned char c[3] )
{
  ColorType color = SketchToColorMap[s];
  c[0] = static_cast< unsigned char >(color.R * 255.0);
  c[1] = static_cast< unsigned char >(color.G * 255.0);
  c[2] = static_cast< unsigned char >(color.B * 255.0);
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushProperty * vtkKWEPaintbrushPropertyManager::
RequestColorForSketch( vtkKWEPaintbrushSketch *s )
{
  // If the user overrode with his own fancy color, let's not displease
  // him by assigning our own.
  if (!s->GetPaintbrushProperty()->HasUserSpecifiedColorOverride())
    {
    // Give this sketch a nice color and put it in our table.
    this->SketchToColorMap[s] = ColorType( 1.0, 0.4, 0.4, 0  );
    this->SketchToColorMap[s] = RequestColor();
    double rgb[3] = { this->SketchToColorMap[s].R,
                      this->SketchToColorMap[s].G,
                      this->SketchToColorMap[s].B };
    s->GetPaintbrushProperty()->SetColorInternal( rgb );
    }

  return s->GetPaintbrushProperty();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::Update()
{
  // Remove unused sketches from our map.

  for( SketchToColorMapType::iterator iter =
       this->SketchToColorMap.begin(); iter !=
           this->SketchToColorMap.end(); )
    {
    SketchToColorMapType::iterator iter2 = iter;
    ++iter;
    if (this->PaintbrushDrawing->IsItemPresent(iter2->first) == 0)
      {
      // Remove this sketch from our color map. It no-longer exists.
      // Maybe someone deleted it.
      --(this->Colors[iter2->second]);
      this->SketchToColorMap.erase(iter2);
      }
    }

  // Now create new colors for the sketches that aren't in the map. These
  // are most likely new sketches. Try to assign unique colors. If unique
  // isn't possible, at least assign the least used colors.

  for (int n = 0; n < this->PaintbrushDrawing->GetNumberOfItems(); n++)
    {
    vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
    if (this->SketchToColorMap.find(s)
          == this->SketchToColorMap.end())
      {
      // This sketch isn't in our table. Put it in the table and give it a
      // nice color.
      this->RequestColorForSketch(s);
      }

    // Propagate our Highlight type to all properties in the drawing.
    s->GetPaintbrushProperty()->SetHighlightType(this->HighlightType);
    }

  this->UpdateLabelToColorMap();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::UpdateLabelToColorMap()
{
  if (this->PaintbrushDrawing->GetRepresentation() == vtkKWEPaintbrushEnums::Label)
    {
    if (vtkKWEPaintbrushEnums::GetLabelType() == VTK_UNSIGNED_CHAR)
      {
      for (int n = 0; n < this->PaintbrushDrawing->GetNumberOfItems(); n++)
        {
        vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
        s->GetPaintbrushProperty()->GetColor(
            this->LabelToColorMapUC[s->GetLabel()]);
        this->LabelToOpacityMapUC[s->GetLabel()]
          = s->GetPaintbrushProperty()->GetVisibility() ?
              s->GetPaintbrushProperty()->GetOpacity() : 0.0;
        }
      }
    else
      {
      unsigned char rgb[3];
      this->LabelToColorMap.clear();
      for (int n = 0; n < this->PaintbrushDrawing->GetNumberOfItems(); n++)
        {
        vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
        s->GetPaintbrushProperty()->GetColor(rgb);
        double opacity = s->GetPaintbrushProperty()->GetVisibility() ?
              s->GetPaintbrushProperty()->GetOpacity() : 0.0;
        this->LabelToColorMap[s->GetLabel()]
          = vtkKWEPaintbrushLabelMapColor(rgb, opacity);
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager
::GrabFocus( vtkKWEPaintbrushSketch *sketch )
{
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();
  for (int n = 0; n < nSketches; n++)
    {
    vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
    s->GetPaintbrushProperty()->SetInteractionEnabled(s == sketch);
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::ReleaseFocus()
{
  const int nSketches = this->PaintbrushDrawing->GetNumberOfItems();
  for (int n = 0; n < nSketches; n++)
    {
    vtkKWEPaintbrushSketch * s = this->PaintbrushDrawing->GetItem(n);
    s->GetPaintbrushProperty()->SetInteractionEnabled(1);
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushPropertyManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
