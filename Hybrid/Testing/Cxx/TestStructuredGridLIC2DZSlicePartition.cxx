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

#include "vtkKWEStructuredGridLIC2D.h"
#include "vtkPNGReader.h"
#include "vtkXMLStructuredGridReader.h"
#include "vtkExtractGrid.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkTexture.h"
#include "vtkDataSetMapper.h"
#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"
#include "vtkPNGWriter.h"
#include "vtkImageShiftScale.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkImageExtractComponents.h"
#include "vtkTestUtilities.h"
#include "vtkImageIterator.h"
//#include "vtkStructuredGridIterator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <assert.h>

const int numParts=5;
const int magnification=1; // 8


// Iterate over points (not cell) on a structured dataset, aka a dataset
// having an extent.

class vtkStructuredDataPointIterator
{
public:
  // Description:
  // Default constructor.
  // \post done: IsAtEnd()
  vtkStructuredDataPointIterator()
  {
    this->Id=0;
    this->EndId=0;
    this->SpanEndId=0;
    this->SliceEndId=0;
    this->Increments[0]=0;
    this->Increments[1]=0;
    this->Increments[2]=0;
    this->ContinuousIncrements[0]=0;
    this->ContinuousIncrements[1]=0;
    this->ContinuousIncrements[2]=0;
  }

  // Description:
  // Start iterating over the extent but only on a give sub extent.
  // \pre ext_exists: ext!=0
  // \pre valid_ext: ext[0]<=ext[1] && ext[2]<=ext[3] && ext[4]<=ext[5]
  // \pre subext_exists: subext!=0
  // \pre valid_subext: subext[0]<=subext[1] && subext[2]<=subext[3] && subext[4]<=subext[5]
  // \pre sub_in_ext: ext[0]<=subext[0] && subext[1]<=ext[1] && ext[2]<=subext[2] && subext[3]<=ext[3] && ext[4]<=subext[4] && subext[5]<=ext[5]
  void Start(int *ext,
             int *subext)
  {
    assert("pre: ext_exists" && ext!=0);
    assert("pre: valid_ext" && ext[0]<=ext[1] && ext[2]<=ext[3] &&
           ext[4]<=ext[5]);
    assert("pre: subext_exists" && subext!=0);
    assert("pre: valid_subext" && subext[0]<=subext[1] && subext[2]<=subext[3] &&
           subext[4]<=subext[5]);
    assert("pre: sub_in_ext" && ext[0]<=subext[0] && subext[1]<=ext[1] && ext[2]<=subext[2] && subext[3]<=ext[3] && ext[4]<=subext[4] && subext[5]<=ext[5]);
    
    //    this->Id=static_cast<DType *>(id->GetScalarPointerForExtent(subext));
    vtkIdType tupleInc[3];
    this->GetIncrements(ext,tupleInc);
    this->GetIncrements(ext,this->Increments);

    int coordinates[3];
    coordinates[0]=subext[0];
    coordinates[1]=subext[2];
    coordinates[2]=subext[4];
    this->Id=this->GetId(ext,coordinates);

    //  id->GetIncrements(this->Increments[0], this->Increments[1],
    //                    this->Increments[2]);
    this->GetContinuousIncrements(ext,subext,this->ContinuousIncrements[0],
                                  this->ContinuousIncrements[1],
                                  this->ContinuousIncrements[2]);
    //  this->EndId= static_cast<DType *>(id->GetScalarPointer(ext[1],ext[3],ext[5]))
    //    +this->Increments[0];

    
    coordinates[0]=subext[1];
    coordinates[1]=subext[3];
    coordinates[2]=subext[5];
    this->EndId=this->GetId(ext,coordinates);
    //    this->EndId= id->GetScalarPointer(ext[1],ext[3],ext[5]))
    //    +this->Increments[0];

  // if the extent is empty then the end idr should equal the begin id.
  if (ext[1] < ext[0] || ext[3] < ext[2] || ext[5] < ext[4])
    {
    this->EndId = this->Id;
    }

  this->SpanEndId=this->Id + tupleInc[0]*(subext[1] - subext[0] + 1);
  this->SliceEndId = this->Id + tupleInc[1]*(subext[3] - subext[2] + 1);
  }

  // Description:
  // Test if the end of the extent has been reached
  bool IsAtEnd()
  {
    return this->Id >= this->EndId;
  }
  
  // Description:
  // Return the first Id for the span
  // \pre not_done: !IsAtEnd()
  vtkIdType GetBeginSpan()
  {
    assert("pre: not_done" && !this->IsAtEnd());
    return this->Id;
  }

  // Description:
  // Return the Id after  the end of the span
  // \pre not_done: !IsAtEnd()
  vtkIdType GetEndSpan()
  {
    assert("pre: not_done" && !this->IsAtEnd());
    return this->SpanEndId;
  }
  
  // Description:
  // Move the iterator to the next span
  // \pre not_done: !IsAtEnd()
  void Next()
  {
    assert("pre: not_done" && !this->IsAtEnd());

    this->Id += this->Increments[1];
    this->SpanEndId += this->Increments[1];
    if (this->Id >= this->SliceEndId)
      {
        this->Id += this->ContinuousIncrements[2];
        this->SpanEndId += this->ContinuousIncrements[2];
        this->SliceEndId += this->Increments[2];
      }
  }

protected:

  // Description:
  // Return tuple increments. If you want increments per component, multiplied
  // each increment by the number of components.
  // \pre: ext_exists: ext!=0
  // \pre valid_ext: ext[0]<=ext[1] && ext[2]<=ext[3] && ext[4]<=ext[5]
  void GetIncrements(int *ext,
                     vtkIdType increments[3])
  {
    assert("pre: ext_exists" && ext!=0);
    assert("pre: valid_ext" && ext[0]<=ext[1] && ext[2]<=ext[3] &&
           ext[4]<=ext[5]);
    increments[0] = 1;
    increments[1] = increments[0] * (ext[1]-ext[0]+1);
    increments[2] = increments[1] * (ext[3]-ext[2]+1);
  }

  void GetContinuousIncrements(int ext[6],
                               int subext[6], vtkIdType &incX,
                               vtkIdType &incY, vtkIdType &incZ)
{
  int e0, e1, e2, e3;

  incX = 0;
  const int *selfExtent =ext;

  e0 = subext[0];
  if (e0 < selfExtent[0])
    {
    e0 = selfExtent[0];
    }
  e1 = subext[1];
  if (e1 > selfExtent[1])
    {
    e1 = selfExtent[1];
    }
  e2 = subext[2];
  if (e2 < selfExtent[2])
    {
    e2 = selfExtent[2];
    }
  e3 = subext[3];
  if (e3 > selfExtent[3])
    {
    e3 = selfExtent[3];
    }

  incY = this->Increments[1] - (e1 - e0 + 1)*this->Increments[0];
  incZ = this->Increments[2] - (e3 - e2 + 1)*this->Increments[1];
}

vtkIdType GetId(int ext[6],
                int coordinates[3])
{
  vtkIdType tupleInc[3];
  this->GetIncrements(ext,tupleInc);
  return (coordinates[0]-ext[0])*tupleInc[0]+(coordinates[1]-ext[2])*tupleInc[1]+(coordinates[2]-ext[4])*tupleInc[2];
}

  vtkIdType Id;
  vtkIdType SpanEndId;
  vtkIdType SliceEndId;
  vtkIdType EndId;
  vtkIdType Increments[3];
  vtkIdType ContinuousIncrements[3];
};

// ----------------------------------------------------------------------------
void Merge0(vtkStructuredGrid *dest,
            vtkStructuredGrid *src)
{
  if (!src || !dest)
    {
    return;
    }


  //  int srcType=src->GetPointData()->GetScalars()->GetDataType();
  //  int destType=dest->GetPointData()->GetScalars()->GetDataType();

  //  if (src->GetScalarType() != dest->GetScalarType())
  //  if(srcType!=destType)
  //    {
  //      cout << vtkImageScalarTypeNameMacro(srcType) << ", " <<  vtkImageScalarTypeNameMacro(destType) << endl;
  //    abort();
  //    }

  //  vtkStructuredGridIterator<unsigned char> inIt(src, src->GetExtent());
  vtkStructuredDataPointIterator inIt;

  inIt.Start(src->GetExtent(), src->GetExtent());
  int outextent[6];
  src->GetExtent(outextent);

  //  vtkStructuredGridIterator<unsigned char> outIt(dest, outextent);
  vtkStructuredDataPointIterator outIt;
  outIt.Start(dest->GetExtent(), outextent);
  
  vtkPointData *inPD=src->GetPointData();
  vtkPointData *outPD=dest->GetPointData();

  while (!outIt.IsAtEnd() && !inIt.IsAtEnd())
    {
      //    unsigned char* spanOut = outIt.BeginSpan();
      //    unsigned char* spanIn = inIt.BeginSpan();
      //    unsigned char* outSpanEnd = outIt.EndSpan();
      //    unsigned char* inSpanEnd = inIt.EndSpan();

    vtkIdType spanOut = outIt.GetBeginSpan();
    vtkIdType spanIn = inIt.GetBeginSpan();
    vtkIdType outSpanEnd = outIt.GetEndSpan();
    vtkIdType inSpanEnd = inIt.GetEndSpan();

    if (outSpanEnd != spanOut && inSpanEnd != spanIn)
      {
      size_t minO = outSpanEnd - spanOut;
      size_t minI = inSpanEnd - spanIn;
      
      if(minO>minI)
        {
          minO=minI;
        }
      //      memcpy(spanOut, spanIn, minO);

      // copy point data.
      vtkIdType inId;
      vtkIdType outId;
      inId=spanIn;
      outId=spanOut;
      vtkIdType i=0;
      while(i<static_cast<vtkIdType>(minO))
        {
          outPD->CopyData(inPD,inId,outId);
          ++i;
          ++inId;
          ++outId;
        }

      }
    inIt.Next();
    outIt.Next();
    }
}
// ----------------------------------------------------------------------------
void Merge2(vtkImageData *dest,
            vtkImageData *src)
{
  if (!src || !dest)
    {
    return;
    }


  if (src->GetScalarType() != dest->GetScalarType())
    {
    cout << src->GetScalarTypeAsString() << ", " << dest->GetScalarTypeAsString() << endl;
    abort();
    }

  vtkImageIterator<float> inIt(src, src->GetExtent());
  int outextent[6];
  src->GetExtent(outextent);

  vtkImageIterator<float> outIt(dest, outextent);

  cout<<"src ext="<<outextent[0]<<" "<<outextent[1]<<" "<<outextent[2]<<" "<<outextent[3]<<" "<<outextent[4]<<" "<<outextent[5]<<endl;

  int debugExt[6];
  dest->GetExtent(debugExt);

 cout<<"dest ext="<<debugExt[0]<<" "<<debugExt[1]<<" "<<debugExt[2]<<" "<<debugExt[3]<<" "<<debugExt[4]<<" "<<debugExt[5]<<endl;
  


  while (!outIt.IsAtEnd() && !inIt.IsAtEnd())
    {
    float* spanOut = outIt.BeginSpan();
    float* spanIn = inIt.BeginSpan();
    float* outSpanEnd = outIt.EndSpan();
    float* inSpanEnd = inIt.EndSpan();
    if (outSpanEnd != spanOut && inSpanEnd != spanIn)
      {
      size_t minO = outSpanEnd - spanOut;
      size_t minI = inSpanEnd - spanIn;
      memcpy(spanOut, spanIn, (minO < minI)? minO : minI);
      }
    inIt.NextSpan();
    outIt.NextSpan();
    }
}

// ----------------------------------------------------------------------------
int TestStructuredGridLIC2DZSlicePartition(int argc, char *argv[])
{
  char* filename =  
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/timestep_0_15.vts");
  vtkXMLStructuredGridReader *reader=vtkXMLStructuredGridReader::New();
  reader->SetFileName(filename);
  delete []filename;
  reader->Update();
  
  int extent[6];
  reader->GetOutput()->GetExtent(extent);
  vtkExtractGrid *voi=vtkExtractGrid::New();
  voi->SetInputConnection(reader->GetOutputPort());
  reader->Delete();
  // test case:
  // One Z-slice
  voi->SetVOI(extent[0],extent[1],extent[2],extent[3],10,10);
  
  // One X-slice
  //  voi->SetVOI(extent[1],extent[1],extent[2],extent[3],extent[4],extent[5]);
  
  // One Y-slice
  //voi->SetVOI(extent[0],extent[1],extent[2],extent[2],extent[4],extent[5]);

  //int width=extent[1]-extent[0]+1;
  //int height=extent[3]-extent[2]+1;

  cout<<"whole ext="<<extent[0]<<" "<<extent[1]<<" "<<extent[2]<<""<<extent[3]<<endl;


  voi->Update();
  
  vtkPNGReader *pngReader=vtkPNGReader::New();
  filename = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/noise.png");
  pngReader->SetFileName(filename);
  delete[] filename;
  pngReader->Update();
  
  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  vtkRenderWindow *win=vtkRenderWindow::New();
  iren->SetRenderWindow(win);
  win->Delete();
  
  // Dont't magnify the geometry.
  vtkStructuredGrid *output0=vtkStructuredGrid::New();
  output0->SetExtent(voi->GetOutput()->GetExtent());
  // spacing and origi are not used because we will use the imagedata as
  // a texture.
  //  output1->SetSpacing(probeData->GetSpacing()); // not used
  //  output1->SetOrigin(probeData->GetOrigin()); // not used
  //  output0->SetScalarTypeToUnsignedChar();
  //  output0->AllocateScalars();

  // The big image.
  vtkImageData *output1=vtkImageData::New();
  output1->SetExtent(magnification*extent[0],magnification*(extent[1]+1)-1,
                     magnification*extent[2],magnification*(extent[3]+1)-1,
                     magnification*10,magnification*10);
  //  output1->SetDimensions(width * magnification, height * magnification, 1);


  // spacing and origi are not used because we will use the imagedata as
  // a texture.
  //  output1->SetSpacing(probeData->GetSpacing()); // not used
  //  output1->SetOrigin(probeData->GetOrigin()); // not used
  //  output1->SetScalarTypeToUnsignedChar();
  output1->SetScalarTypeToFloat();
  output1->AllocateScalars();

  vtkKWEStructuredGridLIC2D *filter=vtkKWEStructuredGridLIC2D::New();
  filter->SetContext(win);
  //  filter->SetInputConnection(0,voi->GetOutputPort(0));

  vtkStructuredGrid *clone3=vtkStructuredGrid::New();
  clone3->ShallowCopy(voi->GetOutput());
  filter->SetInput(0,clone3);

  voi->Delete();
  filter->SetInputConnection(1,pngReader->GetOutputPort(0));
  pngReader->Delete();
  //  filter->SetSteps(1000);
  //  filter->SetStepSize(0.00001);
  filter->SetSteps(100);
  filter->SetStepSize(0.001);
  filter->SetMagnification(magnification);
//  filter->Update();
  filter->UpdateInformation();

  int part=0;
  while(part<numParts)
    {
      cout<<"part="<<part<<endl;
      vtkStreamingDemandDrivenPipeline* sddp = vtkStreamingDemandDrivenPipeline::SafeDownCast(
      filter->GetExecutive());
    sddp->SetUpdateExtent(0, part, numParts, 0);
    filter->Update();

    vtkStructuredGrid *clone0=vtkStructuredGrid::New();
    vtkStructuredGrid *sg=
    vtkStructuredGrid::SafeDownCast(filter->GetOutputDataObject(0));

    clone0->ShallowCopy(sg);
    ::Merge0(output0,clone0);
    clone0->Delete();
    
    vtkImageData *clone=vtkImageData::New();
    vtkImageData *im=
    vtkImageData::SafeDownCast(filter->GetOutputDataObject(1));

    clone->ShallowCopy(im);
    ::Merge2(output1,clone);
    clone->Delete();

      ++part;
    }
  vtkRenderer *renderer=vtkRenderer::New();
  renderer->SetBackground(0.2,0.1,0.2);
  win->AddRenderer(renderer);
  renderer->Delete();

  vtkActor *actor=vtkActor::New();
  renderer->AddViewProp(actor);
  actor->Delete();

  vtkDataSetSurfaceFilter *geometry=vtkDataSetSurfaceFilter::New();
  geometry->SetInputConnection(filter->GetOutputPort(0));
  filter->Delete();
  
  vtkPolyDataMapper *mapper=vtkPolyDataMapper::New();
  mapper->SetInputConnection(geometry->GetOutputPort(0));
  geometry->Delete();

  double range[2];
  output1->GetPointData()->GetScalars()->GetRange(range);

  vtkImageShiftScale *caster=vtkImageShiftScale::New();
  caster->SetInput(output1);
  caster->SetOutputScalarTypeToUnsignedChar();
  caster->SetShift(-range[0]);
  caster->SetScale(255.0/(range[1]-range[0]));
  caster->Update();

  vtkTexture *texture=vtkTexture::New();
  texture->SetInputConnection(caster->GetOutputPort(0));
  caster->Delete();

  /////////

  actor->SetTexture(texture);
  texture->Delete();

  actor->SetMapper(mapper);
  mapper->SetScalarVisibility(0);
  mapper->Delete();

  /////


  // Initial values.
  //renderer->SetTwoSidedLighting(1);
  // modified values.
  renderer->SetTwoSidedLighting(0);

  win->Render();

  int retVal = vtkRegressionTestImage(win);
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    iren->Start();
    }
  iren->Delete();
  
  return !retVal;
}
