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


#define KWE_TESTING
#include "StructuredGridLIC2DDemo.cxx"
#undef KWE_TESTING

#include <vtksys/SystemTools.hxx>

int TestStructuredGridLIC2DYSlice(int argc, char* argv[])
{
  char* fname =  
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/timestep_0_15.vts");

  vtkstd::string filename = fname;
  filename = "--data=" + filename;

  fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/noise.png");
  vtkstd::string noise = fname;
  noise = "--noise=" + noise;

  char** new_argv = new char*[argc+10];
  for (int cc=0; cc < argc; cc++)
    {
    new_argv[cc] = vtksys::SystemTools::DuplicateString(argv[cc]);
    }
  new_argv[argc++] = vtksys::SystemTools::DuplicateString(filename.c_str());
  new_argv[argc++] = vtksys::SystemTools::DuplicateString(noise.c_str());
  new_argv[argc++] = vtksys::SystemTools::DuplicateString("--mag=8");
  new_argv[argc++] = vtksys::SystemTools::DuplicateString("--partitions=1");
  new_argv[argc++] = vtksys::SystemTools::DuplicateString("--num-steps=100");
  new_argv[argc++] = vtksys::SystemTools::DuplicateString("--slice-dir=1");
  new_argv[argc++] = vtksys::SystemTools::DuplicateString("--slice=0");
  int status = ::StructuredGridLIC2DDemo(argc, new_argv);
  for (int kk=0; kk < argc; kk++)
    {
    delete [] new_argv[kk];
    }
  delete [] new_argv;
  return status;
}

