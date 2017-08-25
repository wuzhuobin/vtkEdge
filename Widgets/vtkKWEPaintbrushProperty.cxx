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

#include "vtkKWEPaintbrushProperty.h"

#include "vtkProperty.h"
#include "vtkObjectFactory.h"
#include "vtkKWEPaintbrushSketch.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkKWEPaintbrushProperty, "$Revision: 712 $");

//----------------------------------------------------------------------------
vtkKWEPaintbrushProperty* vtkKWEPaintbrushProperty::New()
{
  return vtkKWEPaintbrushProperty::New(NULL);
}

//----------------------------------------------------------------------------
// Implement the standard form of the New() method.
vtkKWEPaintbrushProperty* vtkKWEPaintbrushProperty::New(vtkKWEPaintbrushSketch * s)
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkKWEPaintbrushProperty");
  if(ret)
    {
    static_cast<vtkKWEPaintbrushProperty*>(ret)->PaintbrushSketch = s;
    return static_cast<vtkKWEPaintbrushProperty*>(ret);
    }
  vtkKWEPaintbrushProperty * p = new vtkKWEPaintbrushProperty;
  p->PaintbrushSketch = s;
  return p;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushProperty::vtkKWEPaintbrushProperty()
{
  this->Mutable                    = 0;
  this->PaintbrushSketch           = NULL;
  this->Identifier                 = NULL;
  this->Property                   = vtkProperty::New();
  this->UserSpecifiedColorOverride = false;
  this->LineWidth                  = 1.0;
  this->HighlightType              = ColorHighlight;
  this->Highlight                  = 0;

  // When highlighted, the lines are thrice as thick.
  // Only used if HighlightType is ThickHighlight
  this->HighlightThickness         = 3.0;

  // The default HighlightColor is yellow.
  // Only used if HighlightType is ColorHighlight
  this->HighlightColor[0] = this->HighlightColor[1] = 1.0;
  this->HighlightColor[2] = 0.0;
}

//----------------------------------------------------------------------------
vtkKWEPaintbrushProperty::~vtkKWEPaintbrushProperty()
{
  this->SetIdentifier(NULL);
  this->Property->Delete();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetOpacity( double o )
{
  this->Property->SetOpacity( o < 0.0 ? 0.0 : (o > 1.0 ? 1.0 : o) );
}

//----------------------------------------------------------------------------
double vtkKWEPaintbrushProperty::GetOpacity()
{
  return this->Property->GetOpacity();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetLineWidth( float o )
{
  // The factor accounts for highlighting. If its 2.0, its highlighted, if
  // its 1.0, its not.
  const double factor = this->Property->GetLineWidth()/this->LineWidth;

  this->Property->SetLineWidth(
    static_cast<float>(factor * static_cast<double>(o)) );
  this->LineWidth = o;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::GetColor( unsigned char rgb[3] )
{
  double rgbd[3];
  this->Property->GetColor(rgbd);
  rgb[0] =  static_cast< unsigned char >(rgbd[0] * 255.0);
  rgb[1] =  static_cast< unsigned char >(rgbd[1] * 255.0);
  rgb[2] =  static_cast< unsigned char >(rgbd[2] * 255.0);
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::GetColor( double rgb[3] )
{
  this->Property->GetColor(rgb);
}

//----------------------------------------------------------------------------
double * vtkKWEPaintbrushProperty::GetColor()
{
  return this->Property->GetColor();
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetColor( double rgb[3] )
{
  this->SetColorInternal( rgb );
  this->UserSpecifiedColorOverride = true;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetColorInternal( double rgb[3] )
{
  this->Property->SetColor(rgb);

  // The edge color is the inverse of the face color. We will use the edge 
  // color for highlighting. Using the same color, espcially when in overlay
  // mode makes it hard for us to see the highlighting.
  this->Property->SetEdgeColor(1.0-rgb[0], 1.0-rgb[1], 1.0-rgb[2]);  
  this->Color[0] = rgb[0];
  this->Color[1] = rgb[1];
  this->Color[2] = rgb[2];
}

//----------------------------------------------------------------------------
// Return the modification for this object.
unsigned long int vtkKWEPaintbrushProperty::GetMTime() 
{
  unsigned long int mtime  = this->Superclass::GetMTime();
  unsigned long int mtime2 = this->Property->GetMTime();
  return (mtime > mtime2 ? mtime : mtime2);
}

//----------------------------------------------------------------------------
// Return the modification for this object.
bool vtkKWEPaintbrushProperty::HasUserSpecifiedColorOverride() 
{
  return this->UserSpecifiedColorOverride;
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetHighlight( int h )
{
  this->Highlight = h;

  // Based on one of the three HighlightTypes, set the vtkProperty 
  // appropriately.

  // 1. Stippled Inverted highlight ..
  this->Property->SetEdgeVisibility( 
    (this->HighlightType == StippledInvertedHighlight && h == 1) ? 1 : 0 );
  this->Property->SetLineStipplePattern(
    (this->HighlightType == StippledInvertedHighlight && h == 1) ? 
      0x000f : 0xffff );

  // 2. Thick highlight ..
  this->Property->SetLineWidth(static_cast<float>( 
    (this->HighlightType == ThickHighlight && h == 1) ? 
    this->HighlightThickness*this->LineWidth : this->LineWidth ));

  // 3. Color highlight ..
  this->Property->SetColor( 
    (this->HighlightType == ColorHighlight && h == 1) ? 
                    this->HighlightColor : this->Color );
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::SetMutable( int m )
{
  int a = m ? 1 : 0; // Clamp to Range: [0-1]
  if (this->Mutable != a)
    {
    this->Mutable = a;
    this->PaintbrushSketch->SetMutable(a);
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkKWEPaintbrushProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
