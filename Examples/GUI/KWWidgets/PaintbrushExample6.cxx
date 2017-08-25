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
// PaintbrushData drawn can be filtered through "operations". These are
// represented by abstract base classes: vtkKWEPaintbrushOperation. The base 
// class acts as a pass through filter. Concrete subclasses can do fancy 
// filtering operations such as flood-fill etc. These operations permit an
// inplace filtering of a paintbrush data during a sketch.
// 
// As an example, a flood-fill operation is implemented in 
// vtkKWEITKConfidenceConnectedPaintbrushOperation. This uses a combination of ITK 
// region growing and hole-filling filters. This example illustrates how to 
// plug an operation into the paintbrush widget.

#include "vtkKWMyWindow.h"

#include "vtkKWEPaintbrushWidget.h"
#include "vtkKWEPaintbrushRepresentation2D.h"
#include "vtkKWEPaintbrushShape.h"
#include "vtkKWEITKImageToStencilFilter.h"
#include "vtkKWEITKPaintbrushOperation.h"
#include "itkBinaryThresholdImageFilter.h"
#include "vtkImageStencilData.h"
#include "vtkKWEPaintbrushStroke.h"
#include "vtkKWEPaintbrushStencilData.h"
#include "vtkKWEPaintbrushGrayscaleData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExtractComponents.h"
#include "vtkExtractVOI.h"
#include "vtkKWEPaintbrushUtilities.h"
#include "vtkKWEWidgetGroup.h"
#include "itkNumericTraits.h"
#include "vtkKWEITKFilterModuleStencilOutput.h"

KWLIBS_CREATE_EXAMPLE // Macro fires up a basic app with some data

namespace vtkitk
{
template <class InputPixelType>
class vtkKWEITKBinaryThresholdImageFilter : public vtkKWEITKImageToStencilFilter
{
public:
  typedef vtkKWEITKBinaryThresholdImageFilter         Self;
  typedef vtkKWEITKImageToStencilFilter                   Superclass;
  typedef itk::SmartPointer< Self >                    Pointer;
  typedef itk::SmartPointer< const Self >              ConstPointer;
  itkTypeMacro(vtkKWEITKBinaryThresholdImageFilter, vtkKWEITKImageToStencilFilter);
  itkNewMacro(Self);
  
  typedef  itk::Image< InputPixelType, Dimension >     InputImageType; 
  typedef  itk::BinaryThresholdImageFilter< 
    InputImageType, OutputImageType >                  FilterType;
  
  // No MTime checks etc.. will update every time.. 
  virtual void Update()
    {
    this->Superclass::Update();

    FilterModuleStencilOutput< FilterType > * f = (dynamic_cast< 
       FilterModuleStencilOutput< FilterType > * >(this->m_Filter));    

    // Update the filter.. If any timing analysis needs to be done.. 
    // This method needs to be interactive enough.
    f->Update();
    }

protected:
  vtkKWEITKBinaryThresholdImageFilter()
    { 
    FilterModuleStencilOutput< FilterType > * filter = new FilterModuleStencilOutput< FilterType >();
    this->m_Filter = filter;
    filter->GetFilter()->SetLowerThreshold( 200 );
    filter->GetFilter()->SetUpperThreshold( 255 );
    }

  ~vtkKWEITKBinaryThresholdImageFilter() { delete this->m_Filter; }
};
} // end namespace vtkitk


class vtkKWEITKBinaryThresholdPaintbrushOperation : public vtkKWEITKPaintbrushOperation
{
public:
  static vtkKWEITKBinaryThresholdPaintbrushOperation *New();
  vtkTypeRevisionMacro(vtkKWEITKBinaryThresholdPaintbrushOperation, 
                       vtkKWEITKPaintbrushOperation);

protected:
  vtkKWEITKBinaryThresholdPaintbrushOperation() {};
  ~vtkKWEITKBinaryThresholdPaintbrushOperation() {};

  // See superclass documentation for details on these methods.
  virtual void DoOperationOnStencil(vtkImageStencilData *, double p[3]);
  virtual void DoOperation( vtkKWEPaintbrushData *data, double p[3] )
    {
    // The paintbrush data can be binary or grayscale. Invoke the appropriate
    // filtering operation in each case.

    if (vtkKWEPaintbrushStencilData *sdata = 
      vtkKWEPaintbrushStencilData::SafeDownCast(data))
      {
      // Convert from world coords to pixel index.
      
      double spacing[3], origin[3];
      this->ImageData->GetSpacing(spacing);
      this->ImageData->GetOrigin(origin);
      this->DoOperationOnStencil( sdata->GetImageStencilData(), p );
      }
    }

private:
  vtkKWEITKBinaryThresholdPaintbrushOperation(
    const vtkKWEITKBinaryThresholdPaintbrushOperation&);  //Not implemented
  void operator=(const 
      vtkKWEITKBinaryThresholdPaintbrushOperation&);  //Not implemented
};

template< class T > int 
MyPaintbrushRunner( vtkKWEITKBinaryThresholdPaintbrushOperation * self, 
                  double center[3], 
                  vtkImageStencilData *stencil,
                  T )
{
  self->InternalFilter = NULL;

  typedef vtkitk::vtkKWEITKBinaryThresholdImageFilter< T > PaintbrushFilterType;
  typename PaintbrushFilterType::Pointer filter = PaintbrushFilterType::New();
  self->InternalFilter = filter;

  // Set the update region to a certain region on either side of the center
  int extent[6], imageExtent[6];
  double spacing[3], origin[3];
  self->GetImageData()->GetSpacing(spacing);
  self->GetImageData()->GetExtent(imageExtent);

  double *width = self->GetPaintbrushShape()->GetWidth();

  // Set the center and radius to mask out a spherical stencil, instead of 
  // one with rectangular jagged edges.
  self->GetImageData()->GetOrigin(origin);

  extent[0] = (int)((center[0] - width[0])/spacing[0] + 0.5);
  extent[1] = (int)((center[0] + width[0])/spacing[0] + 0.5);
  extent[2] = (int)((center[1] - width[1])/spacing[1] + 0.5);
  extent[3] = (int)((center[1] + width[1])/spacing[1] + 0.5);
  extent[4] = (int)((center[2] - width[2])/spacing[2] + 0.5);
  extent[5] = (int)((center[2] + width[2])/spacing[2] + 0.5);
  vtkKWEPaintbrushUtilities::GetIntersectingExtents(extent, imageExtent, extent);
  
  // Despite the fact that the FilterModule framework supports updates on 
  // requested extents, a lot of filters in ITK (such as the 
  // ConfidenceConnectedImageFilter don't really support updating a subextent.
  // So in most cases, you will have to extract a VOI).
  vtkExtractVOI *extractVOI = vtkExtractVOI::New();
  extractVOI->SetInput( self->GetImageData());
  extractVOI->SetVOI(extent);
  
  // Extract the first component
  vtkImageExtractComponents * extractComponent = vtkImageExtractComponents::New();
  extractComponent->SetInput( extractVOI->GetOutput() );
  extractComponent->GetOutput()->SetUpdateExtent( extent );
  extractComponent->SetComponents(0);
  extractComponent->Update();
  filter->SetRequestedExtent(extent);
  
  filter->SetInput( extractComponent->GetOutput() );
  filter->SetRequestedExtent(extent);
  filter->Update();
  filter->BoundWithRadiusOn();
  filter->GetOutputAsStencil(stencil);

  extractComponent->Delete();
  extractVOI->Delete();
  return 1;
}
vtkStandardNewMacro(vtkKWEITKBinaryThresholdPaintbrushOperation)
vtkCxxRevisionMacro(vtkKWEITKBinaryThresholdPaintbrushOperation, "$Revision: 590 $");

void vtkKWEITKBinaryThresholdPaintbrushOperation::
DoOperationOnStencil(vtkImageStencilData *stencilData, double p[3])
{
  this->PaintbrushShape->GetStencil( stencilData, p);

  vtkImageStencilData * stencil = vtkImageStencilData::New();
  switch (this->ImageData->GetScalarType())
    {
    vtkitkTemplateMacro( MyPaintbrushRunner( this, p, stencil, 
          static_cast< VTK_TT >(0)));
    
    default:
      {
      vtkErrorMacro(<< 
          "vtkKWEITKBinaryThresholdPaintbrushOperation: Unknown ScalarType");
      break;
      }
    }

  // Remove everything in the stencil.
  stencilData->AllocateExtents();

  // This will be the stuff that's filtered through the operation. In this
  // case the stuff in stencilData, thresholded.
  stencilData->Add(stencil);

  stencil->Delete();
}


int my_example( int , char *[],
                vtkKWApplication * ,
                vtkKWMyWindow    * example )
{
  example->Initialize();
  vtkImageData  * imageData  = example->GetInput();

  vtkKWEWidgetGroup *set = vtkKWEWidgetGroup::New();
  
  vtkKWEITKBinaryThresholdPaintbrushOperation * operation 
    = vtkKWEITKBinaryThresholdPaintbrushOperation::New();
  
  for (int i = 0; i < 3; i++)
    {
    vtkKWEPaintbrushWidget *w = vtkKWEPaintbrushWidget::New();
    w->SetInteractor( example->GetNthRenderWidget(i)->
                      GetRenderWindow()->GetInteractor());
    vtkKWEPaintbrushRepresentation2D * rep = 
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(w->GetRepresentation());

    rep->SetImageActor( example->GetNthImageActor(i) );

    // Note that an operation if set, must be set before the image data.
    rep->SetPaintbrushOperation(operation);
    rep->SetImageData(imageData);
    
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetSpacing( 
        imageData->GetSpacing() );
    rep->GetPaintbrushOperation()->GetPaintbrushShape()->SetOrigin( 
        imageData->GetOrigin() );

    set->AddWidget(w);
    w->Delete();
    }

  vtkKWEPaintbrushRepresentation2D * rep = 
    vtkKWEPaintbrushRepresentation2D::SafeDownCast(
      set->GetNthWidget(0)->GetRepresentation());  
  vtkKWEPaintbrushDrawing * drawing = rep->GetPaintbrushDrawing();
  for (unsigned int i = 0; i < set->GetNumberOfWidgets(); i++)
    {
    vtkKWEPaintbrushRepresentation2D * repr = 
      vtkKWEPaintbrushRepresentation2D::SafeDownCast(
        set->GetNthWidget(i)->GetRepresentation());
    repr->SetPaintbrushDrawing( drawing );
    }

  set->SetEnabled(1);

  int res = example->Run();
  
  operation->Delete();
  set->Delete();
  return res;
}

