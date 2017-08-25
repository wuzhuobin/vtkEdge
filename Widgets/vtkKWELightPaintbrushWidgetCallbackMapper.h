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
// .NAME vtkKWELightPaintbrushWidgetCallbackMapper - map widget events into callbacks
// .SECTION Description
// vtkKWELightPaintbrushWidgetCallbackMapper is a subclass of vtkKWEPaintbrushWidgetCallbackMapper.
// It is an example of how one may override the default bindings to set their own
// bindings on the paintbrush widget. To use this class, one sets the callback mapper
// on the widget. 
//
// .SECTION Interactions
//<P>
// The follwing interactions are defined by this class when the brush is 
// in Edit mode.
//   Left click and drag       - Draws a stroke
//   Ctrl-Left click and drag  - Erase stroke
//   Shift Left click drag     - Draws a stroke on this sketch alone. All other sketches are deemed uneditable while the shift key is depressed.
//   Shft Ctrl Left click drag - Erase stroke on this sketch alone. All other sketches are deemed uneditable while the shift key is depressed.
//   Right click and drag      - Resize the shape isotropically
// 
// The Select mode is not supported/defined by this class.
//
// .SECTION See Also
// vtkWidgetEvent vtkWidgetEventTranslator

#ifndef __vtkKWELightPaintbrushWidgetCallbackMapper_h
#define __vtkKWELightPaintbrushWidgetCallbackMapper_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkKWEPaintbrushWidgetCallbackMapper.h"

class VTKEdge_WIDGETS_EXPORT vtkKWELightPaintbrushWidgetCallbackMapper : public vtkKWEPaintbrushWidgetCallbackMapper
{
public:
  // Description:
  // Instantiate the class.
  static vtkKWELightPaintbrushWidgetCallbackMapper *New();

  // Description:
  // Standard macros.
  vtkTypeMacro(vtkKWELightPaintbrushWidgetCallbackMapper,vtkKWEPaintbrushWidgetCallbackMapper);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Overriding the superclass method
  virtual void Bindings();

protected:
  vtkKWELightPaintbrushWidgetCallbackMapper() {};
  ~vtkKWELightPaintbrushWidgetCallbackMapper() {};

private:
  vtkKWELightPaintbrushWidgetCallbackMapper(const vtkKWELightPaintbrushWidgetCallbackMapper&); //Not implemented
  void operator=(const vtkKWELightPaintbrushWidgetCallbackMapper&); //Not implemented

};


#endif /* __vtkKWELightPaintbrushWidgetCallbackMapper_h */


