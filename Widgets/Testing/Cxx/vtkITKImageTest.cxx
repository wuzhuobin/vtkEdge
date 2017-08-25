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

#include "vtkITKImage.h"
#include "vtkVolume16Reader.h"
#include "vtkTestUtilities.h"
#include "itkImageFileWriter.h"
#include "vtkXMLImageDataWriter.h"

int main( int argc, char *argv[] )
{
  char* fname2 = vtkTestUtilities::ExpandDataFileName(
      argc, argv, "Data/headsq/quarter");
  vtkVolume16Reader *v16 = vtkVolume16Reader::New();
  v16->SetDataDimensions (64,64);
  v16->SetImageRange (1,93);
  v16->SetDataByteOrderToLittleEndian();
  v16->SetFilePrefix (fname2);
  v16->SetDataSpacing (3.2, 3.2, 1.5);
  v16->Update();

  vtkITKImage * image = vtkITKImage::New();
  image->SetImage( v16->GetOutput() );

  // vtkImageData -> vtkImageData
  //
  vtkXMLImageDataWriter *w = vtkXMLImageDataWriter::New();
  w->SetInput( image->GetVTKImage() );
  image->GetVTKImage()->Print(std::cout);
  w->SetFileName( "VTKImageToVTKImage.vtp" );
  w->Update();

  // vtkImageData -> itk::Image
  //
  typedef itk::Image< unsigned short, 3 > UShortImageType;
  itk::ImageFileWriter< UShortImageType >::Pointer w2 = itk::ImageFileWriter< UShortImageType >::New();
  w2->SetInput( image->GetITKImage( static_cast< unsigned short >(0) ) );
  w2->UseCompressionOn();
  w2->SetFileName( "VTKImageToITKImage.mhd" );
  w2->Update();

  // vtkImageData -> vtkImageStencilData -> itk::Image< T >
  //
  typedef itk::Image< unsigned char, 3 > UCharImageType;
  itk::ImageFileWriter< UCharImageType >::Pointer
    w3 = itk::ImageFileWriter< UCharImageType >::New();
  vtkITKImage *image2 = vtkITKImage::New();
  image2->SetImage(image->GetVTKImageStencilData());
  w3->SetInput( image2->GetITKImage( static_cast< unsigned char >(0) ) );
  w3->SetFileName( "VTKImageToVTKImageStencilDataToITKImage.mhd" );
  w3->UseCompressionOn();
  w3->Update();

  // itk::Image< T >    -->   vtkImageData
  //
  vtkXMLImageDataWriter *w4 = vtkXMLImageDataWriter::New();
  vtkITKImage *image3 = vtkITKImage::New();
  image3->SetImage(image2->GetITKImage( static_cast< unsigned char >(0)));
  w4->SetInput( image3->GetVTKImage() );
  w4->SetFileName( "ITKImageToVTKImage.vtp" );
  w4->Update();

  w->Delete();
  w4->Delete();
  v16->Delete();
  image->Delete();
  image2->Delete();
  image3->Delete();
  delete [] fname2;

  return EXIT_SUCCESS;
}
