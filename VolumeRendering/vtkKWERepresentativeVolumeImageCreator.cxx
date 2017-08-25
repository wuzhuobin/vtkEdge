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

#include "vtkKWERepresentativeVolumeImageCreator.h"

#include "vtkObjectFactory.h"

#include "vtkColorTransferFunction.h"
#include "vtkCommand.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolumeProperty.h"

#include <assert.h>

vtkStandardNewMacro( vtkKWERepresentativeVolumeImageCreator );
vtkCxxRevisionMacro( vtkKWERepresentativeVolumeImageCreator, "$Revision: 1774 $");
vtkCxxSetObjectMacro(vtkKWERepresentativeVolumeImageCreator, Input, vtkImageData);
vtkCxxSetObjectMacro(vtkKWERepresentativeVolumeImageCreator, Property, vtkVolumeProperty);


#define vtkKWERVIC_Interpolate( VAL, PTR, VOXEL, C, DIM, COMPONENTS )   \
  {float _weight[3];                                                    \
  _weight[0] = static_cast<float>(VOXEL[0] - static_cast<float>(static_cast<int>(VOXEL[0]))); \
  _weight[1] = static_cast<float>(VOXEL[1] - static_cast<float>(static_cast<int>(VOXEL[1]))); \
  _weight[2] = static_cast<float>(VOXEL[2] - static_cast<float>(static_cast<int>(VOXEL[2]))); \
  float _A = static_cast<float>(*(PTR + C));                            \
  float _B = static_cast<float>(*(PTR + C + COMPONENTS));               \
  float _C = static_cast<float>(*(PTR + C + COMPONENTS*DIM[0]));        \
  float _D = static_cast<float>(*(PTR + C + COMPONENTS*(1 + DIM[0])));  \
  float _E = static_cast<float>(*(PTR + C + COMPONENTS*DIM[0]*DIM[1])); \
  float _F = static_cast<float>(*(PTR + C + COMPONENTS*(1 +DIM[0]*DIM[1]))); \
  float _G = static_cast<float>(*(PTR + C + COMPONENTS*(DIM[0] + DIM[0]*DIM[1]))); \
  float _H = static_cast<float>(*(PTR + C +                             \
                                  COMPONENTS*(1 +                       \
                                              DIM[0] +                  \
                                              DIM[0]*DIM[1])));         \
  VAL =                                                                 \
    (1.0f -_weight[0])*(1.0f -_weight[1])*(1.0f-_weight[2])*_A +           \
    (      _weight[0])*(1.0f -_weight[1])*(1.0f-_weight[2])*_B +           \
    (1.0f -_weight[0])*(      _weight[1])*(1.0f-_weight[2])*_C +           \
    (      _weight[0])*(      _weight[1])*(1.0f-_weight[2])*_D +           \
    (1.0f -_weight[0])*(1.0f -_weight[1])*(    _weight[2])*_E +           \
    (      _weight[0])*(1.0f -_weight[1])*(    _weight[2])*_F +           \
    (1.0f -_weight[0])*(      _weight[1])*(    _weight[2])*_G +           \
    (      _weight[0])*(      _weight[1])*(    _weight[2])*_H;}

#define vtkKWERVIC_ComputeGradient( N, PTR, VOXEL, C, DIM, COMPONENTS, ASPECT ) \
float _val1, _val2;                                                     \
float _voxel1[3];                                                       \
_voxel1[0] = VOXEL[0] - 1.0f;                                            \
_voxel1[1] = VOXEL[1];                                                  \
_voxel1[2] = VOXEL[2];                                                  \
vtkKWERVIC_Interpolate( _val1,                                          \
                        PTR-COMPONENTS,                                 \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
_voxel1[0] = VOXEL[0] + 1.0f;                                            \
vtkKWERVIC_Interpolate( _val2,                                          \
                        ptr+COMPONENTS,                                 \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
N[0] = _val2 - _val1;                                                   \
                                                                        \
_voxel1[0] = VOXEL[0];                                                  \
_voxel1[1] = VOXEL[1]-1.0f;                                             \
vtkKWERVIC_Interpolate( _val1,                                          \
                        ptr-COMPONENTS*DIM[0],                          \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
_voxel1[1] = VOXEL[1] + 1.0f;                                            \
vtkKWERVIC_Interpolate( _val2,                                          \
                        ptr+COMPONENTS*DIM[0],                          \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
N[1] = _val2 - _val1;                                                   \
                                                                        \
_voxel1[1] = VOXEL[1];                                                  \
_voxel1[2] = VOXEL[2] - 1.0f;                                            \
vtkKWERVIC_Interpolate( _val1,                                          \
                        ptr-COMPONENTS*DIM[0]*DIM[1],                   \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
_voxel1[2] = VOXEL[2] + 1.0f;                                            \
vtkKWERVIC_Interpolate( _val2,                                          \
                        ptr+COMPONENTS*DIM[0]*DIM[1],                   \
                        _voxel1, C, DIM, COMPONENTS );                  \
                                                                        \
N[2] = _val2 - _val1;                                                   \
N[0] /= aspect[0];                                                      \
N[1] /= aspect[1];                                                      \
N[2] /= aspect[2];


#define vtkKWERVIC_ShadeSample(RAY, N, A, D, S, P, R, G, B, C)  \
float _viewDir[3];                                              \
_viewDir[0] = RAY[0];                       \
_viewDir[1] = RAY[1];                       \
_viewDir[2] = RAY[2];                       \
vtkMath::Normalize(_viewDir);                                   \
                                                                \
float _n_dot_l = vtkMath::Dot(N,_viewDir);                      \
                                                                \
float _tmp1 = A;                                                \
float _tmp2 = 0.0f;                                              \
                                                                \
if ( _n_dot_l > 0.0f )                                           \
  {                                                             \
    _tmp1 += D*_n_dot_l;                                        \
    _tmp2 = S * powf( _n_dot_l,P ); \
  }                                                             \
                                                                \
C[0] = _tmp1*R + _tmp2;                                         \
C[1] = _tmp1*G + _tmp2;                                         \
C[2] = _tmp1*B + _tmp2;

// ----------------------------------------------------------------------------
// The only purpose of this class is to be a friend to
// vtkKWERepresentativeVolumeImageCreator so that we can access the interal
// variables without having to expose them as part of the public API
// ----------------------------------------------------------------------------
class vtkKWERVICFriend
{
public:
  vtkKWERepresentativeVolumeImageCreator *Creator;

  vtkImageData *GetInput() {return Creator->InternalInput;};
  vtkVolumeProperty *GetProperty() {return Creator->InternalProperty;};
  float *GetColorTable(int i) {return Creator->ColorTable[i];}
  float *GetOpacityTable(int i) {return Creator->OpacityTable[i];}
  float GetTableOffset(int i) {return Creator->TableOffset[i];}
  float GetTableScale(int i) {return Creator->TableScale[i];}

};


// ----------------------------------------------------------------------------
// This is the templated class for computing a max intensity image
// ----------------------------------------------------------------------------
template <class T>
void vtkKWERVIC_MIP( T *dataPtr,
                     float voxel[3],
                     float rayIncrement[3],
                     unsigned char color[3],
                     vtkKWERVICFriend *myFriend )
{
  // We need the input image data and the property
  vtkImageData *input = myFriend->GetInput();
  vtkVolumeProperty *property = myFriend->GetProperty();

  // Find out how many components we have
  int components = input->GetNumberOfScalarComponents();

  // Are the components independent?
  int independent = property->GetIndependentComponents();

  // What is our volume dimensions? We need this to keep our
  // ray inside the volume
  int dim[3];
  input->GetDimensions(dim);

  // Some variables we'll need to compute the final pixel color
  float maxValue[4];
  int c;
  float *colorTable[4];
  float *opacityTable[4];
  float tableOffset[4];
  float tableScale[4];
  float weight[4];

  // ptr is a pointer to the voxel location
  T *ptr = dataPtr + components * ( static_cast<int>(voxel[0]) +
                                    static_cast<int>(voxel[1]) * dim[0] +
                                    static_cast<int>(voxel[2]) * dim[0] * dim[1] );

  // For each component:
  //  - initialize the max value to the first value found
  //  - get the color and opacity tables
  //  - get the offset / scale values to transform scalar
  //    value into index
  //  - get the weight
  for ( c = 0; c < components; c++ )
    {
    maxValue[c] = static_cast<float>(ptr[c]);
    colorTable[c] = myFriend->GetColorTable(c);
    opacityTable[c] = myFriend->GetOpacityTable(c);
    tableOffset[c] = myFriend->GetTableOffset(c);
    tableScale[c] = myFriend->GetTableScale(c);
    weight[c] = static_cast<float>(property->GetComponentWeight(c));
    }


  // Keep stepping until we exit the volume. We'll be conservative and
  // considering exiting the volume to be passing into or beyong the last
  // voxel in each direction - that avoids boundary conditions in any
  // blending modes that require gradients
  while ( voxel[0] > 1.0 &&
          voxel[1] > 1.0 &&
          voxel[2] > 1.0 &&
          voxel[0] < (dim[0]-2) &&
          voxel[1] < (dim[1]-2) &&
          voxel[2] < (dim[2]-2) )
    {
    if ( independent )
      {
      // Check each component if we have a new max value
      for ( c = 0; c < components; c++ )
        {
        float val;
        vtkKWERVIC_Interpolate( val, ptr, voxel, c, dim, components );
        if ( val > maxValue[c] )
          {
          maxValue[c] = val;
          }
        }
      }
    else
      {
      float val;
      vtkKWERVIC_Interpolate( val, ptr, voxel, components-1, dim, components );
      if ( val > maxValue[components-1] )
        {
        maxValue[components-1] = val;
        for ( c = 0; c < components-1; c++ )
          {
          vtkKWERVIC_Interpolate( val, ptr, voxel, c, dim, components );
          maxValue[c] = val;
          }
        }
      }

    // Increment to the next voxel
    voxel[0] += rayIncrement[0];
    voxel[1] += rayIncrement[1];
    voxel[2] += rayIncrement[2];

    // Move the pointer to this location
    ptr = dataPtr + components * ( static_cast<int>(voxel[0]) +
                                   static_cast<int>(voxel[1]) * dim[0] +
                                   static_cast<int>(voxel[2]) * dim[0] * dim[1] );

    }

  // Now compute the color. Exact computation will depend on whether
  // the component are independent or dependend (and if dependent, on
  // whether there are Need to consider each components contribution
  float tmpColor[4] = {0,0,0,0};
  int index;

  if ( independent )
    {
    for ( c = 0; c < components; c++ )
      {
      // Find the index into the table and look up RGBA
      index = static_cast<int>((maxValue[c] + tableOffset[c]) * tableScale[c] + 0.5);
      index = (index < 0 )?(0):((index>1023)?(1023):(index));
      float r = *(colorTable[c] + 3*index);
      float g = *(colorTable[c] + 3*index+1);
      float b = *(colorTable[c] + 3*index+2);
      float a = *(opacityTable[c] + index);

      // Add it in (multiplied by weight)
      tmpColor[0] += weight[c] * r * a;
      tmpColor[1] += weight[c] * g * a;
      tmpColor[2] += weight[c] * b * a;
      tmpColor[3] += weight[c] * a;
      }
    }
  else if ( components == 2 )
    {
    // Find the index into the table and look up A
    index = static_cast<int>((maxValue[1] + tableOffset[1]) * tableScale[1] + 0.5);
    tmpColor[3] = *(opacityTable[0] + index);

    // Find the index into the table and look up RGB
    index = static_cast<int>((maxValue[0] + tableOffset[0]) * tableScale[0] + 0.5);
    tmpColor[0] = *(colorTable[0] + 3*index  ) * tmpColor[3];
    tmpColor[1] = *(colorTable[0] + 3*index+1) * tmpColor[3];
    tmpColor[2] = *(colorTable[0] + 3*index+2) * tmpColor[3];
    }
  else if ( components == 4 )
    {
    // Find the index into the table and look up A
    index = static_cast<int>((maxValue[3] + tableOffset[3]) * tableScale[3] + 0.5);
    tmpColor[3] = *(opacityTable[0] + index);

    // Use first three components directly as RGB. We know this must be
    // unsigned char (only type allowed in this case) so we'll use that
    // knowledge to convert to a [0,1] range.
    tmpColor[0] = maxValue[0]/255.0f * tmpColor[3];
    tmpColor[1] = maxValue[1]/255.0f * tmpColor[3];
    tmpColor[2] = maxValue[2]/255.0f * tmpColor[3];
    }


  // Check bounds to make sure everything is between [0,1]
  tmpColor[0] = (tmpColor[0] < 0)?(0):((tmpColor[0]>1)?(1):(tmpColor[0]));
  tmpColor[1] = (tmpColor[1] < 0)?(0):((tmpColor[1]>1)?(1):(tmpColor[1]));
  tmpColor[2] = (tmpColor[2] < 0)?(0):((tmpColor[2]>1)?(1):(tmpColor[2]));
  tmpColor[3] = (tmpColor[3] < 0)?(0):((tmpColor[3]>1)?(1):(tmpColor[3]));

  // Now convert to unsigned char
  color[0] = static_cast<unsigned char>(tmpColor[0]*255.0 + 0.5);
  color[1] = static_cast<unsigned char>(tmpColor[1]*255.0 + 0.5);
  color[2] = static_cast<unsigned char>(tmpColor[2]*255.0 + 0.5);
}


// ----------------------------------------------------------------------------
// This is the templated class for computing a composite image
// ----------------------------------------------------------------------------
template <class T>
void vtkKWERVIC_Composite( T *dataPtr,
                           float voxel[3],
                           float rayIncrement[3],
                           unsigned char color[3],
                           vtkKWERVICFriend *myFriend )
{
  // We need the input image data and the property
  vtkImageData *input = myFriend->GetInput();
  vtkVolumeProperty *property = myFriend->GetProperty();

  double spacing[3];
  input->GetSpacing(spacing);

  double avgSpacing = (spacing[0]+spacing[1]+spacing[2])/3.0;

  // adjust the aspect
  float aspect[3];
  aspect[0] = static_cast<float>(spacing[0] * 2.0 / avgSpacing);
  aspect[1] = static_cast<float>(spacing[1] * 2.0 / avgSpacing);
  aspect[2] = static_cast<float>(spacing[2] * 2.0 / avgSpacing);

  // Find out how many components we have
  int components = input->GetNumberOfScalarComponents();

  // Are the components independent?
  int independent = property->GetIndependentComponents();

  // What is our volume dimensions? We need this to keep our
  // ray inside the volume
  int dim[3];
  input->GetDimensions(dim);

  // Some variables we'll need to compute the final pixel color
  int c;
  float *colorTable[4];
  float *opacityTable[4];
  float tableOffset[4];
  float tableScale[4];
  float weight[4];
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float specularPower[4];

  float accumColor[3] = {0,0,0};
  float remainingOpacity = 1.0;

  // ptr is a pointer to the voxel location
  T *ptr = dataPtr + components * ( static_cast<int>(voxel[0]) +
                                    static_cast<int>(voxel[1]) * dim[0] +
                                    static_cast<int>(voxel[2]) * dim[0] * dim[1] );

  // For each component:
  //  - get the color and opacity tables
  //  - get the offset / scale values to transform scalar
  //    value into index
  //  - get the weight
  for ( c = 0; c < components; c++ )
    {
    colorTable[c] = myFriend->GetColorTable(c);
    opacityTable[c] = myFriend->GetOpacityTable(c);
    tableOffset[c] = myFriend->GetTableOffset(c);
    tableScale[c] = myFriend->GetTableScale(c);
    weight[c] = static_cast<float>(property->GetComponentWeight(c));
    ambient[c] = static_cast<float>(property->GetAmbient(c));
    diffuse[c] = static_cast<float>(property->GetDiffuse(c));
    specular[c] = static_cast<float>(property->GetSpecular(c));
    specularPower[c] = static_cast<float>(property->GetSpecularPower(c));
    }


  // Keep stepping until we exit the volume. We'll be conservative and
  // considering exiting the volume to be passing into or beyong the last
  // voxel in each direction - that avoids boundary conditions in any
  // blending modes that require gradients
  while ( voxel[0] > 1 &&
          voxel[1] > 1 &&
          voxel[2] > 1 &&
          voxel[0] < (dim[0]-2) &&
          voxel[1] < (dim[1]-2) &&
          voxel[2] < (dim[2]-2) &&
          remainingOpacity > 0.02 )
    {
    float tmpColor[4] = {0,0,0,0};
    int index;
    if ( independent )
      {
      for ( c = 0; c < components; c++ )
        {
        float val;
        vtkKWERVIC_Interpolate( val, ptr, voxel, c, dim, components );
        index = static_cast<int>((val + tableOffset[c]) * tableScale[c] + 0.5);
        index = (index<0)?(0):((index>1023)?(1023):(index));
        float r = *(colorTable[c] + 3*index);
        float g = *(colorTable[c] + 3*index+1);
        float b = *(colorTable[c] + 3*index+2);
        float a = *(opacityTable[c] + index);

        if ( a > 0.0 && property->GetShade() )
          {
          float n[3];
          vtkKWERVIC_ComputeGradient( n, ptr, voxel,
                                      c, dim, components, aspect );

          float length = vtkMath::Normalize(n);

          if ( length < 1.0 / tableScale[c] )
            {
            tmpColor[0] += weight[c]*a*((ambient[c])*r);
            tmpColor[1] += weight[c]*a*((ambient[c])*g);
            tmpColor[2] += weight[c]*a*((ambient[c])*b);
            tmpColor[3] += weight[c]*a;
            }
          else
            {
            float shadedColor[3];
            vtkKWERVIC_ShadeSample(rayIncrement, n,
                                   ambient[c], diffuse[c],
                                   specular[c],specularPower[c],
                                   r, g, b, shadedColor );

            tmpColor[0] += weight[c]*a*shadedColor[0];
            tmpColor[1] += weight[c]*a*shadedColor[1];
            tmpColor[2] += weight[c]*a*shadedColor[2];
            tmpColor[3] += weight[c]*a;
            }
          }
        else
          {
          // Add it in (multiplied by weight)
          tmpColor[0] += weight[c] * r * a;
          tmpColor[1] += weight[c] * g * a;
          tmpColor[2] += weight[c] * b * a;
          tmpColor[3] += weight[c] * a;
          }
        }
      }
    else
      {
      // start by getting the opacity from the last component
      // passed through the lookup table
      float val;
      c = components-1;
      vtkKWERVIC_Interpolate( val, ptr, voxel, c, dim, components );
      index = static_cast<int>((val + tableOffset[c]) * tableScale[c] + 0.5);
      index = (index<0)?(0):((index>1023)?(1023):(index));
      float r, g, b, a;

      // avoid compiler warning
      r = g = b = 0.0;

      // initialize alpha
      a = *(opacityTable[0] + index);

      if ( a )
        {
        if ( components == 4 )
          {
          vtkKWERVIC_Interpolate( val, ptr, voxel, 0, dim, components );
          r = val/255.0f;

          vtkKWERVIC_Interpolate( val, ptr, voxel, 1, dim, components );
          g = val/255.0f;

          vtkKWERVIC_Interpolate( val, ptr, voxel, 2, dim, components );
          b = val/255.0f;
          }
        else if ( components == 2 )
          {
          vtkKWERVIC_Interpolate( val, ptr, voxel, 0, dim, components );
          index = static_cast<int>((val + tableOffset[0]) * tableScale[0] + 0.5);
          index = (index<0)?(0):((index>1023)?(1023):(index));

          r = *(colorTable[0] + 3*index);
          g = *(colorTable[0] + 3*index+1);
          b = *(colorTable[0] + 3*index+2);
          }
        else
          {
          // This case should not happen, it is just here to
          // avoid VS warning C4701: "potentially uninitialized local variable
          // 'r'/'g'/'b' used"
          r=0.0f;
          g=0.0f;
          b=0.0f;
          assert("check: impossible case. components is neither 2 nor 4" && 0);
          }

        // unshaded color
        tmpColor[0] = a*r;
        tmpColor[1] = a*g;
        tmpColor[2] = a*b;
        tmpColor[3] = a;

        if ( property->GetShade() )
          {
          float n[3];
          vtkKWERVIC_ComputeGradient( n, ptr, voxel,
                                      components-1, dim,
                                      components, aspect );

          float length = vtkMath::Normalize(n);

          if ( length < 1.0 / tableScale[0] )
            {
            tmpColor[0] = r*ambient[0];
            tmpColor[1] = g*ambient[0];
            tmpColor[2] = b*ambient[0];
            }
          else
            {
            float shadedColor[3];
            vtkKWERVIC_ShadeSample(rayIncrement, n,
                                   ambient[0], diffuse[0],
                                   specular[0],specularPower[0],
                                   r, g, b, shadedColor );

            tmpColor[0] = a*shadedColor[0];
            tmpColor[1] = a*shadedColor[1];
            tmpColor[2] = a*shadedColor[2];
            tmpColor[3] = a;
            }
          }
        }
      }


    accumColor[0] += tmpColor[0]*remainingOpacity;
    accumColor[1] += tmpColor[1]*remainingOpacity;
    accumColor[2] += tmpColor[2]*remainingOpacity;
    remainingOpacity *= (1.0f - tmpColor[3]);

    // Increment to the next voxel
    voxel[0] += rayIncrement[0];
    voxel[1] += rayIncrement[1];
    voxel[2] += rayIncrement[2];

    // Move the pointer to this location
    ptr = dataPtr + components * ( static_cast<int>(voxel[0]) +
                                   static_cast<int>(voxel[1]) * dim[0] +
                                   static_cast<int>(voxel[2]) * dim[0] * dim[1] );
    }


  // Check bounds to make sure everything is between [0,1]
  accumColor[0] = (accumColor[0] < 0)?(0):((accumColor[0]>1)?(1):(accumColor[0]));
  accumColor[1] = (accumColor[1] < 0)?(0):((accumColor[1]>1)?(1):(accumColor[1]));
  accumColor[2] = (accumColor[2] < 0)?(0):((accumColor[2]>1)?(1):(accumColor[2]));

  // Now do the alpha multiplication and convert to unsigned char
  color[0] = static_cast<unsigned char>(accumColor[0]*255.0 + 0.5);
  color[1] = static_cast<unsigned char>(accumColor[1]*255.0 + 0.5);
  color[2] = static_cast<unsigned char>(accumColor[2]*255.0 + 0.5);
}

// ----------------------------------------------------------------------------
// This is the threaded function
// ----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE vtkKWERVICGenerateImage( void *arg )
{
  vtkKWERepresentativeVolumeImageCreator  *creator;
  vtkMutexLock                            *activeFlagLock;
  int                                     *activeFlag;
  vtkMultiThreader::ThreadInfo            *castedArg;

  castedArg = static_cast<vtkMultiThreader::ThreadInfo *>(arg);
  activeFlagLock = castedArg->ActiveFlagLock;
  activeFlag = castedArg->ActiveFlag;

  creator = static_cast<vtkKWERepresentativeVolumeImageCreator *>
    (static_cast<vtkMultiThreader::ThreadInfo *>(arg)->UserData);

  creator->GenerateImage( activeFlag, activeFlagLock );

  return VTK_THREAD_RETURN_VALUE;
}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
vtkKWERepresentativeVolumeImageCreator::vtkKWERepresentativeVolumeImageCreator()
{
  this->Property       = NULL;
  this->BlendMode      = -1;
  this->Input          = NULL;

  this->InternalProperty       = vtkVolumeProperty::New();
  this->InternalBlendMode      = -1;
  this->InternalInput          = NULL;

  this->VisibleSide = vtkKWERepresentativeVolumeImageCreator::MinusYSideView;

  this->Threader = vtkMultiThreader::New();

  this->SpawnedThreadID = -1;

  this->RepresentativeImage = NULL;
  this->RepresentativeImageIsValidLock = vtkMutexLock::New();
  this->RepresentativeImageIsValid = 0;
  this->RepresentativeImageSize[0] = 0;
  this->RepresentativeImageSize[1] = 0;
}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
vtkKWERepresentativeVolumeImageCreator::~vtkKWERepresentativeVolumeImageCreator()
{
  this->Stop();
  this->ClearInternalParameters();
  this->InternalProperty->Delete();
  this->SetProperty(NULL);
  this->SetInput(NULL);
  this->RepresentativeImageIsValidLock->Delete();
  this->Threader->Delete();

  if ( this->RepresentativeImage )
    {
    this->RepresentativeImage->Delete();
    }

}

// ----------------------------------------------------------------------------
// Internal method to clear out copies of parameters kept for processing in the
// spawned thread
// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::ClearInternalParameters()
{
  if ( this->InternalInput )
    {
    this->InternalInput->UnRegister(this);
    }

  this->InternalBlendMode      = -1;
  this->InternalInput          = NULL;
}

// ----------------------------------------------------------------------------
// The Start method will start a process to create the representative image.
// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::Start()
{
  // First check if we are already processing - it is an error to try to
  // process more than one image at a time.
  if ( this->IsProcessing() )
    {
    vtkErrorMacro("Cannot start since an image is currently being processed.");
    return;
    }

  // Clear out the old internal parameters
  this->ClearInternalParameters();

  // Invalidate the last representative image
  this->RepresentativeImageIsValidLock->Lock();
  this->RepresentativeImageIsValid = 0;
  this->RepresentativeImageIsValidLock->Unlock();

  // If the image size is not set, it is an error
  if ( this->RepresentativeImageSize[0] == 0 ||
       this->RepresentativeImageSize[1] == 0 )
    {
    vtkErrorMacro("You must set the size of the representative image before starting.");
    return;
    }


  // If we already have a representative image, release it
  if ( this->RepresentativeImage )
    {
    this->RepresentativeImage->Delete();
    }

  // Now create one of the right size and type
  this->RepresentativeImage = vtkImageData::New();
  this->RepresentativeImage->SetDimensions( this->RepresentativeImageSize[0],
                                            this->RepresentativeImageSize[1],
                                            1 );
  this->RepresentativeImage->SetScalarTypeToUnsignedChar();
  this->RepresentativeImage->SetNumberOfScalarComponents(3);
  this->RepresentativeImage->AllocateScalars();

  // Keep a separate pointer to the input. We are still sharing the
  // input with the main thread - we'll only be reading it, but the main
  // thread can also only read it while we are processing. The main thread
  // can change our input (but cannot make changes to the vtkImageData that
  // was the input at the time we were last started...)
  this->InternalInput = this->Input;
  this->InternalInput->Register(this);

  // For the property, let's make a copy of it. This way we don't
  // have to worry about what the main thread might be doing with
  // it.
  this->InternalProperty->DeepCopy(this->Property);

  this->InternalBlendMode = this->BlendMode;

  // Now spawn the thread
  this->SpawnedThreadID = this->Threader->SpawnThread( vtkKWERVICGenerateImage, this );
}

// ----------------------------------------------------------------------------
// The GenerateImage method is called from the threaded function
// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::GenerateImage(int *flag,
                                                           vtkMutexLock *lock)
{
  float rayIncrement[3];
  this->ComputeRayIncrement(rayIncrement);

  double sampleDistance = this->ComputeSampleDistance(rayIncrement);
  this->UpdateTransferFunctions(sampleDistance);

  unsigned char *ptr =
    static_cast<unsigned char *>(this->RepresentativeImage->GetScalarPointer());

  int i, j;
  for ( j = 0; j < this->RepresentativeImageSize[1]; j++ )
    {
    for ( i = 0; i < this->RepresentativeImageSize[0]; i++ )
      {
      float voxel[3];
      this->ComputeFirstVoxel(i, j, voxel);
      if ( voxel[0] == -1 )
        {
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        }
      else
        {
        unsigned char color[3];
        this->CastRay(voxel, rayIncrement, color);

        ptr[0] = color[0];
        ptr[1] = color[1];
        ptr[2] = color[2];
        }
      ptr+=3;
      }
    }

  // The image is done now
  this->RepresentativeImageIsValidLock->Lock();
  this->RepresentativeImageIsValid = 1;
  this->RepresentativeImageIsValidLock->Unlock();

  lock->Lock();
  *flag = 0;
  lock->Unlock();
}

// ----------------------------------------------------------------------------
// The Stop method will stop the background thread that is processing the image.
// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::Stop()
{
  if ( this->IsProcessing() )
    {
    this->Threader->TerminateThread(this->SpawnedThreadID);
    }
}

// ----------------------------------------------------------------------------
// IsProcessing will return 1 if the background thread is processing an image
// ----------------------------------------------------------------------------
int vtkKWERepresentativeVolumeImageCreator::IsProcessing()
{
  // Can't be processing, we don't have a spawned thread ID
  if ( this->SpawnedThreadID < 0 )
    {
    return 0;
    }

  // Check with the threader
  return this->Threader->IsThreadActive(this->SpawnedThreadID);
}

// ----------------------------------------------------------------------------
// IsValid will return 1 is the last attempt at processing an image was
// successful
// ----------------------------------------------------------------------------
int vtkKWERepresentativeVolumeImageCreator::IsValid()
{
  this->RepresentativeImageIsValidLock->Lock();
  int val = this->RepresentativeImageIsValid;
  this->RepresentativeImageIsValidLock->Unlock();

  return val;
}

// ----------------------------------------------------------------------------
// GetRepresentativeImage will return the representative image as long as
// this class is not currently processing and that image is valid
// ----------------------------------------------------------------------------
vtkImageData *vtkKWERepresentativeVolumeImageCreator::GetRepresentativeImage()
{
  if ( this->IsProcessing() )
    {
    vtkErrorMacro("Can't get image while processing");
    return NULL;
    }

  if ( !this->IsValid() )
    {
    vtkErrorMacro("Image is not valid");
    return NULL;
    }

  return this->RepresentativeImage;
}

// ----------------------------------------------------------------------------
// SetRepresentativeImageSize will set the width / height of the representative
// image. These values must be at least 1 and at most 1024.
// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::SetRepresentativeImageSize(int width, int height)
{
  if ( width < 1 ||
       height < 1 ||
       width > 1024 ||
       height > 1024 )
    {
    vtkErrorMacro("Width and height must be between 1 and 1024!");
    return;
    }

  if ( width != this->RepresentativeImageSize[0]  ||
       height != this->RepresentativeImageSize[1] )
    {
    this->RepresentativeImageSize[0] = width;
    this->RepresentativeImageSize[1] = height;
    this->Modified();
    }
}

// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::ComputeRayIncrement(float rayIncrement[3])
{
  float inc = 0.5;

  switch ( this->VisibleSide )
    {
    case vtkKWERepresentativeVolumeImageCreator::XSideView:
      rayIncrement[0] = -inc;
      rayIncrement[1] = 0.0;
      rayIncrement[2] = 0.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusXSideView:
      rayIncrement[0] = inc;
      rayIncrement[1] = 0.0;
      rayIncrement[2] = 0.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::YSideView:
      rayIncrement[0] = 0.0;
      rayIncrement[1] = -inc;
      rayIncrement[2] = 0.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusYSideView:
      rayIncrement[0] = 0.0;
      rayIncrement[1] = inc;
      rayIncrement[2] = 0.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::ZSideView:
      rayIncrement[0] = 0.0;
      rayIncrement[1] = 0.0;
      rayIncrement[2] = -inc;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusZSideView:
      rayIncrement[0] = 0.0;
      rayIncrement[1] = 0.0;
      rayIncrement[2] = inc;
      break;
    default:
      vtkErrorMacro("Invalid VisibleSide");
      break;
    }
}

// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::ComputeFirstVoxel(int i,
                                                               int j,
                                                               float voxel[3])
{
  int idim[3];
  this->RepresentativeImage->GetDimensions(idim);

  int vdim[3];
  this->InternalInput->GetDimensions(vdim);

  double spacing[3];
  this->InternalInput->GetSpacing(spacing);

  double size[3];
  size[0] = static_cast<double>(vdim[0]-1)*spacing[0];
  size[1] = static_cast<double>(vdim[1]-1)*spacing[1];
  size[2] = static_cast<double>(vdim[2]-1)*spacing[2];


  int imageXAxis   = 0;
  int imageYAxis   = 1;
  double xAxisSign = 1.0;

  switch ( this->VisibleSide )
    {
    case vtkKWERepresentativeVolumeImageCreator::XSideView:
      imageXAxis = 1;
      imageYAxis = 2;
      voxel[0] = static_cast<float>(vdim[0] - 2.0001 - vtkMath::Random());
      xAxisSign = 1.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusXSideView:
      imageXAxis = 1;
      imageYAxis = 2;
      voxel[0] = static_cast<float>(1.0001 + vtkMath::Random());
      xAxisSign = -1.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::YSideView:
      imageXAxis = 0;
      imageYAxis = 2;
      voxel[1] = static_cast<float>(vdim[1] - 2.0001 - vtkMath::Random());
      xAxisSign = -1.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusYSideView:
      imageXAxis = 0;
      imageYAxis = 2;
      voxel[1] = static_cast<float>(1.0001 + vtkMath::Random());
      xAxisSign = 1.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::ZSideView:
      imageXAxis = 0;
      imageYAxis = 1;
      voxel[2] = static_cast<float>(vdim[2] - 2.0001 - vtkMath::Random());
      xAxisSign = 1.0;
      break;
    case vtkKWERepresentativeVolumeImageCreator::MinusZSideView:
      imageXAxis = 0;
      imageYAxis = 1;
      voxel[2] = static_cast<float>(1.0001 + vtkMath::Random());
      xAxisSign = -1.0;
      break;
    default:
      vtkErrorMacro("Invalid VisibleSide");
      break;
    }

  double imageScaleX = static_cast<double>(idim[0]-1)/(size[imageXAxis]);
  double imageScaleY = static_cast<double>(idim[1]-1)/(size[imageYAxis]);

  double scaleFactor;
  double length;
  if ( imageScaleX < imageScaleY )
    {
    scaleFactor = imageScaleX;
    length = idim[0]-1;
    }
  else
    {
    scaleFactor = imageScaleY;
    length = idim[1]-1;
    }



  // convert position into normalized location centered on image center
  double p1 =
    (imageScaleX/scaleFactor)* (static_cast<double>(i) -
                                static_cast<double>(idim[0]-1)/2.0)/length;

  // flip p1 because X axis runs from right to left
  p1 *= xAxisSign;

  double p2  =
    (imageScaleY/scaleFactor)*(static_cast<double>(j) -
                               static_cast<double>(idim[1]-1)/2.0)/length;

  // convert to voxel location
  voxel[imageXAxis] =  static_cast<float>(p1 * static_cast<double>(vdim[imageXAxis]-5.0) +
                                          static_cast<double>(vdim[imageXAxis]-1)/2.0);
  voxel[imageYAxis] =  static_cast<float>(p2 * static_cast<double>(vdim[imageYAxis]-5.0) +
                                          static_cast<double>(vdim[imageYAxis]-1)/2.0);

  if ( ! (voxel[0] > 1.0 &&
          voxel[1] > 1.0 &&
          voxel[2] > 1.0 &&
          voxel[0] < (vdim[0]-2) &&
          voxel[1] < (vdim[1]-2) &&
          voxel[2] < (vdim[2]-2)) )
    {
    // Ray does not intersect volume - happens due to aspect ratio because
    // we make the whole volume fit in all directions.
    voxel[0] = -1;
    }

}

// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::UpdateTransferFunctions(double sampleDistance)
{
  int numComponents = this->InternalInput->GetNumberOfScalarComponents();
  int independent =  this->InternalProperty->GetIndependentComponents();

  int i;
  for (i = 0; i < numComponents; i++ )
    {
    double range[2];
    this->InternalInput->GetPointData()->GetScalars()->GetRange(range, i);

    this->TableOffset[i] = static_cast<float>(-range[0]);
    this->TableScale[i] = static_cast<float>(1023.0 / (range[1]-range[0]));

    // If we have independent components, we'll get a color and opacity table for each
    // component and store it in that component's index (i). Otherwise, we have
    // "dependent" components (LA, or RGBA) and we have at most one color table and
    // exactly one opacity table, and we'll store those in the 0th entry in our
    // ColorTable / OpacityTable.
    int tableIdx = (independent==0)?(0):(i);

    if ( independent || ( i == 0 && numComponents == 2) )
      {
      if ( this->InternalProperty->GetColorChannels(tableIdx) == 1 )
        {
        vtkPiecewiseFunction *gf = this->InternalProperty->GetGrayTransferFunction(tableIdx);
        float tmp[1024];
        gf->GetTable( range[0], range[1], 1024, tmp );
        for ( int index = 0; index < 1024; index++ )
          {
          this->ColorTable[tableIdx][index*3  ] = tmp[index];
          this->ColorTable[tableIdx][index*3+1] = tmp[index];
          this->ColorTable[tableIdx][index*3+2] = tmp[index];
          }
        }
      else
        {
        vtkColorTransferFunction *cf = this->InternalProperty->GetRGBTransferFunction(tableIdx);
        cf->GetTable( range[0], range[1], 1024, this->ColorTable[tableIdx] );
        }
      }

    if ( independent || ( i == numComponents-1) )
      {
      this->InternalProperty->GetScalarOpacity(tableIdx)->GetTable( range[0], range[1],
                                                               1024,
                                                               this->OpacityTable[tableIdx] );

      // If the blend mode is composite, then we'll need to adjust the opacity
      // function for the sample distance
      if ( this->InternalBlendMode == vtkVolumeMapper::COMPOSITE_BLEND )
        {
        int idx;
        for ( idx = 0; idx < 1023; idx++ )
          {
          if ( this->OpacityTable[tableIdx][idx] > 0.0001 )
            {
            this->OpacityTable[tableIdx][idx] = static_cast<float>(
              1.0-pow(static_cast<double>((1.0 - this->OpacityTable[tableIdx][idx])),
                      sampleDistance));
            }
          }
        }
      }
    }


}

// ----------------------------------------------------------------------------
double vtkKWERepresentativeVolumeImageCreator::ComputeSampleDistance(float rayIncrement[3])
{
  double spacing[3];
  this->InternalInput->GetSpacing(spacing);

  switch ( this->VisibleSide )
    {
    case vtkKWERepresentativeVolumeImageCreator::XSideView:
    case vtkKWERepresentativeVolumeImageCreator::MinusXSideView:
      return spacing[0]*fabs(rayIncrement[0]);
      break;
    case vtkKWERepresentativeVolumeImageCreator::YSideView:
    case vtkKWERepresentativeVolumeImageCreator::MinusYSideView:
      return spacing[1]*fabs(rayIncrement[1]);
      break;
    case vtkKWERepresentativeVolumeImageCreator::ZSideView:
    case vtkKWERepresentativeVolumeImageCreator::MinusZSideView:
      return spacing[2]*fabs(rayIncrement[2]);
      break;
    default:
      vtkErrorMacro("Invalid VisibleSide");
      break;
    }

  return 0.0;

}


// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::CastRay(float voxel[3],
                                                     float rayIncrement[3],
                                                     unsigned char color[3])
{
  int scalarType = this->InternalInput->GetScalarType();
  void *dataPtr = this->InternalInput->GetScalarPointer();

  vtkKWERVICFriend *myFriend = new vtkKWERVICFriend;
  myFriend->Creator = this;

  switch ( this->InternalBlendMode )
    {
    case vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND:
      switch ( scalarType )
        {
        vtkTemplateMacro( vtkKWERVIC_MIP(static_cast<VTK_TT *>(dataPtr),
                                          voxel,
                                          rayIncrement,
                                          color,
                                          myFriend ) );
        }
      break;

    case vtkVolumeMapper::COMPOSITE_BLEND:
      switch ( scalarType )
        {
        vtkTemplateMacro( vtkKWERVIC_Composite(static_cast<VTK_TT *>(dataPtr),
                                                voxel,
                                                rayIncrement,
                                                color,
                                                myFriend ) );
        }
      break;

    default:
      color[0] = 255;
      color[1] = 0;
      color[2] = 0;
      break;
    }

  delete myFriend;
}

// ----------------------------------------------------------------------------
void vtkKWERepresentativeVolumeImageCreator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

