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
#include "vtkKWEGlyph3DMapper.h"

#include "vtkActor.h"
#include "vtkBitArray.h"
#include "vtkBoundingBox.h"
#include "vtkDataArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkDefaultPainter.h"
#include "vtkGarbageCollector.h"
#include "vtkHardwareSelector.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPainterPolyDataMapper.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkScalarsToColorsPainter.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTimerLog.h"
#include "vtkTransform.h"
#include "vtkHardwareSelectionPolyDataPainter.h"

#include <assert.h>
#include <vtkstd/vector>
#include "vtkgl.h"

vtkCxxRevisionMacro(vtkKWEGlyph3DMapper, "$Revision: 808 $");
vtkStandardNewMacro(vtkKWEGlyph3DMapper);

template <class T>
static T vtkClamp(T val, T min, T max)
{
  val = val < min? min : val;
  val = val > max? max : val;
  return val;
}

class vtkKWEGlyph3DMapperArray
{
public:
  vtkstd::vector<vtkSmartPointer<vtkPainterPolyDataMapper > > Mappers;
};

// ---------------------------------------------------------------------------
// Construct object with scaling on, scaling mode is by scalar value,
// scale factor = 1.0, the range is (0,1), orient geometry is on, and
// orientation is by vector. Clamping and indexing are turned off. No
// initial sources are defined.
vtkKWEGlyph3DMapper::vtkKWEGlyph3DMapper()
{
  this->SetNumberOfInputPorts(2);

  this->Scaling = true;
  this->ScaleMode = SCALE_BY_MAGNITUDE;
  this->ScaleFactor = 1.0;
  this->Range[0] = 0.0;
  this->Range[1] = 1.0;
  this->Orient = true;
  this->Clamping = false;
  this->SourceIndexing = false;
  this->OrientationMode = vtkKWEGlyph3DMapper::DIRECTION;

  // Set default arrays.
  this->SetScaleArray(vtkDataSetAttributes::SCALARS);
  this->SetMaskArray(vtkDataSetAttributes::SCALARS);
  this->SetOrientationArray(vtkDataSetAttributes::VECTORS);

  this->SourceMappers=0;

  this->DisplayListId=0; // for the matrices and color per glyph
  this->LastWindow = 0;

  this->NestedDisplayLists = true;

  this->Masking = false;
  this->SelectionColorId=1;

  this->ScalarsToColorsPainter = vtkScalarsToColorsPainter::New();
  this->PainterInformation = vtkInformation::New();
}

// ---------------------------------------------------------------------------
vtkKWEGlyph3DMapper::~vtkKWEGlyph3DMapper()
{
  if(this->SourceMappers!=0)
    {
    delete this->SourceMappers;
    this->SourceMappers=0;
    }

  if (this->LastWindow)
    {
    this->ReleaseGraphicsResources(this->LastWindow);
    this->LastWindow = 0;
    }
  if (this->ScalarsToColorsPainter)
    {
    this->ScalarsToColorsPainter->Delete();
    this->ScalarsToColorsPainter = 0;
    }
  this->PainterInformation->Delete();
  this->PainterInformation = 0;
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetMaskArray(int fieldAttributeType)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::MASK, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType);
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetMaskArray(const char* maskarrayname)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::MASK, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, maskarrayname);
}

// ---------------------------------------------------------------------------
vtkDataArray* vtkKWEGlyph3DMapper::GetMaskArray(vtkDataSet* input)
{
  if (this->Masking)
    {
    return this->GetInputArrayToProcess(vtkKWEGlyph3DMapper::MASK, input);
    }
  return 0;
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetOrientationArray(const char* orientationarrayname)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::ORIENTATION, 0, 0, 
    vtkDataObject::FIELD_ASSOCIATION_POINTS, orientationarrayname);
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetOrientationArray(int fieldAttributeType)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::ORIENTATION, 0, 0, 
    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType);
}

// ---------------------------------------------------------------------------
vtkDataArray* vtkKWEGlyph3DMapper::GetOrientationArray(vtkDataSet* input)
{
  if (this->Orient)
    {
    return this->GetInputArrayToProcess(vtkKWEGlyph3DMapper::ORIENTATION,
      input);
    }
  return false;
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetScaleArray(const char* scalarsarrayname)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::SCALE, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, scalarsarrayname);
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetScaleArray(int fieldAttributeType)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::SCALE, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType);
}

// ---------------------------------------------------------------------------
vtkDataArray* vtkKWEGlyph3DMapper::GetScaleArray(vtkDataSet* input)
{
  if (this->Scaling && this->ScaleMode != vtkKWEGlyph3DMapper::NO_DATA_SCALING)
    {
    vtkDataArray* arr = this->GetInputArrayToProcess(vtkKWEGlyph3DMapper::SCALE, input);
    return arr;
    }
  return 0;
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetSourceIndexArray(const char* arrayname)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::SOURCE_INDEX, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, arrayname);
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::SetSourceIndexArray(int fieldAttributeType)
{
  this->SetInputArrayToProcess(vtkKWEGlyph3DMapper::SOURCE_INDEX, 0, 0,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldAttributeType);
}

// ---------------------------------------------------------------------------
vtkDataArray* vtkKWEGlyph3DMapper::GetSourceIndexArray(vtkDataSet* input)
{
  if (this->SourceIndexing)
    {
    return this->GetInputArrayToProcess(
      vtkKWEGlyph3DMapper::SOURCE_INDEX, input);
    }
  return 0;
}

// ---------------------------------------------------------------------------
vtkUnsignedCharArray* vtkKWEGlyph3DMapper::GetColors(vtkDataSet* input)
{
  return vtkUnsignedCharArray::SafeDownCast(
    input->GetPointData()->GetScalars());
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::UpdatePainterInformation()
{
  if (this->GetMTime() < this->PainterUpdateTime)
    {
    return;
    }

  vtkInformation* info = this->PainterInformation;

  info->Set(vtkPainter::STATIC_DATA(), this->Static);
  info->Set(vtkScalarsToColorsPainter::USE_LOOKUP_TABLE_SCALAR_RANGE(),
    this->GetUseLookupTableScalarRange());
  info->Set(vtkScalarsToColorsPainter::SCALAR_RANGE(), 
    this->GetScalarRange(), 2);
  info->Set(vtkScalarsToColorsPainter::SCALAR_MODE(), this->GetScalarMode());
  info->Set(vtkScalarsToColorsPainter::COLOR_MODE(), this->GetColorMode());
  info->Set(vtkScalarsToColorsPainter::INTERPOLATE_SCALARS_BEFORE_MAPPING(),
    this->GetInterpolateScalarsBeforeMapping());
  info->Set(vtkScalarsToColorsPainter::LOOKUP_TABLE(), this->LookupTable);
  info->Set(vtkScalarsToColorsPainter::SCALAR_VISIBILITY(), 
    this->GetScalarVisibility());
  info->Set(vtkScalarsToColorsPainter::ARRAY_ACCESS_MODE(), 
    this->ArrayAccessMode);
  info->Set(vtkScalarsToColorsPainter::ARRAY_ID(), this->ArrayId);
  info->Set(vtkScalarsToColorsPainter::ARRAY_NAME(), this->ArrayName);
  info->Set(vtkScalarsToColorsPainter::ARRAY_COMPONENT(), this->ArrayComponent);
  info->Set(vtkScalarsToColorsPainter::SCALAR_MATERIAL_MODE(), 
    this->GetScalarMaterialMode());
  this->PainterUpdateTime.Modified();
}

// ---------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkKWEGlyph3DMapper::SetSourceConnection(int id,
  vtkAlgorithmOutput *algOutput)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(1);
  if (id < numConnections)
    {
    this->SetNthInputConnection(1, id, algOutput);
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(1, algOutput);
    }
  else if (algOutput)
    {
    vtkWarningMacro("The source id provided is larger than the maximum "
      "source id, using " << numConnections << " instead.");
    this->AddInputConnection(1, algOutput);
    }
}

// ---------------------------------------------------------------------------
// Specify a source object at a specified table location.
void vtkKWEGlyph3DMapper::SetSource(int id, vtkPolyData *pd)
{
  if (id < 0)
    {
    vtkErrorMacro("Bad index " << id << " for source.");
    return;
    }

  int numConnections = this->GetNumberOfInputConnections(1);
  vtkAlgorithmOutput *algOutput = 0;
  if (pd)
    {
    algOutput = pd->GetProducerPort();
    }
  else
    {
    vtkErrorMacro("Cannot set NULL source.");
    return;
    }

  if (id < numConnections)
    {
    if (algOutput)
      {
      this->SetNthInputConnection(1, id, algOutput);
      }
    }
  else if (id == numConnections && algOutput)
    {
    this->AddInputConnection(1, algOutput);
    }
}

// ---------------------------------------------------------------------------
// Description:
// Set the source to use for he glyph. Old style. See SetSourceConnection.
void vtkKWEGlyph3DMapper::SetSource(vtkPolyData *pd)
{
  this->SetSource(0,pd);
}

// ---------------------------------------------------------------------------
// Get a pointer to a source object at a specified table location.
vtkPolyData *vtkKWEGlyph3DMapper::GetSource(int id)
{
  if ( id < 0 || id >= this->GetNumberOfInputConnections(1) )
    {
    return NULL;
    }

  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, id));
}

// ---------------------------------------------------------------------------
vtkPolyData *vtkKWEGlyph3DMapper::GetSource(int idx,
  vtkInformationVector *sourceInfo)
{
  vtkInformation *info = sourceInfo->GetInformationObject(idx);
  if (!info)
    {
    return NULL;
    }
  return vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
}

// ---------------------------------------------------------------------------
const char* vtkKWEGlyph3DMapper::GetOrientationModeAsString()
{
  switch (this->OrientationMode)
    {
  case vtkKWEGlyph3DMapper::DIRECTION:
    return "Direction";
  case vtkKWEGlyph3DMapper::ORIENTATION:
    return "Orientation";
    }
  return "Invalid";
}

// ---------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->GetNumberOfInputConnections(1) < 2 )
    {
    if ( this->GetSource(0) != NULL )
      {
      os << indent << "Source: (" << this->GetSource(0) << ")\n";
      }
    else
      {
      os << indent << "Source: (none)\n";
      }
    }
  else
    {
    os << indent << "A table of " << this->GetNumberOfInputConnections(1) 
      << " glyphs has been defined\n";
    }

  os << indent << "Scaling: " << (this->Scaling ? "On\n" : "Off\n");

  os << indent << "Scale Mode: " << this->GetScaleModeAsString() << endl;
  os << indent << "Scale Factor: " << this->ScaleFactor << "\n";
  os << indent << "Clamping: " << (this->Clamping ? "On\n" : "Off\n");
  os << indent << "Range: (" << this->Range[0] << ", " << this->Range[1] << ")\n";
  os << indent << "Orient: " << (this->Orient ? "On\n" : "Off\n");
  os << indent << "OrientationMode: " 
    << this->GetOrientationModeAsString() << "\n";
  os << indent << "SourceIndexing: " 
    << (this->SourceIndexing? "On" : "Off") << endl;
  os << "Masking: " << (this->Masking? "On" : "Off") << endl;
}

// ---------------------------------------------------------------------------
  int vtkKWEGlyph3DMapper::RequestUpdateExtent(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  if (sourceInfo)
    {
    sourceInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(),
      0);
    sourceInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(),
      1);
    sourceInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(),
      0);
    }
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(),
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()));
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(),
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES()));
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(),
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS()));
  inInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);

  return 1;
}

// ---------------------------------------------------------------------------
int vtkKWEGlyph3DMapper::FillInputPortInformation(int port,
  vtkInformation *info)
{
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    return 1;
    }
  else if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
    }
  return 0;
}

// ---------------------------------------------------------------------------
// Description:
// Return the method of scaling as a descriptive character string.
const char *vtkKWEGlyph3DMapper::GetScaleModeAsString(void)
{
  if ( this->ScaleMode == SCALE_BY_MAGNITUDE)
    {
    return "ScaleByMagnitude";
    }
  else if ( this->ScaleMode == SCALE_BY_COMPONENTS)
    {
    return "ScaleByVectorComponents";
    }

  return "NoDataScaling";
}

// ---------------------------------------------------------------------------
// Description:
// Send mapper ivars to sub-mapper.
// \pre mapper_exists: mapper!=0
void vtkKWEGlyph3DMapper::CopyInformationToSubMapper(
  vtkPainterPolyDataMapper *mapper)
{
  assert("pre: mapper_exists" && mapper!=0);

  // see void vtkPainterPolyDataMapper::UpdatePainterInformation()

  mapper->SetStatic(this->Static);
  mapper->ScalarVisibilityOff();
  // not used
  mapper->SetClippingPlanes(this->ClippingPlanes);

  mapper->SetResolveCoincidentTopology(this->GetResolveCoincidentTopology());
  mapper->SetResolveCoincidentTopologyZShift(
    this->GetResolveCoincidentTopologyZShift());

  // ResolveCoincidentTopologyPolygonOffsetParameters is static
  mapper->SetResolveCoincidentTopologyPolygonOffsetFaces(
    this->GetResolveCoincidentTopologyPolygonOffsetFaces());
  mapper->SetImmediateModeRendering(this->ImmediateModeRendering);
}

// ---------------------------------------------------------------------------
// Description:
// Method initiates the mapping process. Generally sent by the actor 
// as each frame is rendered.
void vtkKWEGlyph3DMapper::Render(vtkRenderer *ren, vtkActor *actor)
{
  vtkHardwareSelector* selector = ren->GetSelector();
  bool selecting_points = selector && (selector->GetFieldAssociation() == 
    vtkDataObject::FIELD_ASSOCIATION_POINTS);

  if (selector)
    {
    selector->BeginRenderProp();
    }

  if (selector && !selecting_points)
    {
    // Selecting some other attribute. Not supported.
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
  bool immediateMode=this->ImmediateModeRendering ||
    vtkMapper::GetGlobalImmediateModeRendering() ||
    !this->NestedDisplayLists || (selecting_points);

  vtkDataSet *input=
    vtkDataSet::SafeDownCast(this->GetExecutive()->GetInputData(0,0));

  vtkProperty *prop=actor->GetProperty();

  bool createDisplayList=false;

  if (immediateMode)
    {
    this->ReleaseList();
    }
  else
    {
    // if something has changed, regenerate display lists.
    createDisplayList=this->DisplayListId==0 || 
      this->GetMTime() > this->BuildTime ||
      input->GetMTime() > this->BuildTime ||
      prop->GetMTime() > this->BuildTime ||
      ren->GetRenderWindow() != this->LastWindow.GetPointer();
    }

  if (immediateMode || createDisplayList)
    {
    cout << "Generate lists" << endl;
    int numberOfSources=this->GetNumberOfInputConnections(1);
    //  vtkPointData *pd=input->GetPointData();
    // scalars for scaling

    vtkDataArray* scaleArray = this->GetScaleArray(input);
    vtkDataArray* orientArray = this->GetOrientationArray(input);
    vtkDataArray* indexArray = this->GetSourceIndexArray(input);

    vtkIdType numPts = input->GetNumberOfPoints();
    if (numPts < 1)
      {
      vtkDebugMacro(<<"No points to glyph!");
      return;
      }

    vtkBitArray *maskArray = 0;
    if (this->Masking)
      {
      maskArray = vtkBitArray::SafeDownCast(this->GetMaskArray(input));
      if (maskArray==0)
        {
        vtkDebugMacro(<<"masking is enabled but there is no mask array. Ignore masking.");
        }
      else
        {
        if (maskArray->GetNumberOfComponents()!=1)
          {
          vtkErrorMacro(" expecting a mask array with one component, getting "
            << maskArray->GetNumberOfComponents() << " components.");
          return;
          }
        }
      }

    if (orientArray!=0 && orientArray->GetNumberOfComponents()!=3)
      {
      vtkErrorMacro(" expecting an orientation array with 3 component, getting "
        << orientArray->GetNumberOfComponents() << " components.");
      return;
      }

    // Check input for consistency
    //
    double den=this->Range[1]-this->Range[0];
    if (den==0.0)
      {
      den=1.0;
      }

    // Create a default source, if no source is specified.
    if (this->GetSource(0)==0)
      {
      vtkPolyData *defaultSource = vtkPolyData::New();
      defaultSource->Allocate();
      vtkPoints *defaultPoints = vtkPoints::New();
      defaultPoints->Allocate(6);
      defaultPoints->InsertNextPoint(0, 0, 0);
      defaultPoints->InsertNextPoint(1, 0, 0);
      vtkIdType defaultPointIds[2];
      defaultPointIds[0] = 0;
      defaultPointIds[1] = 1;
      defaultSource->SetPoints(defaultPoints);
      defaultSource->InsertNextCell(VTK_LINE, 2, defaultPointIds);
      defaultSource->SetUpdateExtent(0, 1, 0);
      this->SetSource(defaultSource);
      defaultSource->Delete();
      defaultSource = NULL;
      defaultPoints->Delete();
      defaultPoints = NULL;
      }

    vtkTransform *trans=vtkTransform::New();

    if(this->SourceMappers==0)
      {
      this->SourceMappers=new vtkKWEGlyph3DMapperArray;
      }
    if (indexArray)
      {
      this->SourceMappers->Mappers.resize(
        static_cast<size_t>(numberOfSources));
      }
    else
      {
      this->SourceMappers->Mappers.resize(1);
      }
    for (size_t cc=0; cc < this->SourceMappers->Mappers.size(); cc++)
      {
      vtkPolyData *s=this->GetSource(static_cast<int>(cc));
      // s can be null.
      if (this->SourceMappers->Mappers[cc]==0)
        {
        this->SourceMappers->Mappers[cc]=vtkPainterPolyDataMapper::New();
        this->SourceMappers->Mappers[cc]->Delete();
        vtkDefaultPainter *p=
          static_cast<vtkDefaultPainter *>(this->SourceMappers->Mappers[cc]->GetPainter());
        p->SetScalarsToColorsPainter(0); // bypass default mapping.
        p->SetClipPlanesPainter(0); // bypass default mapping.
        vtkHardwareSelectionPolyDataPainter::SafeDownCast(
          this->SourceMappers->Mappers[cc]->GetSelectionPainter())->EnableSelectionOff();
          // use the same painter for selection pass as well.
        }
      // Copy mapper ivar to sub-mapper
      this->CopyInformationToSubMapper(this->SourceMappers->Mappers[cc]);

      vtkPolyData *ss=this->SourceMappers->Mappers[cc]->GetInput();
      if (ss==0)
        {
        ss = vtkPolyData::New();
        this->SourceMappers->Mappers[cc]->SetInput(ss);
        ss->Delete();
        ss->ShallowCopy(s);
        }

      if (s->GetMTime()>ss->GetMTime())
        {
        ss->ShallowCopy(s);
        }

      if (createDisplayList)
        {
        this->SourceMappers->Mappers[cc]->SetForceCompileOnly(1);
        this->SourceMappers->Mappers[cc]->Render(ren,actor); // compile display list.
        this->SourceMappers->Mappers[cc]->SetForceCompileOnly(0);
        }
      }

    if(createDisplayList)
      {
      this->ReleaseList();
      this->DisplayListId=glGenLists(1);
      glNewList(this->DisplayListId,GL_COMPILE);
      }

    this->UpdatePainterInformation();
    this->ScalarsToColorsPainter->SetInput(input);
    this->ScalarsToColorsPainter->Render(ren, actor, 0xff, false);
    vtkUnsignedCharArray* colors = this->GetColors(
      vtkDataSet::SafeDownCast(this->ScalarsToColorsPainter->GetOutput()));
    bool multiplyWithAlpha = 
      this->ScalarsToColorsPainter->GetPremultiplyColorsWithAlpha(actor)==1;
    if (multiplyWithAlpha)
      {
      // We colors were premultiplied by alpha then we change the blending
      // function to one that will compute correct blended destination alpha
      // value, otherwise we stick with the default.
      // save the blend function.
      glPushAttrib(GL_COLOR_BUFFER_BIT);
      // the following function is not correct with textures because there
      // are not premultiplied by alpha.
      glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
      }

    // Traverse all Input points, transforming Source points
    vtkIdType inPtId;

    for (inPtId=0; inPtId < numPts; inPtId++)
      {
      if ( ! (inPtId % 10000) )
        {
        this->UpdateProgress (static_cast<double>(inPtId)/
                              static_cast<double>(numPts));
        if (this->GetAbortExecute())
          {
          break;
          }
        }

      if (maskArray && maskArray->GetValue(inPtId)==0)
        {
        continue;
        }

      double scalex = 1.0;
      double scaley = 1.0;
      double scalez = 1.0;
      // Get the scalar and vector data
      if (scaleArray)
        {
        double* tuple = scaleArray->GetTuple(inPtId);
        switch (this->ScaleMode)
          {
        case SCALE_BY_MAGNITUDE:
          scalex = scaley = scalez = vtkMath::Norm(tuple,
            scaleArray->GetNumberOfComponents());
          break;
        case SCALE_BY_COMPONENTS:
          if (scaleArray->GetNumberOfComponents() != 3)
            {
            vtkErrorMacro("Cannot scale by components since " << 
              scaleArray->GetName() << " does not have 3 components.");
            }
          else
            {
            scalex = tuple[0];
            scaley = tuple[1];
            scalez = tuple[2];
            }
          break;
        case NO_DATA_SCALING:
        default:
          break;
          }

        // Clamp data scale if enabled
        if (this->Clamping && this->ScaleMode != NO_DATA_SCALING)
          {
          scalex = (scalex < this->Range[0] ? this->Range[0] :
            (scalex > this->Range[1] ? this->Range[1] : scalex));
          scalex = (scalex - this->Range[0]) / den;
          scaley = (scaley < this->Range[0] ? this->Range[0] :
            (scaley > this->Range[1] ? this->Range[1] : scaley));
          scaley = (scaley - this->Range[0]) / den;
          scalez = (scalez < this->Range[0] ? this->Range[0] :
            (scalez > this->Range[1] ? this->Range[1] : scalez));
          scalez = (scalez - this->Range[0]) / den;
          }
        }
      scalex *= this->ScaleFactor;
      scaley *= this->ScaleFactor;
      scalez *= this->ScaleFactor;

      int index = 0;
      // Compute index into table of glyphs
      if (indexArray)
        {
        double value = vtkMath::Norm(indexArray->GetTuple(inPtId),
          indexArray->GetNumberOfComponents());
        index = static_cast<int>((value-this->Range[0])*numberOfSources/den);
        index = ::vtkClamp(index, 0, numberOfSources-1);
        }

      // source can be null.
      vtkPolyData *source=this->GetSource(index);

      // Make sure we're not indexing into empty glyph
      if (source!=0)
        {
        // Now begin copying/transforming glyph
        trans->Identity();

        // translate Source to Input point
        double x[3];
        input->GetPoint(inPtId, x);
        trans->Translate(x[0], x[1], x[2]);

        if (orientArray!=0)
          {
          double orientation[3];
          orientArray->GetTuple(inPtId, orientation);
          switch (this->OrientationMode)
            {
          case ROTATION:
            trans->RotateZ(orientation[2]);
            trans->RotateX(orientation[0]);
            trans->RotateY(orientation[1]);
            break;

          case DIRECTION:
            if (orientation[1] == 0.0 && orientation[2] == 0.0)
              {
              if (orientation[0] < 0) //just flip x if we need to
                {
                trans->RotateWXYZ(180.0,0,1,0);
                }
              }
            else
              {
              double vMag = vtkMath::Norm(orientation);
              double vNew[3];
              vNew[0] = (orientation[0]+vMag) / 2.0;
              vNew[1] = orientation[1] / 2.0;
              vNew[2] = orientation[2] / 2.0;
              trans->RotateWXYZ(180.0,vNew[0],vNew[1],vNew[2]);
              }
            break;
            }
          }

        // determine scale factor from scalars if appropriate
        // Copy scalar value
        if (selecting_points)
          {
          selector->RenderAttributeId(inPtId);
          }
        else if (colors)
          {
          unsigned char rgba[4];
          colors->GetTupleValue(inPtId, rgba);
          glColor4ub(rgba[0], rgba[1], rgba[2], rgba[3]);
          }
        //glFinish(); // for debug
        // scale data if appropriate
        if (this->Scaling)
          {
          if ( scalex == 0.0 )
            {
            scalex = 1.0e-10;
            }
          if ( scaley == 0.0 )
            {
            scaley = 1.0e-10;
            }
          if ( scalez == 0.0 )
            {
            scalez = 1.0e-10;
            }
          trans->Scale(scalex,scaley,scalez);
          }

        // multiply points and normals by resulting matrix
        // glFinish(); // for debug
        // glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        double *mat = trans->GetMatrix()->Element[0];
        float mat2[16]; // transpose for OpenGL, float is native OpenGL
        // format.
        mat2[0] = static_cast<float>(mat[0]);
        mat2[1] = static_cast<float>(mat[4]);
        mat2[2] = static_cast<float>(mat[8]);
        mat2[3] = static_cast<float>(mat[12]);
        mat2[4] = static_cast<float>(mat[1]);
        mat2[5] = static_cast<float>(mat[5]);
        mat2[6] = static_cast<float>(mat[9]);
        mat2[7] = static_cast<float>(mat[13]);
        mat2[8] = static_cast<float>(mat[2]);
        mat2[9] = static_cast<float>(mat[6]);
        mat2[10] = static_cast<float>(mat[10]);
        mat2[11] = static_cast<float>(mat[14]);
        mat2[12] = static_cast<float>(mat[3]);
        mat2[13] = static_cast<float>(mat[7]);
        mat2[14] = static_cast<float>(mat[11]);
        mat2[15] = static_cast<float>(mat[15]);
        glMultMatrixf(mat2);
        this->SourceMappers->Mappers[static_cast<size_t>(
                                       index)]->Render(ren,actor);
        // assume glMatrix(GL_MODELVIEW);
        // glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        // glFinish(); // for debug
        }
      }
    trans->Delete();

    // from vtkOpenGLScalarsToColorsPainter::RenderInternal
    if(multiplyWithAlpha)
      {
      // restore the blend function
      glPopAttrib();
      }

    if(createDisplayList)
      {
      glEndList();
      this->BuildTime.Modified();
      this->LastWindow = ren->GetRenderWindow();
      }

    } // if(immediateMode||createDisplayList)

  if(!immediateMode)
    {
    this->TimeToDraw=0.0;
    this->Timer->StartTimer();
    glCallList(this->DisplayListId);
    this->Timer->StopTimer();
    this->TimeToDraw += this->Timer->GetElapsedTime();
    }

  if (selector && !selecting_points)
    {
    // Selecting some other attribute. Not supported.
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  if (selector)
    {
    selector->EndRenderProp();
    }
}

//-------------------------------------------------------------------------
double *vtkKWEGlyph3DMapper::GetBounds()
{
  //  static double bounds[] = {-1.0,1.0, -1.0,1.0, -1.0,1.0};

  // do we have an input
  if ( ! this->GetNumberOfInputConnections(0) )
    {
    vtkMath::UninitializeBounds(this->Bounds);
    return this->Bounds;
    }
  else
    {
    if (!this->Static)
      {
      // For proper clipping, this would be this->Piece,
      // this->NumberOfPieces.
      // But that removes all benefites of streaming.
      // Update everything as a hack for paraview streaming.
      // This should not affect anything else, because no one uses this.
      // It should also render just the same.
      // Just remove this lie if we no longer need streaming in paraview :)
      //this->GetInput()->SetUpdateExtent(0, 1, 0);
      //this->GetInput()->Update();

      // first get the bounds from the input
      this->Update();
      }

    vtkDataSet *input=this->GetInput();
    input->GetBounds(this->Bounds);

    // if the input is not conform to what the mapper expects (use vector
    // but no vector data), nothing will be mapped.
    // It make sense to return uninitialized bounds.

    vtkDataArray *scaleArray = this->GetScaleArray(input);
    vtkDataArray *orientArray = this->GetOrientationArray(input);
    // TODO:
    // 1. cumulative bbox of all the glyph
    // 2. scale it by scale factor and maximum scalar value (or vector mag)
    // 3. enlarge the input bbox half-way in each direction with the
    // glyphs bbox.


    double den=this->Range[1]-this->Range[0];
    if(den==0.0)
      {
      den=1.0;
      }

    if(this->GetSource(0)==0)
      {
      vtkPolyData *defaultSource = vtkPolyData::New();
      defaultSource->Allocate();
      vtkPoints *defaultPoints = vtkPoints::New();
      defaultPoints->Allocate(6);
      defaultPoints->InsertNextPoint(0, 0, 0);
      defaultPoints->InsertNextPoint(1, 0, 0);
      vtkIdType defaultPointIds[2];
      defaultPointIds[0] = 0;
      defaultPointIds[1] = 1;
      defaultSource->SetPoints(defaultPoints);
      defaultSource->InsertNextCell(VTK_LINE, 2, defaultPointIds);
      defaultSource->SetUpdateExtent(0, 1, 0);
      this->SetSource(defaultSource);
      defaultSource->Delete();
      defaultSource = NULL;
      defaultPoints->Delete();
      defaultPoints = NULL;
      }

    // FB

    // Compute indexRange.
    int indexRange[2] = {0, 0};
    int numberOfSources=this->GetNumberOfInputConnections(1);
    vtkDataArray *indexArray = this->GetSourceIndexArray(input);
    if (indexArray)
      {
      double range[2];
      indexArray->GetRange(range, -1);
      for (int i=0; i<2; i++)
        {
        indexRange[i]=static_cast<int>((range[i]-this->Range[0])*numberOfSources/den);
        indexRange[i] = ::vtkClamp(indexRange[i], 0, numberOfSources-1);
        }
      }

    vtkBoundingBox bbox; // empty

    double xScaleRange[2] = {1.0, 1.0};
    double yScaleRange[2] = {1.0, 1.0};
    double zScaleRange[2] = {1.0, 1.0};

    if (scaleArray)
      {
      switch(this->ScaleMode)
        {
      case SCALE_BY_MAGNITUDE:
        scaleArray->GetRange(xScaleRange,-1);
        yScaleRange[0]=xScaleRange[0];
        yScaleRange[1]=xScaleRange[1];
        zScaleRange[0]=xScaleRange[0];
        zScaleRange[1]=xScaleRange[1];
        break;

      case SCALE_BY_COMPONENTS:
        scaleArray->GetRange(xScaleRange,0);
        scaleArray->GetRange(yScaleRange,1);
        scaleArray->GetRange(zScaleRange,2);
        break;

      default:
        // NO_DATA_SCALING: do nothing, set variables to avoid warnings.
        break;
        }

      if (this->Clamping && this->ScaleMode != NO_DATA_SCALING)
        {
        xScaleRange[0]=vtkMath::ClampAndNormalizeValue(xScaleRange[0],
          this->Range);
        xScaleRange[1]=vtkMath::ClampAndNormalizeValue(xScaleRange[1],
          this->Range);
        yScaleRange[0]=vtkMath::ClampAndNormalizeValue(yScaleRange[0],
          this->Range);
        yScaleRange[1]=vtkMath::ClampAndNormalizeValue(yScaleRange[1],
          this->Range);
        zScaleRange[0]=vtkMath::ClampAndNormalizeValue(zScaleRange[0],
          this->Range);
        zScaleRange[1]=vtkMath::ClampAndNormalizeValue(zScaleRange[1],
          this->Range);
        }
      }
    int index=indexRange[0];
    while(index<=indexRange[1])
      {
      vtkPolyData *source=this->GetSource(index);
      // Make sure we're not indexing into empty glyph
      if(source!=0)
        {
        double bounds[6];
        source->GetBounds(bounds);// can be invalid/uninitialized
        if(vtkMath::AreBoundsInitialized(bounds))
          {
          bbox.AddBounds(bounds);
          }
        }
      ++index;
      }

    if(this->Scaling)
      {
      vtkBoundingBox bbox2(bbox);
      bbox.Scale(xScaleRange[0],yScaleRange[0],zScaleRange[0]);
      bbox2.Scale(xScaleRange[1],yScaleRange[1],zScaleRange[1]);
      bbox.AddBox(bbox2);
      bbox.Scale(this->ScaleFactor,this->ScaleFactor,this->ScaleFactor);
      }

    if(bbox.IsValid())
      {
      double bounds[6];
      if (orientArray)
        {
        // bounding sphere.
        double c[3];
        bbox.GetCenter(c);
        double l=bbox.GetDiagonalLength()/2.0;
        bounds[0]=c[0]-l;
        bounds[1]=c[0]+l;
        bounds[2]=c[1]-l;
        bounds[3]=c[1]+l;
        bounds[4]=c[2]-l;
        bounds[5]=c[2]+l;
        }
      else
        {
        bbox.GetBounds(bounds);
        }
      int j=0;
      while(j<6)
        {
        this->Bounds[j]+=bounds[j];
        ++j;
        }
      }
    else
      {
      vtkMath::UninitializeBounds(this->Bounds);
      return this->Bounds;
      }
    return this->Bounds;
    }
}

//-------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::GetBounds(double bounds[6])
{
  this->Superclass::GetBounds(bounds);
}

// ---------------------------------------------------------------------------
// Description:
// Release any graphics resources that are being consumed by this mapper.
// The parameter window could be used to determine which graphic
// resources to release.
void vtkKWEGlyph3DMapper::ReleaseGraphicsResources(vtkWindow *window)
{
  if(this->SourceMappers!=0)
    {
    size_t c=this->SourceMappers->Mappers.size();
    size_t i=0;
    while(i<c)
      {
      this->SourceMappers->Mappers[i]->ReleaseGraphicsResources(window);
      ++i;
      }
    }
  this->ReleaseList();
}

// ---------------------------------------------------------------------------
// Description:
// Release display list used for matrices and color.
void vtkKWEGlyph3DMapper::ReleaseList()
{
  if(this->DisplayListId>0)
    {
    glDeleteLists(this->DisplayListId,1);
    this->DisplayListId = 0;
    }
}

//-----------------------------------------------------------------------------
void vtkKWEGlyph3DMapper::ReportReferences(vtkGarbageCollector *collector)
{
  this->Superclass::ReportReferences(collector);
  vtkGarbageCollectorReport(collector, this->ScalarsToColorsPainter, 
    "ScalarsToColorsPainter");
}
