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

//-----------------------------------------------------------------------------
// .NAME vtkKWERepresentativeVolumeImageCreator - Create a representative
// image given a volume and properties
// .SECTION Description
// vtkKWEResprentativeVolumeImageCreator is a class that will generate
// reprentative images of a volume given some properties such as the
// color and opacity transfer functions (contained in a vtkVolumeProperty), the
// blending mode, and the input volume. It will create a representative image
// of the requested size in a background thread. It is assumed that the Stop()
// method will be called by the calling process before any of the input
// parameters are modified. The IsComplete() method will return 0 if the
// image is still being generated or if an error occurred, and 1 if the image
// generation process is complete. The IsValid() method will return 1 if the
// genterated image is valid, or 0 if an error occurred during the computation
// (e.g. due to missing input parameters).
//
// .SECTION Limitations
//  - Works only on the scalars in the point data of the input
//  - Does not do gradient opacity
//-----------------------------------------------------------------------------

#ifndef __vtkKWERepresentativeVolumeImageCreator_h
#define __vtkKWERepresentativeVolumeImageCreator_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

#include "vtkVolumeMapper.h" // for definition of blend modes

class vtkImageData;
class vtkMultiThreader;
class vtkMutexLock;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkVolumeProperty;


// Forward declaration for friend
//BTX
VTK_THREAD_RETURN_TYPE vtkKWERVICGenerateImage(void *arg);
class vtkKWERVICFriend;
//ETX

class VTKEdge_VOLUMERENDERING_EXPORT vtkKWERepresentativeVolumeImageCreator : public vtkObject
{
public:
  static vtkKWERepresentativeVolumeImageCreator *New();
  vtkTypeRevisionMacro(vtkKWERepresentativeVolumeImageCreator,vtkObject);
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Set the vtkVolumeProperty. This will be used for the various transfer
  // function, and the component information.
  void SetProperty(vtkVolumeProperty *p);

  // Description:
  // Get the current property
  vtkGetObjectMacro( Property, vtkVolumeProperty );

  // Description:
  // Set/Get the blend mode.
  vtkSetMacro( BlendMode, int );
  void SetBlendModeToComposite()
    { this->SetBlendMode( vtkVolumeMapper::COMPOSITE_BLEND ); }
  void SetBlendModeToMaximumIntensity()
    { this->SetBlendMode( vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND ); }
  void SetBlendModeToMinimumIntensity()
    { this->SetBlendMode( vtkVolumeMapper::MINIMUM_INTENSITY_BLEND ); }
  vtkGetMacro( BlendMode, int );

  // Description:
  // Set the input volume
  void SetInput(vtkImageData *volume);

  // Description:
  // Get the input volume
  vtkGetObjectMacro( Input, vtkImageData );

  // Description:
  // Start generating the image. This is a non-blocking operation -
  // the image will be generated in a background thread. Use
  // IsProcessing() to poll whether it is done, or register for
  // the End event.
  void Start();

  // Description:
  // Stop generating the image.
  void Stop();

  // Description:
  // Is this class still working on generating the image? A return value
  // of 1 means that the class is still actively processing the image.
  // Otherwise the return value will be 0.
  int IsProcessing();

  // Description:
  // Is the generated image valid? This will return 0 if IsProcessing()
  // returns 1. It will also return 0 if an error occurred during
  // the last image creation attempt, or if no image has been created.
  // Otherwise it will return 1.
  int IsValid();

  // Description:
  // Set the size of the representative image. The default value is
  // (0,0) which is invalid, so this must be set before calling Start().
  // The width / height must be at least 1 and at most 1024.
  void SetRepresentativeImageSize( int w, int h );
  void SetRepresentativeImageSize( int size[2] )
    {this->SetRepresentativeImageSize(size[0], size[1]);}

  // Description:
  // Get the size of the representative image. A value of (0,0) means
  // the size was never set.
  vtkGetVectorMacro( RepresentativeImageSize, int, 3 );


  // Description:
  // Get the representative image. Should only be called after
  // Start() has been called, and IsProcessing() is 0, and
  // IsValid() is 1. This will create a new vtkImageData that
  // the caller should immediately register since this class
  // will release its hold on it when Start() is called again.
  vtkImageData *GetRepresentativeImage();

  // Description:
  // Select the side to view. An XSideView implied that the +X size
  // of the data is visible - the viewing direction is (-1, 0, 0).
  // The two sets of enumerated types are equivalent views using
  // different terminology (scientific X,Y,Z right handed coordinate
  // system vs. medical R, A, S left-handed coordinate system)
  vtkSetClampMacro( VisibleSide, int,
                    vtkKWERepresentativeVolumeImageCreator::XSideView,
                    vtkKWERepresentativeVolumeImageCreator::MinusZSideView );

  // Description:
  // Get the visible side of the volume. See SetVisibleSide for further
  // clarification.
  vtkGetMacro( VisibleSide, int );

//BTX
  enum
  {
    XSideView=0,
    MinusXSideView,
    YSideView,
    MinusYSideView,
    ZSideView,
    MinusZSideView
  };

  enum
  {
    LeftSideView=0,
    RightSideView,
    PosteriorSideView,
    AnteriorSideView,
    SuperiorSideView,
    InferiorSideView
  };
//ETX

protected:
  vtkKWERepresentativeVolumeImageCreator();
  ~vtkKWERepresentativeVolumeImageCreator();

  friend VTK_THREAD_RETURN_TYPE vtkKWERVICGenerateImage(void *arg);
  friend class vtkKWERVICFriend;

  void GenerateImage( int *flag,
                      vtkMutexLock *lock );

  void ClearInternalParameters();

  vtkVolumeProperty  *Property;
  int                 BlendMode;
  vtkImageData       *Input;

  vtkVolumeProperty  *InternalProperty;
  int                 InternalBlendMode;
  vtkImageData       *InternalInput;

  vtkImageData       *RepresentativeImage;
  vtkMutexLock       *RepresentativeImageIsValidLock;
  int                 RepresentativeImageIsValid;
  int                 RepresentativeImageSize[2];

  int                 VisibleSide;

  vtkMultiThreader   *Threader;

  int                 SpawnedThreadID;

  void ComputeFirstVoxel( int i, int j, float voxel[3] );
  void ComputeRayIncrement( float increment[3] );
  void CastRay( float voxel[3],
                float increment[3],
                unsigned char color[3] );

  void UpdateTransferFunctions(double sampleDistance);
  double ComputeSampleDistance(float rayIncrement[3]);

  float TableOffset[4];
  float TableScale[4];
  float ColorTable[4][1024*3];
  float OpacityTable[4][1024];
  int TableComponents;
  int TableIndependentComponents;

private:
  vtkKWERepresentativeVolumeImageCreator(const vtkKWERepresentativeVolumeImageCreator&);  // Not implemented.
  void operator=(const vtkKWERepresentativeVolumeImageCreator&);  // Not implemented.
};

#endif
