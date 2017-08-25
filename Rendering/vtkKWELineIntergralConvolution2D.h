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
// .NAME vtkKWELineIntergralConvolution2D - implementation for Line Intergral
// Convolution
// .SECTION Description
// GPU implementation of a Line Integral Convolution, a technique for
// imaging  vector fields.
//
// .SECTION Required OpenGL Extensins
// GL_ARB_texture_non_power_of_two
// GL_VERSION_2_0
// GL_ARB_texture_float
// GL_ARB_draw_buffers
// GL_EXT_framebuffer_object

#ifndef __vtkKWELineIntergralConvolution2D_h
#define __vtkKWELineIntergralConvolution2D_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkRenderWindow;
class vtkTextureObject;

class VTKEdge_RENDERING_EXPORT vtkKWELineIntergralConvolution2D : public vtkObject
{
public:
  static vtkKWELineIntergralConvolution2D* New();
  vtkTypeRevisionMacro(vtkKWELineIntergralConvolution2D, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // Number of steps. Initial value is 1.
  // class invariant: Steps>0.
  // In term of visual quality, the greater the better.
  vtkSetMacro(Steps,int);
  vtkGetMacro(Steps,int);

  // Description:
  // The the magnification factor. Default is 1.0.
  vtkSetClampMacro(Magnification, int, 1, VTK_INT_MAX);
  vtkGetMacro(Magnification, int);
  
  // Description:
  // Get/Set the step size. This is the length of the step in normalized image
  // space i.e. in range [0, 1].
  // Initial value is 0.01.
  // class invariant: StepSize>0.0.
  // In term of visual quality, the smaller the better.
  // The type for the interface is double as VTK interface is double
  // but GPU only supports float. This value will be converted to
  // float in the execution of the algorithm.
  vtkSetClampMacro(StepSize,double, 0.0, 1.0);
  vtkGetMacro(StepSize,double);
  
  // Description:
  // Vector field. Initial value is NULL.
  void SetVectorField(vtkTextureObject *vectorField);
  vtkGetObjectMacro(VectorField,vtkTextureObject);
  
  // Description:
  // Noise texture. Initial value is NULL.
  void SetNoise(vtkTextureObject *noise);
  vtkGetObjectMacro(Noise,vtkTextureObject);

  // Description:
  // Set the spacing in each dimension of the plane on which the vector field 
  // is defined. This class performs LIC in the normalized image space. Hence,
  // generally, it needs to transform the input vector field (given in physical
  // space) to the normalized image space. The Spacing is needed to determine
  // the tranform. Default is (1.0, 1.0). It is possible to disable vector
  // transformation by setting TransformVectors to 0.
  vtkSetVector2Macro(Spacing, double);
  vtkGetVector2Macro(Spacing, double);

  // Description:
  // This class performs LIC in the normalized image space. Hence, by default it
  // transforms the input vectors to the normalized image space (using the
  // Spacing and input VectorField dimensions). Set this to 0 to disable
  // tranformation if the vectors are already tranformed.
  vtkSetClampMacro(TransformVectors, int, 0, 1);
  vtkBooleanMacro(TransformVectors, int);
  vtkGetMacro(TransformVectors, int);

  // Description:
  // If VectorField has >= 3 components, we must choose which 2 components form
  // the (X,Y) component for the vector field.
  // Must be in the range [0, 3].
  vtkSetVector2Macro(Components, int);
  vtkGetVector2Macro(Components, int);
  
  // Description:
  // Perform the LIC. Set the LIC texture.
  // Return value is 1 if no error.
  int Execute();

  // Description:
  // Same as Execute() except that the LIC operation is performed only on a
  // window (given by the \c extent) in the input VectorField. The \c extent is
  // relative to the input VectorField. The output LIC image will be of the size
  // specified by extent.
  int Execute(unsigned int extent[4]);
  int Execute(int extent[4]);
  
  // Description:
  // LIC texture. Initial value is NULL.
  // Set by Execute().
  void SetLIC(vtkTextureObject *lic);
  vtkGetObjectMacro(LIC,vtkTextureObject);
 
  // Description:
  // On machines where the vector field texture is clamped between [0,1], one can
  // specify the shift/scale factor used to convert the original vector field to
  // lie in the clamped range. Default is (0.0, 1.0);
  void SetVectorShiftScale(double shift, double scale)
    {
    this->Shift = shift;
    this->Scale = scale;
    this->Modified();
    }


  // Description:
  // Returns if the context supports the required extensions.
  static bool IsSupported(vtkRenderWindow* renWin);

protected:
   vtkKWELineIntergralConvolution2D();
  ~vtkKWELineIntergralConvolution2D();
 
  int Magnification;
  int Steps;
  double StepSize;
  double Shift;
  double Scale;
  
  vtkTextureObject *VectorField;
  vtkTextureObject *Noise;
  vtkTextureObject *LIC;
  int Components[2];
  double Spacing[2];
  int TransformVectors;
  
private:
  vtkKWELineIntergralConvolution2D(const vtkKWELineIntergralConvolution2D&); // Not implemented.
  void operator=(const vtkKWELineIntergralConvolution2D&); // Not implemented.
//ETX
};

#endif
