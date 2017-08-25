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
