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

#ifndef _vtkKWEITKFilterModuleBase_h
#define _vtkKWEITKFilterModuleBase_h

#include "VTKEdgeConfigure.h" // Needed for export symbols
#include "itkCommand.h"
#include "itkProcessObject.h"
#include "itkImageRegion.h"
#include "itkSize.h"
#include "itkIndex.h"
#include "vtkSetGet.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

class vtkImageData;
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


namespace vtkitk
{

class FilterModuleBase
{

public:
  // Command/Observer intended to update the progress
  typedef itk::MemberCommand< FilterModuleBase >  CommandType;

  // Type used by most algorithms. These types depend only on dimension.
  typedef itk::ImageRegion<3>     RegionType;
  typedef itk::Index<3>           IndexType;
  typedef itk::Size<3>            SizeType;

  /**  Constructor */
  FilterModuleBase()
    {
    this->m_CommandObserver    = CommandType::New();
    this->m_UpdateMessage      = "Processing the filter...";
    this->m_CommandObserver->SetCallbackFunction( this, &FilterModuleBase::ProgressUpdate );
    this->m_CumulatedProgress = 0.0;
    this->m_CurrentFilterProgressWeight = 1.0;
    this->m_ProcessComponentsIndependetly = true;
    this->m_InternalIterationCounter = 0;
    }


  /**  Destructor */
  virtual ~FilterModuleBase()
    {
    }

  void SetUpdateMessage( const char * message )
    {
    m_UpdateMessage = message;
    }

  void InitializeProgressValue()
    {
    this->m_CumulatedProgress = 0.0;
    }


  void SetCumulatedProgress( float progress )
    {
    this->m_CumulatedProgress = progress;
    }


  void SetCurrentFilterProgressWeight( float weight )
    {
    this->m_CurrentFilterProgressWeight = weight;
    }

  float GetCurrentFilterProgressWeight() const
    {
    return this->m_CurrentFilterProgressWeight;
    }


  void SetProcessComponentsIndependetly( bool independentProcessing )
    {
    this->m_ProcessComponentsIndependetly = independentProcessing;
    }

  CommandType * GetCommandObserver()
    {
    return this->m_CommandObserver;
    }

  void
  ProgressUpdate( itk::Object * itkNotUsed(caller),
                  const itk::EventObject & itkNotUsed(event) )
    {
    // TODO Invoke a vtk event for the corresponding itk event here....
    }

  virtual void SetInput( vtkImageData * itkNotUsed(data) ) = 0;
  virtual vtkImageData * GetInput() = 0;

  // Optional method for subclasses that output a stencil
  virtual int GetOutputAsStencil( vtkImageStencilData * itkNotUsed(data) )
             { return 0; };
  virtual int GetSphereBoundedOutputAsStencil( vtkImageStencilData *
                                                        itkNotUsed(data),
                                            int itkNotUsed(center)[3],
                                            double itkNotUsed(radius)[3] )
             { return 0; };

  // Set the requested region
  virtual void SetRequestedExtent( int itkNotUsed(extent)[6] ) {};

private:
    CommandType::Pointer         m_CommandObserver;
    std::string                  m_UpdateMessage;
    float                        m_CumulatedProgress;
    float                        m_CurrentFilterProgressWeight;
    bool                         m_ProcessComponentsIndependetly;
    unsigned int                 m_InternalIterationCounter;
};

} // end namespace vtkitk

#endif

