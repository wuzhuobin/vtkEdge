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
// .NAME vtkKWEPaintbrushWidgetCallbackMapper - map widget events into callbacks
// .SECTION Description
// vtkKWEPaintbrushWidgetCallbackMapper maps widget events into static callback
// methods on the paintbrush widget. This class may be overridden to customize
// the paintbrush widget interactions.
// 
// .SECTION Interactions
//<P>
// The follwing interactions are defined by this class when the brush is 
// in Edit mode.
//   Left click and drag       - Draws a stroke
//   Ctrl-Left click and drag  - Erase stroke
//   Shift Left click drag     - Draws a stroke on this sketch alone. All other sketches are deemed uneditable while the shift key is depressed.
//   Shft Ctrl Left click drag - Erase stroke on this sketch alone. All other sketches are deemed uneditable while the shift key is depressed.
//   Backspace or Left key     - Undo the sketch (deletes the previously drawn stroke in a sketch)
//   Right key                 - Redo on the given sketch
//   Shift "+"                 - Start a new sketch.
//   Shift Right               - Traverse to the next sketch.
//   Shift Left                - Traverse to the previous sketch.
//   Shift Backspace           - Delete a sketch.
//   Right click and drag      - Resize the shape isotropically
//   Ctrl-Right click and drag - Resize the shape anisotropically
//   Shift "<"                 - Decrease the opacity of the overlaid drawing
//   Shift ">"                 - Increase the opacity of the overlaid drawing
//   "l" key                   - Render the current sketch immutable/mutable. (Supported only when editing labelmaps)
//   Escape                    - Enable/Disable interactions
//<P>
// The follwing interactions are defined by this class when the brush is in Select Mode
//   Left click                - Select/Unselect a sketch
//   Left click and drag       - Drag and drop merge of sketches.
//   Backspace key             - Delete selected sketches.
//   "m" key                   - Merge selected sketches.
//   Escape                    - Enable/Disable interactions
// 
//
// .SECTION See Also
// vtkWidgetEvent vtkWidgetEventTranslator

#ifndef __vtkKWEPaintbrushWidgetCallbackMapper_h
#define __vtkKWEPaintbrushWidgetCallbackMapper_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "vtkWidgetCallbackMapper.h"

class vtkKWEAbstractPaintbrushWidget;
class vtkKWEPaintbrushWidget;

class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushWidgetCallbackMapper : public vtkWidgetCallbackMapper
{
public:
  // Description:
  // Instantiate the class.
  static vtkKWEPaintbrushWidgetCallbackMapper *New();

  // Description:
  // Standard macros.
  vtkTypeMacro(vtkKWEPaintbrushWidgetCallbackMapper,vtkWidgetCallbackMapper);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the paintbrush widget. No reference counting to avoid loops.
  void SetPaintbrushWidget( vtkKWEAbstractPaintbrushWidget * w );

  // Description:
  // Set the default bindings. If you want to set your own bindings, you 
  // would create your own subclass of vtkKWEPaintbrushWidgetCallbackMapper 
  // and override the Bindings() method.
  virtual void Bindings();

protected:
  vtkKWEPaintbrushWidgetCallbackMapper();
  ~vtkKWEPaintbrushWidgetCallbackMapper();

  vtkKWEAbstractPaintbrushWidget * PaintbrushWidget;

private:
  vtkKWEPaintbrushWidgetCallbackMapper(const vtkKWEPaintbrushWidgetCallbackMapper&); //Not implemented
  void operator=(const vtkKWEPaintbrushWidgetCallbackMapper&); //Not implemented

};


#endif /* __vtkKWEPaintbrushWidgetCallbackMapper_h */

