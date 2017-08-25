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

#include "vtkKWEVolumeMapper.h"

#include "vtkObjectFactory.h"

#include "vtkColorTransferFunction.h"
#include "vtkDataArray.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkKWEEventForwarderCommand.h"
#include "vtkKWEGPUVolumeRayCastMapper.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeTextureMapper3D.h"


vtkStandardNewMacro( vtkKWEVolumeMapper );
vtkCxxRevisionMacro(vtkKWEVolumeMapper, "$Revision: 715 $");

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
vtkKWEVolumeMapper::vtkKWEVolumeMapper()
{
  // Default for Window / Level - no adjustment
  this->FinalColorWindow  = 1.0;
  this->FinalColorLevel   = 0.5;
  
  // Our render mode is undefined at this point
  this->CurrentRenderMode = vtkKWEVolumeMapper::UndefinedRenderMode;
  
  // Nothing is initialized and we assume nothing is supported
  this->Initialized        = 0;
  this->TextureSupported   = 0;
  this->GPUSupported       = 0;
  this->RayCastSupported   = 0;
  this->LowResGPUNecessary = 0;

  // Create all the mappers we might need
  this->RayCastMapper   = vtkFixedPointVolumeRayCastMapper::New();
  this->GPUMapper       = vtkKWEGPUVolumeRayCastMapper::New();
  this->TextureMapper   = vtkVolumeTextureMapper3D::New();
  this->GPULowResMapper = vtkKWEGPUVolumeRayCastMapper::New();

  // If the render window has a desired update rate of at least 1 frame
  // per second or more, we'll consider this interactive
  this->InteractiveUpdateRate = 0.00001;

  // This is the resample filter that may be used if we need 
  // a lower resolution version of the input for GPU rendering
  this->GPUResampleFilter = vtkImageResample::New();

  // Turn this on by default - this means that the sample spacing will be
  // automatically computed from the spacing of the input data. This is
  // also true for the GPU ray cast mapper. 
  this->RayCastMapper->LockSampleDistanceToInputSpacingOn();

  // Default to the default mode - which will use the best option that
  // is supported by the hardware
  this->RequestedRenderMode = vtkKWEVolumeMapper::DefaultRenderMode;

  // Keep track of what blend mode we had when we initialized and
  // checked for hardware support - we need to recheck if the blend
  // mode changes
  this->InitializedBlendMode = -1;
  
  // Create the forwarding command
  vtkKWEEventForwarderCommand *cb = vtkKWEEventForwarderCommand::New();
  cb->SetForwardingClass(this);
  
  // Now forward the ray caster's events
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperRenderStartEvent, cb);
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperRenderEndEvent, cb);
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperRenderProgressEvent, cb);
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsStartEvent, cb);
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsEndEvent, cb);
  this->RayCastMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsProgressEvent, cb);
  
  // And the texture mapper's events
  this->TextureMapper->AddObserver(vtkCommand::StartEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::EndEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::ProgressEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperRenderStartEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperRenderEndEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperRenderProgressEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsStartEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsEndEvent, cb);
  this->TextureMapper->AddObserver(vtkCommand::VolumeMapperComputeGradientsProgressEvent, cb);
  
  // And the GPU mapper's events
  // Commented out because too many events are being forwwarded 
  // put back in after that is fixed
  /***
  this->GPUMapper->AddObserver(vtkCommand::VolumeMapperRenderStartEvent, cb);
  this->GPUMapper->AddObserver(vtkCommand::VolumeMapperRenderEndEvent, cb);
  this->GPUMapper->AddObserver(vtkCommand::VolumeMapperRenderProgressEvent, cb);
  ***/
  
  // And the low res GPU mapper's events
  // Commented out because too many events are being forwwarded 
  // put back in after that is fixed
  /***
  this->GPULowResMapper->AddObserver(vtkCommand::VolumeMapperRenderStartEvent, cb);
  this->GPULowResMapper->AddObserver(vtkCommand::VolumeMapperRenderEndEvent, cb);
  this->GPULowResMapper->AddObserver(vtkCommand::VolumeMapperRenderProgressEvent, cb);
  ***/
  
  cb->Delete();
}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
vtkKWEVolumeMapper::~vtkKWEVolumeMapper()
{
  this->RayCastMapper->Delete();
  this->GPUMapper->Delete();
  this->GPULowResMapper->Delete();
  this->TextureMapper->Delete();
  this->GPUResampleFilter->Delete();
}


// ----------------------------------------------------------------------------
// The Render method will determine the render mode and then render using the
// appropriate mapper. If the render mode is invalid (the user explicitly 
// chooses something that is not supported) the render will silently fail.
// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::Render( vtkRenderer *ren, vtkVolume *vol )
{
  // Compute the render mode based on the requested
  // render mode, available hardware, and render window's
  // desired update rate
  this->ComputeRenderMode(ren->GetRenderWindow(), vol);
 
  switch ( this->CurrentRenderMode )
    {
    case vtkKWEVolumeMapper::RayCastRenderMode:
      this->RayCastMapper->Render(ren,vol);
      break;
    case vtkKWEVolumeMapper::TextureRenderMode:
      this->TextureMapper->Render(ren,vol);
      break;
    case vtkKWEVolumeMapper::GPURenderMode:
      if ( ren->GetRenderWindow()->GetDesiredUpdateRate() >= 
           this->InteractiveUpdateRate &&
           this->LowResGPUNecessary )
        {
        this->GPULowResMapper->Render(ren, vol);
        }
      else
        {
        if ( ren->GetRenderWindow()->GetDesiredUpdateRate() >= 
             this->InteractiveUpdateRate )
          {
          this->GPUMapper->AutoAdjustSampleDistancesOn();
          }
        else
          {
          this->GPUMapper->AutoAdjustSampleDistancesOff();
          }
        this->GPUMapper->Render(ren,vol);
        }
      break;
    case vtkKWEVolumeMapper::InvalidRenderMode:
      // Silently fail - a render mode that is not
      // valid was selected so we will render nothing
      break;
    default:
      vtkErrorMacro("Internal Error!");
      break;
    }
}


// ----------------------------------------------------------------------------
// Initialize the rende
// We need to determine whether the texture mapper or GPU mapper are supported
// First we need to know what input scalar field we are working with to find
// out how many components it has. If it has more than one, and we are considering
// them to be independent components, then we know that neither the texture mapper
// nor the GPU mapper will work.
// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::Initialize(vtkRenderWindow *win, vtkVolume *vol)
{
 
  if ( !this->GetInput() )
    {
    this->Initialized = 0;
    return;
    }

  int usingCellColors=0;
  vtkDataArray *scalars  = this->GetScalars(this->GetInput(), this->ScalarMode,
                                            this->ArrayAccessMode,
                                            this->ArrayId, this->ArrayName,
                                            usingCellColors);
 
  if ( scalars->GetNumberOfComponents() != 1 )
    {
    if ( vol->GetProperty()->GetIndependentComponents() )
      {
      this->TextureSupported = 0;
      this->GPUSupported     = 0;
      if ( usingCellColors )
        {
        this->RayCastSupported = 0;
        }
      else
        {
        this->RayCastSupported = 1;
        }
      this->Initialized      = 1;
      this->SupportStatusCheckTime.Modified();
      return;
      }
    }
  
  if ( usingCellColors )
    {
    this->RayCastSupported = 0;
    }
  else
    {
    this->RayCastSupported = 1;
    }
  
  // Make the window current because we need the OpenGL context
  win->MakeCurrent();
  
  // Have to give the texture mapper its input or else it won't report that
  // it is supported. Texture mapper only supported for composite blend
  if ( this->GetBlendMode() !=  vtkVolumeMapper::COMPOSITE_BLEND )
    {
    this->TextureSupported = 0;
    }
  else
    {
    this->TextureMapper->SetInputConnection(this->GetInputConnection(0,0));
    this->TextureSupported = this->TextureMapper->IsRenderSupported(vol->GetProperty());    
    }
  
  this->GPUSupported = this->GPUMapper->IsRenderSupported(win,vol->GetProperty());
  this->Initialized = 1;
  this->InitializedBlendMode = this->GetBlendMode();
  this->SupportStatusCheckTime.Modified();
}

// ----------------------------------------------------------------------------
// Compute the render mode based on what hardware is available, what the user
// requested as a render mode, and the desired update rate of the render window
// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::ComputeRenderMode(vtkRenderWindow *win, vtkVolume *vol)
{
  // If we are already initialized, and the volume,
  // volume's input, and volume's property have not
  // changed since last time we computed the render mode,
  // then we don't need to initialize again
  if (!( this->Initialized && 
         this->SupportStatusCheckTime.GetMTime() > this->GetMTime() &&
         this->SupportStatusCheckTime.GetMTime() > vol->GetProperty()->GetMTime() &&
         this->SupportStatusCheckTime.GetMTime() > this->GetInput()->GetMTime() &&
         this->InitializedBlendMode == this->GetBlendMode() ) )
    {
    this->Initialize(win,vol);
    }

  
  // Use this as the initial state to simplify the code below
  this->CurrentRenderMode = vtkKWEVolumeMapper::InvalidRenderMode;
  
  if ( !this->GetInput() )
    {
    return;
    }
  
  double scale[3];  
  double spacing[3];      
  this->GetInput()->GetSpacing(spacing);
  
  switch ( this->RequestedRenderMode )
    {
    // Requested ray casting - OK as long as it is supported
    // This ray caster is a software mapper so it is supported as
    // we aren't attempting to render cell scalars
    case vtkKWEVolumeMapper::RayCastRenderMode:
      if ( this->RayCastSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::RayCastRenderMode;
        }
      break;      
      
    // Requested 3D texture - OK as long as it is supported
    case vtkKWEVolumeMapper::TextureRenderMode:
      if ( this->TextureSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::TextureRenderMode;
        }
      
    // Requested GPU - OK as long as it is supported
    case vtkKWEVolumeMapper::GPURenderMode:
      if ( this->GPUSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::GPURenderMode;
        }
      break;
      
      // Requested default mode - select GPU if supported, otherwise
      // select texture mapping for interactive rendering (if supported)
      // and ray casting for still rendering. Make determination of
      // still vs. interactive based on whether the desired update rate
      // is at or above this->InteractiveUpdateRate
    case vtkKWEVolumeMapper::DefaultRenderMode:
      // Go with GPU rendering if it is supported
      if ( this->GPUSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::GPURenderMode;
        }
      // If this is interactive, try for texture mapping
      else if ( win->GetDesiredUpdateRate() >= this->InteractiveUpdateRate &&
                this->TextureSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::TextureRenderMode;
        }
      else if ( this->RayCastSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::RayCastRenderMode;
        }
      break;
      
      // Requested the texture mapping / ray cast combo. If texture 
      // mapping is supported and this is an interactive render, then
      // use it. Otherwise use ray casting.
    case vtkKWEVolumeMapper::RayCastAndTextureRenderMode:
      if ( win->GetDesiredUpdateRate() >= this->InteractiveUpdateRate &&
           this->TextureSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::TextureRenderMode;
        }
      else if ( this->RayCastSupported )
        {
        this->CurrentRenderMode = vtkKWEVolumeMapper::RayCastRenderMode;
        }
      break;
      
      // This should never happen since the SetRequestedRenderMode
      // protects against invalid states
    default:
      vtkErrorMacro("Internal Error: Invalid RequestedRenderMode");
      break;
    }
  
  switch ( this->CurrentRenderMode )
    {
    // We are rendering with the vtkFixedPointVolumeRayCastMapper
    case vtkKWEVolumeMapper::RayCastRenderMode:
      this->RayCastMapper->SetInputConnection(this->GetInputConnection(0,0));
      this->RayCastMapper->SetClippingPlanes(this->GetClippingPlanes());
      this->RayCastMapper->SetCropping(this->GetCropping());
      this->RayCastMapper->SetCroppingRegionPlanes(this->GetCroppingRegionPlanes());    
      this->RayCastMapper->SetBlendMode( this->GetBlendMode() );
      break;
      
      // We are rendering with the vtkVolumeTextureMapper3D
    case vtkKWEVolumeMapper::TextureRenderMode:
      this->TextureMapper->SetInputConnection(this->GetInputConnection(0,0));
      if ( this->RequestedRenderMode == vtkKWEVolumeMapper::DefaultRenderMode ||
           this->RequestedRenderMode == vtkKWEVolumeMapper::RayCastAndTextureRenderMode )
        {          
        this->TextureMapper->SetSampleDistance( static_cast<float>((spacing[0] + spacing[1] + spacing[2] ) / 2.0) );
        }
      else
        {
        this->TextureMapper->SetSampleDistance( static_cast<float>((spacing[0] + spacing[1] + spacing[2] ) / 6.0) );
        }      
      this->TextureMapper->SetClippingPlanes(this->GetClippingPlanes());
      this->TextureMapper->SetCropping(this->GetCropping());
      this->TextureMapper->SetCroppingRegionPlanes(this->GetCroppingRegionPlanes());
      break;
      
      // We are rendering with the vtkKWEGPUVolumeRayCastMapper
    case vtkKWEVolumeMapper::GPURenderMode:
      this->GPUMapper->SetSampleDistance( static_cast<float>((spacing[0] + spacing[1] + spacing[2] ) / 6.0) );
  
      this->GPUMapper->SetInputConnection(this->GetInputConnection(0,0));
      this->GPUMapper->SetClippingPlanes(this->GetClippingPlanes());
      this->GPUMapper->SetCropping(this->GetCropping());
      this->GPUMapper->SetCroppingRegionPlanes(this->GetCroppingRegionPlanes());
      this->GPUMapper->SetBlendMode( this->GetBlendMode() );

      // Make the window current because we need the OpenGL context
      win->MakeCurrent();
  
      // Now we need to find out if we need to use a low resolution
      // version of the mapper for interactive rendering. This is true 
      // if the GPU mapper cannot hand the size of the volume.
      this->GPUMapper->GetReductionRatio(scale);

      // if any of the scale factors is not 1.0, then we do need
      // to use the low res mapper for interactive rendering
      if ( scale[0] != 1.0 || scale[1] != 1.0 || scale[2] != 1.0 )
        {
        this->LowResGPUNecessary = 1;
        
        this->GPUResampleFilter->SetInputConnection(this->GetInputConnection(0,0));
        this->GPUResampleFilter->SetAxisMagnificationFactor( 0, scale[0]/2.0 );
        this->GPUResampleFilter->SetAxisMagnificationFactor( 1, scale[1]/2.0 );
        this->GPUResampleFilter->SetAxisMagnificationFactor( 2, scale[2]/2.0 );

        this->GPULowResMapper->SetInputConnection(this->GPUResampleFilter->GetOutputPort());
        this->GPULowResMapper->SetClippingPlanes(this->GetClippingPlanes());
        this->GPULowResMapper->SetCropping(this->GetCropping());
        this->GPULowResMapper->SetCroppingRegionPlanes(this->GetCroppingRegionPlanes());
        this->GPULowResMapper->SetBlendMode( this->GetBlendMode() );
        }
      else
        {
        this->LowResGPUNecessary = 0;
        }

      break;
      
      // The user selected a RequestedRenderMode that is
      // not supported. In this case the mapper will just
      // silently fail.
    case vtkKWEVolumeMapper::InvalidRenderMode:
      break;
      
      // This should never happen since we don't set the CurrentRenderMode
      // to anything other than the above handled options
    default:
      vtkErrorMacro("Internal Error: Invalid CurrentRenderMode");
      break;
    }
  
}

// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::SetRequestedRenderMode(int mode)
{
  // If we aren't actually changing it, just return
  if ( this->RequestedRenderMode == mode )
    {
    return;
    }
  
  // Make sure it is a valid mode
  if ( mode < vtkKWEVolumeMapper::DefaultRenderMode ||
       mode > vtkKWEVolumeMapper::GPURenderMode )
    {
    vtkErrorMacro("Invalid Render Mode.");
    return;
    }
  
  this->RequestedRenderMode = mode;
  this->Modified();
  
}

// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::ReleaseGraphicsResources(vtkWindow *w)
{
  this->RayCastMapper->ReleaseGraphicsResources(w);
  this->TextureMapper->ReleaseGraphicsResources(w);
  this->GPUMapper->ReleaseGraphicsResources(w);  
  this->Initialized      = 0;
  this->TextureSupported = 0;
  this->GPUSupported     = 0;
  this->RayCastSupported = 0;
}


// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::CreateCanonicalView(
  vtkRenderer *ren,
  vtkVolume *volume,
  vtkVolume *volume2,
  vtkImageData *image,
  int blend_mode,
  double viewDirection[3],
  double viewUp[3])
{
  this->ComputeRenderMode(ren->GetRenderWindow(), volume);

  if ( this->CurrentRenderMode == vtkKWEVolumeMapper::GPURenderMode )
    {
    vtkVolumeProperty *savedProperty = volume->GetProperty();
    volume->SetProperty(volume2->GetProperty());
    volume->GetProperty()->Modified();
    volume->GetProperty()->GetScalarOpacity()->Modified();
    volume->GetProperty()->GetRGBTransferFunction()->Modified();
    this->GPUMapper->CreateCanonicalView(ren, volume,
                                         image, blend_mode,
                                         viewDirection, viewUp);    
    volume->SetProperty(savedProperty);
    volume->GetProperty()->Modified();
    volume->GetProperty()->GetScalarOpacity()->Modified();
    volume->GetProperty()->GetRGBTransferFunction()->Modified();
    }
  else if ( this->RayCastSupported )
    {
    this->RayCastMapper->CreateCanonicalView(volume2,
                                             image, blend_mode,
                                             viewDirection, viewUp);
    }
  else
    {
    vtkErrorMacro("Could not create image - no available mapper");
    }
}

// ----------------------------------------------------------------------------
void vtkKWEVolumeMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
