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
// .NAME vtkKWEStructuredExtent - helper class to aid working with structured
// extents.
// .SECTION Description
// vtkKWEStructuredExtent is an helper class that helps in arithmetic with
// structured extents. It defines a bunch of static methods (most of which are
// inlined) to aid in dealing with extents.

#ifndef __vtkKWEStructuredExtent_h
#define __vtkKWEStructuredExtent_h

#include "vtkObject.h"
#include "VTKEdgeConfigure.h" // include configuration header

class VTKEdge_HYBRID_EXPORT vtkKWEStructuredExtent : public vtkObject
{
public:
  static vtkKWEStructuredExtent* New();
  vtkTypeRevisionMacro(vtkKWEStructuredExtent, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Clamps \c ext to fit in \c wholeExt.
  static void Clamp(int ext[6], int wholeExt[6]);

  // Description:
  // Returns true if \c ext is fits within \c wholeExt with atleast 1 dimension
  // smaller than the \c wholeExt.
  static bool StrictlySmaller(const int ext[6], const int wholeExt[6]);

  // Description:
  // Returns if \c ext fits within \c wholeExt. Unline StrictlySmaller, this
  // method returns true even if \c ext == \c wholeExt.
  static bool Smaller(const int ext[6], const int wholeExt[6]);

  // Description:
  // Grows the \c ext on each side by the given \c count.
  static void Grow(int ext[6], int count);

  // Description:
  // Makes \c ext relative to \c wholeExt.
  static void Transform(int ext[6], int wholeExt[6]);

  // Description:
  // Given the extents, computes the dimensions.
  static void GetDimensions(const int ext[6], int dims[3]);

//BTX
protected:
  vtkKWEStructuredExtent();
  ~vtkKWEStructuredExtent();

private:
  vtkKWEStructuredExtent(const vtkKWEStructuredExtent&); // Not implemented.
  void operator=(const vtkKWEStructuredExtent&); // Not implemented.
//ETX
};

//----------------------------------------------------------------------------
inline void vtkKWEStructuredExtent::Clamp(int ext[6], int wholeExt[6])
{
  ext[0] = (ext[0] < wholeExt[0])? wholeExt[0] : ext[0];
  ext[1] = (ext[1] > wholeExt[1])? wholeExt[1] : ext[1];

  ext[2] = (ext[2] < wholeExt[2])? wholeExt[2] : ext[2];
  ext[3] = (ext[3] > wholeExt[3])? wholeExt[3] : ext[3];

  ext[4] = (ext[4] < wholeExt[4])? wholeExt[4] : ext[4];
  ext[5] = (ext[5] > wholeExt[5])? wholeExt[5] : ext[5];
}

//----------------------------------------------------------------------------
inline bool vtkKWEStructuredExtent::Smaller(const int ext[6], const int wholeExt[6])
{
  if (ext[0]   < wholeExt[0] || ext[0]     > wholeExt[0 + 1] ||
    ext[0 + 1] < wholeExt[0] || ext[0 + 1] > wholeExt[0 + 1])
    {
    return false;
    }

  if (ext[2]   < wholeExt[2] || ext[2]     > wholeExt[2 + 1] ||
    ext[2 + 1] < wholeExt[2] || ext[2 + 1] > wholeExt[2 + 1])
    {
    return false;
    }

  if (ext[4]   < wholeExt[4] || ext[4]     > wholeExt[4 + 1] ||
    ext[4 + 1] < wholeExt[4] || ext[4 + 1] > wholeExt[4 + 1])
    {
    return false;
    }

  return true;
}

//----------------------------------------------------------------------------
inline bool vtkKWEStructuredExtent::StrictlySmaller(const int ext[6], const int wholeExt[6])
{
  if (!vtkKWEStructuredExtent::Smaller(ext, wholeExt))
    {
    return false;
    }

  if (ext[0] > wholeExt[0] || ext[1] < wholeExt[1] ||
    ext[2] > wholeExt[2] || ext[3] < wholeExt[3] ||
    ext[4] > wholeExt[4] || ext[5] < wholeExt[5])
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
inline void vtkKWEStructuredExtent::Grow(int ext[6], int count)
{
  ext[0] -= count;
  ext[2] -= count;
  ext[4] -= count;

  ext[1] += count;
  ext[3] += count;
  ext[5] += count;
}

//----------------------------------------------------------------------------
inline void vtkKWEStructuredExtent::Transform(int ext[6], int wholeExt[6])
{
  ext[0] -= wholeExt[0];
  ext[1] -= wholeExt[0];

  ext[2] -= wholeExt[2];
  ext[3] -= wholeExt[2];

  ext[4] -= wholeExt[4];
  ext[5] -= wholeExt[4];
}

//----------------------------------------------------------------------------
inline void vtkKWEStructuredExtent::GetDimensions(const int ext[6], int dims[3])
{
  dims[0] = ext[1]-ext[0] + 1;
  dims[1] = ext[3]-ext[2] + 1;
  dims[2] = ext[5]-ext[4] + 1;
}

#endif


