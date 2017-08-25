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

#include "vtkKWEITKImage.h"

#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkImageStencilData.h"
#include "vtkImageIterator.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#define ItkToVtkPipelineGetOutputMacro(type) \
  { \
  typedef itk::Image< type, 3 > InputImageType; \
  typedef itk::ImageToVTKImageFilter< InputImageType > TType; \
  if (TType * t = dynamic_cast< TType * >(exporter.GetPointer())) \
    { \
    return t->GetOutput(); \
    } \
  }

#define VtkToItkPipelineGetOutputMacro(type) \
  { \
  typedef itk::Image< type, 3 > OutputImageType; \
  typedef itk::VTKImageToImageFilter< OutputImageType > TType; \
  if (TType * t = dynamic_cast< TType * >(exporter.GetPointer())) \
    { \
    return const_cast< OutputImageType * >(t->GetOutput()); \
    } \
  }

/** \class ItkToVtkPipeline
 *  This helper class will take care of instantiating the appropriate
 *  ITK Export class corresponding to the actual pixel type of the
 *  input image. */
template <class TPixel > class ItkToVtkPipeline
{
public:
  typedef itk::Image< TPixel, 3 >     ImageType;

  static void CreateExporter( itk::ImageBase<3>::Pointer  & imageBase,
                              itk::ProcessObject::Pointer & exporter )
    {
    if (ImageType * image =
          dynamic_cast< ImageType * >( imageBase.GetPointer() ))
      {
      typedef itk::ImageToVTKImageFilter< ImageType >   ExportFilterType;
      typedef typename ExportFilterType::Pointer ExportFilterPointer;
      ExportFilterPointer itkExporter = ExportFilterType::New();
      itkExporter->SetInput( image );
      exporter = itkExporter;
      }
    }

  static vtkImageData * GetOutput( itk::ProcessObject::Pointer & exporter )
    {
    ItkToVtkPipelineGetOutputMacro( unsigned char );
    ItkToVtkPipelineGetOutputMacro( char );
    ItkToVtkPipelineGetOutputMacro( unsigned short );
    ItkToVtkPipelineGetOutputMacro( short );
    ItkToVtkPipelineGetOutputMacro( unsigned int );
    ItkToVtkPipelineGetOutputMacro( int );
    ItkToVtkPipelineGetOutputMacro( unsigned long );
    ItkToVtkPipelineGetOutputMacro( long );
    ItkToVtkPipelineGetOutputMacro( float );
    ItkToVtkPipelineGetOutputMacro( double );
    return NULL;
    }
};

/** \class ItkToVtkPipeline
 *  This helper class will take care of instantiating the appropriate
 *  ITK Export class corresponding to the actual pixel type of the
 *  input image. */
template <class TPixel > class VtkToItkPipeline
{
public:
  typedef itk::Image< TPixel, 3 >     ImageType;

  static void CreateExporter( vtkImageData                * image,
                              itk::ProcessObject::Pointer & exporter )
    {
    typedef itk::VTKImageToImageFilter< ImageType > ExportFilterType;
    typedef typename ExportFilterType::Pointer ExportFilterPointer;
    ExportFilterPointer itkExporter = ExportFilterType::New();
    itkExporter->SetInput( image );
    exporter = itkExporter;
    }

  static itk::ImageBase<3> *
  GetOutput( itk::ProcessObject::Pointer & exporter )
    {
    VtkToItkPipelineGetOutputMacro( unsigned char );
    VtkToItkPipelineGetOutputMacro( char );
    VtkToItkPipelineGetOutputMacro( unsigned short );
    VtkToItkPipelineGetOutputMacro( short );
    VtkToItkPipelineGetOutputMacro( unsigned int );
    VtkToItkPipelineGetOutputMacro( int );
    VtkToItkPipelineGetOutputMacro( unsigned long );
    VtkToItkPipelineGetOutputMacro( long );
    VtkToItkPipelineGetOutputMacro( float );
    VtkToItkPipelineGetOutputMacro( double );
    return NULL;
    }
};

/** This helper macro will instantiate the pipeline creator for a particular
 * pixel type */
#define CreateItkToVtkPipelineMacro( PixelType ) \
  ItkToVtkPipeline< PixelType >::CreateExporter( \
      this->ItkImage, this->ItkToVtkExporter );

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWEITKImage );
vtkCxxRevisionMacro(vtkKWEITKImage, "$Revision: 1774 $");

//----------------------------------------------------------------------------
vtkKWEITKImage::vtkKWEITKImage()
{
  this->InputMode = vtkKWEITKImage::NO_INPUT_SET;
}

//----------------------------------------------------------------------------
vtkKWEITKImage::~vtkKWEITKImage()
{
}

//----------------------------------------------------------------------------
void vtkKWEITKImage::SetImage( itk::ImageBase< 3 > * image )
{
  if( this->InputMode     == vtkKWEITKImage::ITK_IMAGE_INPUT &&
      this->ItkImage      == image)
    {
    return;
    }

  this->InputMode = vtkKWEITKImage::ITK_IMAGE_INPUT;
  this->ItkImage = image;
  this->Modified();
  this->ItkImageTime.Modified();
}

//----------------------------------------------------------------------------
void vtkKWEITKImage::SetImage( vtkImageData * image )
{
  if( this->InputMode == vtkKWEITKImage::VTK_IMAGE_INPUT &&
      this->VtkImage == image)
    {
    return;
    }

  this->InputMode = vtkKWEITKImage::VTK_IMAGE_INPUT;
  this->VtkImage = image;
  this->Modified();
  this->VtkImageTime.Modified();
}

//----------------------------------------------------------------------------
void vtkKWEITKImage::SetImage( vtkImageStencilData * image )
{
  if( this->InputMode == vtkKWEITKImage::VTK_IMAGE_STENCIL_INPUT &&
      this->VtkImageStencil == image)
    {
    return;
    }

  this->VtkImageStencil = image;
  this->InputMode = vtkKWEITKImage::VTK_IMAGE_STENCIL_INPUT;
  this->Modified();
  this->VtkImageStencilTime.Modified();
}

//----------------------------------------------------------------------------
vtkImageData * vtkKWEITKImage::GetVTKImage()
{
  if (this->InputMode == ITK_IMAGE_INPUT && this->ItkImage)
    {
    if (this->VtkImageTime < this->ItkImageTime)
      {
      CreateItkToVtkPipelineMacro( unsigned char );
      CreateItkToVtkPipelineMacro( char );
      CreateItkToVtkPipelineMacro( unsigned short );
      CreateItkToVtkPipelineMacro( short );
      CreateItkToVtkPipelineMacro( unsigned int );
      CreateItkToVtkPipelineMacro( int );
      CreateItkToVtkPipelineMacro( unsigned long );
      CreateItkToVtkPipelineMacro( long );
      CreateItkToVtkPipelineMacro( float );
      CreateItkToVtkPipelineMacro( double );
      this->ItkToVtkExporter->Update();
      this->VtkImage = ItkToVtkPipeline<int>::GetOutput(this->ItkToVtkExporter);
      this->VtkImageTime.Modified();
      }
    }

  else if (this->InputMode == VTK_IMAGE_STENCIL_INPUT && this->VtkImageStencil)
    {
    if (this->VtkImageTime < this->VtkImageStencilTime)
      {
      if (!this->VtkImage)
        {
        this->VtkImage = vtkImageData::New();
        }

      vtkKWEITKImage::GetImageFromStencil(
          this->VtkImage, this->VtkImageStencil, 255, 0);
      this->VtkImageTime.Modified();
      }
    }

  else if (this->InputMode == VTK_IMAGE_INPUT && this->VtkImage)
    {
    // No conversion to do :)
    }

  else
    {
    this->VtkImage = NULL;
    }

  return this->VtkImage;
}

//----------------------------------------------------------------------------
vtkKWEITKImage::ImageBaseType * vtkKWEITKImage::GetITKImage()
{
  if (this->InputMode == ITK_IMAGE_INPUT && this->ItkImage)
    {
    // No conversion to do :)
    }

  else if (this->InputMode == VTK_IMAGE_STENCIL_INPUT && this->VtkImageStencil)
    {
    if (this->ItkImageTime < this->VtkImageStencilTime)
      {
      if (vtkImageData *image = this->GetVTKImage())
        {
        switch (image->GetScalarType())
          {
          vtkitkTemplateMacro( VtkToItkPipeline< VTK_TT >::CreateExporter(
                                    image, this->VtkToItkExporter) );

          default:
            {
            vtkErrorMacro(<< "vtkitkTemplateMacro: Unknown ScalarType");
            return NULL;
            }
          }

        this->ItkImage = VtkToItkPipeline<int>::GetOutput(this->VtkToItkExporter);
        this->ItkImageTime.Modified();
        }
      }
    }

  else if (this->InputMode == VTK_IMAGE_INPUT && this->VtkImage)
    {
    switch (this->VtkImage->GetScalarType())
      {
      vtkitkTemplateMacro( VtkToItkPipeline<VTK_TT>::CreateExporter(
              this->VtkImage, this->VtkToItkExporter) );

      default:
        {
        vtkErrorMacro(<< "vtkitkTemplateMacro: Unknown ScalarType");
        return NULL;
        }
      }

    this->ItkImage = VtkToItkPipeline<int>::GetOutput(this->VtkToItkExporter);
    this->ItkImageTime.Modified();
    }

  else
    {
    this->ItkImage = NULL;
    }

  return this->ItkImage;
}

//----------------------------------------------------------------------------
vtkImageStencilData * vtkKWEITKImage::GetVTKImageStencilData()
{
  if (this->InputMode == ITK_IMAGE_INPUT && this->ItkImage)
    {
    if (this->VtkImageStencilTime < this->ItkImageTime)
      {
      if (!this->VtkImageStencil)
        {
        this->VtkImageStencil = vtkImageStencilData::New();
        }

      vtkKWEITKImage::GetStencilFromImage(this->GetVTKImage(), this->VtkImageStencil, 128);
      this->VtkImageStencilTime.Modified();
      }
    }

  else if (this->InputMode == VTK_IMAGE_INPUT && this->VtkImage)
    {
    if (this->VtkImageStencilTime < this->VtkImageTime)
      {
      if (!this->VtkImageStencil)
        {
        this->VtkImageStencil = vtkImageStencilData::New();
        }

      vtkKWEITKImage::GetStencilFromImage(this->GetVTKImage(), this->VtkImageStencil, 128);
      this->VtkImageStencilTime.Modified();
      }
    }

  else if (this->InputMode == VTK_IMAGE_STENCIL_INPUT && this->VtkImageStencil)
    {
    // No conversion to do :)
    }

  else
    {
    this->VtkImageStencil = NULL;
    }

  return this->VtkImageStencil;
}

//----------------------------------------------------------------------------
vtkKWEITKImage::ITKScalarPixelType vtkKWEITKImage::GetITKScalarPixelType()
{
  this->GetITKImage();

  ITKScalarPixelType pixelType = itk::ImageIOBase::UCHAR;

  ImageBaseType * itkImageBase = this->ItkImage.GetPointer();

  if( dynamic_cast< itk::Image< unsigned char, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::UCHAR;
    }
  else if( dynamic_cast< itk::Image< char, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::CHAR;
    }
  else if( dynamic_cast< itk::Image< short, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::SHORT;
    }
  else if( dynamic_cast< itk::Image< unsigned short, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::USHORT;
    }
  else if( dynamic_cast< itk::Image< int, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::INT;
    }
  else if( dynamic_cast< itk::Image< unsigned int, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::UINT;
    }
  else if( dynamic_cast< itk::Image< long, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::LONG;
    }
  else if( dynamic_cast< itk::Image< unsigned long, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::ULONG;
    }
  else if( dynamic_cast< itk::Image< float, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::FLOAT;
    }
  else if( dynamic_cast< itk::Image< double, 3> * >( itkImageBase ) )
    {
    pixelType = itk::ImageIOBase::DOUBLE;
    }

  return pixelType;
}

//----------------------------------------------------------------------------
int vtkKWEITKImage::GetVTKScalarPixelType()
{
  return this->GetVTKImage()->GetScalarType();
}

//----------------------------------------------------------------------------
void vtkKWEITKImage::GetImageFromStencil(
                          vtkImageData *image,
                          vtkImageStencilData *stencilData,
                          unsigned char inVal, unsigned char outVal)
{
  int extent[6];
  stencilData->GetExtent( extent );
  image->SetExtent( extent );
  image->SetScalarTypeToUnsignedChar();
  image->SetNumberOfScalarComponents(1);
  image->AllocateScalars();

  // Fill image with zeroes

  vtkImageIterator< unsigned char > it(image, image->GetExtent());
  while( !it.IsAtEnd() )
    {
    unsigned char *inSI    = it.BeginSpan();
    unsigned char *inSIEnd = it.EndSpan();
    while (inSI != inSIEnd)
      {
      *inSI = outVal;
      ++inSI;
      }
    it.NextSpan();
    }

  vtkIdType increments[3];
  image->GetIncrements( increments );

  int iter = 0;
  for (int z=extent[4]; z <= extent[5]; z++)
    {
    for (int y=extent[2]; y <= extent[3]; y++, iter = 0)
      {
      int r1,r2;
      int moreSubExtents = 1;
      while( moreSubExtents )
        {
        moreSubExtents = stencilData->GetNextExtent(
          r1, r2, extent[0], extent[1], y, z, iter);

        // sanity check
        if (r1 <= r2 )
          {
          unsigned char *beginExtent =
            (unsigned char *)(image->GetScalarPointer(r1, y, z));
          unsigned char *endExtent   =
            (unsigned char *)(image->GetScalarPointer(r2, y, z));
          while (beginExtent <= endExtent)
            {
            *beginExtent = inVal;
            beginExtent += increments[0];
            }
          }
        } // end for each extent tuple
      } // end for each scan line
    } // end of each slice
}

//----------------------------------------------------------------------------
template < class T >
void vtkKWEITKImageGetStencilFromImage( vtkImageData *image,
                                     vtkImageStencilData *stencilData,
                                     T threshold)
{
  int extent[6];
  double spacing[3], origin[3];
  image->GetExtent(extent);
  image->GetSpacing(spacing);
  image->GetOrigin(origin);

  stencilData->SetExtent(extent);
  stencilData->SetSpacing(spacing);
  stencilData->SetOrigin(origin);
  stencilData->AllocateExtents();

  vtkImageIterator< T > it(image, extent);

  int ends[2], index[3];
  index[1] = extent[2];
  index[2] = extent[4];

  while( !it.IsAtEnd() )
    {
    T *inSI    = it.BeginSpan();
    T *inSIEnd = it.EndSpan();

    index[0] = extent[0];
    ends[0] = -1;
    ends[1] = -1;
    if (*inSI >= threshold)
      {
      ends[0] = extent[0];
      }

    // for each row

    while (inSI != inSIEnd)
      {

      if (ends[0] == -1 && *inSI >= threshold)
        {
        // look for start
        ends[0] = index[0];
        ++index[0];
        ++inSI;
        continue;
        }

      if (ends[0] != -1 && ends[1] == -1 && *inSI < threshold)
        {
        ends[1] = index[0];
        stencilData->InsertNextExtent(ends[0], ends[1], index[1], index[2]);
        ends[0] = ends[1] = -1;
        }

      ++index[0];
      ++inSI;
      }

    if (*(inSI-1) >= threshold) // ends[0] has to be not -1, ends[1] has to be -1;
      {
      stencilData->InsertNextExtent(ends[0], extent[1], index[1], index[2]);
      }

    it.NextSpan();

    if (index[1] == extent[3])
      {
      ++index[2];
      index[1] = extent[2];
      }
    else
      {
      ++index[1];
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEITKImage::GetStencilFromImage( vtkImageData *image,
                                       vtkImageStencilData *stencilData,
                                       double threshold)
{
  if (image == NULL)
    {
    stencilData = NULL;
    return;
    }

  switch (image->GetScalarType())
    {
    vtkTemplateMacro( vtkKWEITKImageGetStencilFromImage(
              image, stencilData, static_cast<VTK_TT>(threshold)));
    }
}
