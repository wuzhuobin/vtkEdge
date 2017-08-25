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
// .NAME vtkSMKWEGlyph3DMapperRepresentationProxy
// .SECTION Description
//

#ifndef __vtkSMKWEGlyph3DMapperRepresentationProxy_h
#define __vtkSMKWEGlyph3DMapperRepresentationProxy_h

#include "vtkSMSurfaceRepresentationProxy.h"
#include "vtkSmartPointer.h"
class vtkSMKWEGlyph3DMapperRepresentationProxy : public vtkSMSurfaceRepresentationProxy
{
public:
  static vtkSMKWEGlyph3DMapperRepresentationProxy* New();
  vtkTypeRevisionMacro(vtkSMKWEGlyph3DMapperRepresentationProxy, vtkSMSurfaceRepresentationProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // vtkSMInputProperty requires that the consumer proxy support
  // AddInput() method. Hence, this method is defined. This method
  // sets up the input connection.
  // Overridden to handle the glyph source input connection.
  virtual void AddInput(unsigned int inputPort, vtkSMSourceProxy* input,
    unsigned int outputPort, const char* method);
  virtual void AddInput(vtkSMSourceProxy* input,
                        const char* method)
     { this->Superclass::AddInput(input, method); }

  // Description:
  // Get the bounds and transform according to rotation, translation, and scaling.
  // Returns true if the bounds are "valid" (and false otherwise)
  virtual bool GetBounds(double bounds[6]);

//BTX
protected:
  vtkSMKWEGlyph3DMapperRepresentationProxy();
  ~vtkSMKWEGlyph3DMapperRepresentationProxy();

  // Description:
  // This method is called at the beginning of CreateVTKObjects().
  // If this method returns false, CreateVTKObjects() is aborted.
  // Overridden to abort CreateVTKObjects() only if the input has
  // been initialized correctly.
  virtual bool BeginCreateVTKObjects();

  // Description:
  // This method is called after CreateVTKObjects(). 
  // This gives subclasses an opportunity to do some post-creation
  // initialization.
  // Overridden to setup view time link.
  virtual bool EndCreateVTKObjects();

  // Description:
  // Some representations may require lod/compositing strategies from the view
  // proxy. This method gives such subclasses an opportunity to as the view
  // module for the right kind of strategy and plug it in the representation
  // pipeline. Returns true on success. Default implementation suffices for
  // representation that don't use strategies.
  virtual bool InitializeStrategy(vtkSMViewProxy* view);

  vtkSmartPointer<vtkSMProxy> Source;
  unsigned int SourceOutputPort;
  vtkSMProxy* GlyphMapper;
private:
  vtkSMKWEGlyph3DMapperRepresentationProxy(const vtkSMKWEGlyph3DMapperRepresentationProxy&); // Not implemented
  void operator=(const vtkSMKWEGlyph3DMapperRepresentationProxy&); // Not implemented
//ETX
};

#endif

