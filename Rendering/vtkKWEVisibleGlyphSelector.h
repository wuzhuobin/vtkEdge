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
// .NAME vtkKWEVisibleGlyphSelector - Select glyphs on a Glyph3DMapper.
// .SECTION Description
// Return the id of the points used as input of the Glyph3DMapper for which
// the glyphs have been selected.
// .SECTION See Also
// vtkKWEGlyph3DMapper,  vtkVisibleCellSelector

#ifndef __vtkKWEVisibleGlyphSelector_h
#define __vtkKWEVisibleGlyphSelector_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkSelection;
class vtkRenderer;
class vtkIdTypeArray;
class vtkProp;
class vtkIdentColoredPainter;
class vtkIntArray;
class vtkKWEGlyphSelectionRenderMode;

class VTKEdge_RENDERING_EXPORT vtkKWEVisibleGlyphSelector : public vtkObject
{
public:
//BTX
  enum
    {
      NOT_SELECTING = 0,
      COLOR_BY_PROCESSOR,
      COLOR_BY_ACTOR,
      COLOR_BY_GLYPH_ID_HIGH,
      COLOR_BY_GLYPH_ID_MID,
      COLOR_BY_GLYPH_ID_LOW
    };
//ETX
  vtkTypeRevisionMacro(vtkKWEVisibleGlyphSelector, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkKWEVisibleGlyphSelector *New();

  // Description:
  // Call to let this know where to select within.
  virtual void SetRenderer(vtkRenderer *);

  // Description:
  // Call to set the selection area region.
  // This crops the selected area to the renderers pixel limits.
  void SetArea(unsigned int x0, unsigned int y0,
               unsigned int x1, unsigned int y1);

  // Description:
  // The caller of SetArea can use to check for cropped limits.
  void GetArea(unsigned int &x0, unsigned int &y0,
               unsigned int &x1, unsigned int &y1);

  // Description:
  // Call to let this know what processor number to render as in the processor
  // select pass. Internally this adds 1 to pid because 0 is reserved for miss.
  virtual void SetProcessorId(unsigned int pid);
  vtkGetMacro(ProcessorId, unsigned int);

  // Description:
  // Call to let this know what selection render passes to do.
  // If you have only one processor or one actor, you can leave DoProcessor 
  // and DoActor as false (the default). If you have less than 2^48 glyphs in 
  // any actor, you do not need the GlyphIdHi pass, or similarly if you have 
  // less than 2^24 glyphs, you do not need DoGlyphIdMid.
  void SetRenderPasses(int DoProcessor, int DoActor, 
                       int DoGlyphIdHi, int DoGlyphIdMid, int DoGlyphIdLo);

  // Description:
  // Execute the selection algorithm.
  void Select();

  // Description:
  // After Select(), this will return the list of selected Ids.
  // The ProcessorId and Actor Id are returned in the first two components.
  // The CellId is returned in the last two components (only 64 bits total).
  void GetSelectedIds(vtkIdTypeArray *ToCopyInto);

  // Description:
  // After Select(), this will return the list of selected Ids.
  void GetSelectedIds(vtkSelection *ToCopyInto);

  // Description:
  // After a select, this will return a pointer to the actor corresponding to
  // a particular id. This will return NULL if id is out of range.
  //  vtkProp *GetActorFromId(vtkIdType id);

  // Description:
  // For debugging - prints out the list of selected ids.
  void PrintSelectedIds(vtkIdTypeArray *IdsToPrint);

  // Description:
  // Get the glyphId, actor, processor rendering the actor at a 
  // given display position. Makes sense only after Select() has been called.
  void GetPixelSelection( int displayPos[2],
                          vtkIdType & procId,
                          vtkIdType & glyphId,
                          vtkProp  *& actorPtr );  

protected:
  vtkKWEVisibleGlyphSelector();
  ~vtkKWEVisibleGlyphSelector();

  // Description:
  // Give this a selected region of the render window after a selection render
  // with one of the passes defined above.
  void SavePixelBuffer(int pass, unsigned char *src);

  // Description:
  // After one or more calls to SavePixelBuffer(), this will convert the saved
  // pixel buffers into a list of Ids.
  void ComputeSelectedIds();

  // Description:
  // Simply calls this->Renderer's method of the same name.
  void SetSelectMode(int mode);

  // Simply calls this->Renderer's method of the same name.
  void SetSelectConst(unsigned int constant);

  // Description:
  //  void SetIdentPainter(vtkIdentColoredPainter *);

  vtkRenderer *Renderer;

  int DoProcessor;
  int DoActor;
  int DoGlyphIdHi;
  int DoGlyphIdMid;
  int DoGlyphIdLo;

  unsigned int ProcessorId;

  unsigned int X0;
  unsigned int Y0;
  unsigned int X1;
  unsigned int Y1;

  //buffer for id colored pixels
  unsigned char *PixBuffer[5];

  // Description:
  // The results of the selection: processorIds, ActorIds, GlyphIds.
  vtkIdTypeArray *SelectedIds;
  vtkIntArray *PixelCounts;

  // vtkIdentColoredPainter *IdentPainter;

  vtkKWEGlyphSelectionRenderMode *Delegate;

private:
  vtkKWEVisibleGlyphSelector(const vtkKWEVisibleGlyphSelector&); //Not implemented
  void operator=(const vtkKWEVisibleGlyphSelector&); //Not implemented
};

#endif
