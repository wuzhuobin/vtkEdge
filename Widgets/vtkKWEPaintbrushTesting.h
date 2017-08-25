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
// .NAME vtkKWEPaintbrushTesting - class providing framework for Paintbrush testing
// .SECTION Description
// vtkKWEPaintbrushTesting provides convenience utilities for paintbrush testing 
// such as a 3 viewer slice viewer etc..

#ifndef __vtkKWEPaintbrushTesting_h
#define __vtkKWEPaintbrushTesting_h

#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <vtksys/SystemTools.hxx>
#include <vtksys/CommandLineArguments.hxx>
#include <vtksys/stl/string>
#include <vtksys/stl/vector>
#include <iostream>
#include "VTKEdgeConfigure.h" // export

class vtkImageViewer2;
class vtkImageReader2;
class vtkKWEPaintbrushWidget;
class vtkSliderWidget;
class vtkKWEPaintbrushTestingSliderCallback;

// ------- The window class used by all examples ------------
class VTKEdge_WIDGETS_EXPORT vtkKWEPaintbrushTesting : public vtkObject
{
public:
  static vtkKWEPaintbrushTesting* New();
  vtkTypeRevisionMacro(vtkKWEPaintbrushTesting,vtkObject);

  // Description:
  // Initialize() must be called before Run()
  void Initialize();
  int Run();

  // Description:
  // Filename of the data to be loaded.
  vtkSetStringMacro( Filename );
  vtkGetStringMacro( Filename );

  // Description:
  // Default is a single render window. However you can display
  // Axial, Sagittal, Coronal, Blank render windows simultaneously
  vtkSetMacro( FourPaneView, int );
  vtkGetMacro( FourPaneView, int );
  vtkBooleanMacro( FourPaneView, int );

  // Description:
  // Get the image actors
  vtkImageActor * GetAxialImageActor()    { return this->GetNthImageActor(0); }
  vtkImageActor * GetCoronalImageActor()  { return this->GetNthImageActor(1); }
  vtkImageActor * GetSagittalImageActor() { return this->GetNthImageActor(2); }
  vtkImageActor * GetNthImageActor( int i );
  vtkImageViewer2 * GetNthImageViewer( int i ) { return this->ImageViewer[i]; }
  
  // Description:
  // Set/Get the input image data that the actors are showing. This may be
  // set explicitly as below, or may be supplied via a user-argument to the
  // example application as "--data somefile.mha" . If no input is specified,
  // a default head MRI image is loaded up.
  vtkGetObjectMacro( Input, vtkImageData );
  virtual void SetInput( vtkImageData * );

protected:
  vtkKWEPaintbrushTesting();
  ~vtkKWEPaintbrushTesting();

  vtkImageViewer2                           *ImageViewer[3];
  vtkKWEPaintbrushTestingSliderCallback        *SliderCallbacks[3];
  vtkSliderWidget                           *SliderWidget[3];
  vtkImageData                              *Input;
  char *                                    Filename;
  int                                       FourPaneView;
  void GetValidReader( vtkImageReader2 * & reader, const char * filename);

private:
  vtkKWEPaintbrushTesting(const vtkKWEPaintbrushTesting&);   // Not implemented.
  void operator=(const vtkKWEPaintbrushTesting&);  // Not implemented.
};

#endif
