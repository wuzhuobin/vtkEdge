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

#ifndef __vtkKWEITKImage_h
#define __vtkKWEITKImage_h

#include "VTKEdgeConfigure.h" // needed for export symbols directives
#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkCastImageFilter.h"
#include "vtkKWEITKFilterModuleBase.h"
#include "itkImage.h"
#include "vtkImageData.h"
#include "itkImageIOBase.h"
#include "vtkObject.h"
#include "vtkSmartPointer.h"

class vtkImageData;
class vtkImageImport;
class vtkImageStencilData;

#if defined(VTK_TYPE_USE___INT64)
  #define vtkitkTemplateMacro___INT64 \
    vtkTemplateMacroCase_si64(VTK___INT64, __int64, call);
#else 
  #define vtkitkTemplateMacro___INT64 
#endif

#if defined(VTK_TYPE_USE___INT64) && defined(VTK_TYPE_CONVERT_UI64_TO_DOUBLE)
  #define vtkitkTemplateMacro___UINT64 \
    vtkTemplateMacroCase_ui64(VTK_UNSIGNED___INT64, unsigned __int64, call);
#else 
  #define vtkitkTemplateMacro___UINT64
#endif

// ITK's support for 64 bit types, long long etc is poor, not as exhaustive
// as VTK. Define an alternate macro here that ignores those types. Nobody
// will use them anyway.
#define vtkitkTemplateMacro(call)                                           \
  vtkTemplateMacroCase(VTK_DOUBLE, double, call);                           \
  vtkTemplateMacroCase(VTK_FLOAT, float, call);                             \
  vtkitkTemplateMacro___INT64                                               \
  vtkitkTemplateMacro___UINT64                                              \
  vtkTemplateMacroCase(VTK_LONG, long, call);                               \
  vtkTemplateMacroCase(VTK_UNSIGNED_LONG, unsigned long, call);             \
  vtkTemplateMacroCase(VTK_INT, int, call);                                 \
  vtkTemplateMacroCase(VTK_UNSIGNED_INT, unsigned int, call);               \
  vtkTemplateMacroCase(VTK_SHORT, short, call);                             \
  vtkTemplateMacroCase(VTK_UNSIGNED_SHORT, unsigned short, call);           \
  vtkTemplateMacroCase(VTK_CHAR, char, call);                               \
  vtkTemplateMacroCase(VTK_SIGNED_CHAR, signed char, call);                 \
  vtkTemplateMacroCase(VTK_UNSIGNED_CHAR, unsigned char, call)


// .NAME vtkKWEITKImage
// .SECTION Description
// This class provides seamless conversion from VTK to ITK images and vice
// versa. Specifically, you can set an ITK image (itk::Image< T >) and get 
// the corresponding VTK image (vtkImageData), or a binarized stencil image 
// (vtkImageStencilData). You can also go the other way around.
// .SECTION See Also
class VTKEdge_WIDGETS_EXPORT vtkKWEITKImage : public vtkObject
{

public:
  static vtkKWEITKImage* New();
  vtkTypeRevisionMacro(vtkKWEITKImage,vtkObject);

  typedef itk::ImageIOBase::IOComponentType      ITKScalarPixelType;

  // Description:
  // Set methods. You should use only one of them. Using another
  // invalidates the previous "Set..."
  void             SetImage( itk::ImageBase< 3 > *    );
  void             SetImage( vtkImageData        *    );
  void             SetImage( vtkImageStencilData *    );

  // Description:
  // Get an image as a ITK (untemplated or templated and implicitly casted) / 
  // VTK / VTK binarized image.
  vtkImageData                          * GetVTKImage();
  vtkImageStencilData                   * GetVTKImageStencilData();
  itk::ImageBase< 3 >                   * GetITKImage();
  template< class T > itk::Image<T, 3>  * GetITKImage( T ) 
    { return this->InternalGetITKImage(static_cast<T>(0)); }
  
  // Return the pixel type using ITK enums.
  ITKScalarPixelType GetITKScalarPixelType();

  // Return the pixel type using VTK enums.
  int GetVTKScalarPixelType();

protected:
  vtkKWEITKImage();
  ~vtkKWEITKImage();

  // Description:
  // Get a binary stencil from an image. All pixels >= threshold are
  // considered within the stencil.
  static void GetStencilFromImage( vtkImageData *, 
                                   vtkImageStencilData *, double thresold=127.5 );

  // Description:
  // Construct an unsigned char vtkImageData from a stencil. 
  static void GetImageFromStencil( vtkImageData *, 
                                   vtkImageStencilData *,
                                   unsigned char inVal=255, 
                                   unsigned char outVal=0);
    
  enum InputType
    {
    VTK_IMAGE_INPUT = 0,
    VTK_IMAGE_STENCIL_INPUT,
    ITK_IMAGE_INPUT,
    NO_INPUT_SET
    };

  typedef itk::ImageBase< 3 >                    ImageBaseType;
  typedef ImageBaseType::Pointer                 ImagePointer;
  typedef ImageBaseType::ConstPointer            ImageConstPointer;
    
private:
  vtkKWEITKImage(const vtkKWEITKImage&);   // Not implemented.
  void operator=(const vtkKWEITKImage&);  // Not implemented.

  InputType                              InputMode;
  itk::ProcessObject::Pointer            ItkToVtkExporter;
  itk::ProcessObject::Pointer            VtkToItkExporter;
  itk::ProcessObject::Pointer            ItkToItkCastor;
  
  vtkSmartPointer< vtkImageStencilData > VtkImageStencil;
  vtkSmartPointer< vtkImageData >        VtkImage;
  itk::ImageBase<3>::Pointer             ItkImage;
  vtkTimeStamp                           VtkImageStencilTime;
  vtkTimeStamp                           VtkImageTime;
  vtkTimeStamp                           ItkImageTime;
  
  
  /** \class ItkToItkCastor
   *  This helper class will take care of casting an ITK image to one of type
   * TPixel. */
  template <class TPixel, class TOutputPixel > class ItkToItkCast
  {
  public:
    typedef itk::Image< TPixel, 3 >       InputImageType;
    typedef itk::Image< TOutputPixel, 3 > OutputImageType;
    typedef itk::CastImageFilter< InputImageType, 
                                  OutputImageType > CastFilterType;
    static itk::Image<TOutputPixel, 3> * Cast( 
                     itk::ImageBase<3> * imageBase, 
                     itk::ProcessObject::Pointer & castor )
      {
      if (InputImageType * image = 
            dynamic_cast< InputImageType * >( imageBase ))
        {
        typedef typename CastFilterType::Pointer CastFilterPointer;
        CastFilterPointer caster = dynamic_cast< CastFilterType * >(castor.GetPointer());
        if (!caster) { caster = CastFilterType::New(); }
        caster->SetInput( image );
        castor = caster;
        caster->Update();
        return caster->GetOutput();
        }
      return NULL;
      }
  };
  
  template<class T> itk::Image<T, 3>  * InternalGetITKImage( T ) 
    {
    if (itk::Image<T, 3> * i = ItkToItkCast< unsigned char,  T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< char,           T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< unsigned short, T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< short,          T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< unsigned int,   T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< int,            T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< unsigned long,  T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< long,           T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< float,          T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    if (itk::Image<T, 3> * i = ItkToItkCast< double,         T >::Cast( this->GetITKImage(), this->ItkToItkCastor )) return i;
    return NULL;
    }
};

#endif
