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

#define KWE_TESTING
#include "ObjectTreeDemo.cxx"
#undef KWE_TESTING

#include <vtksys/SystemTools.hxx>

int TestObjectTree(int argc, char *argv[])
{
  char* fname =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/ObjectTree.xml");
  vtkstd::string filename = fname;
  if (fname)
    {
    delete [] fname;
    }
  filename = "--input=" + filename;

  char** new_argv = new char*[argc+1];
  for (int cc=0; cc < argc; cc++)
    {
    new_argv[cc] = vtksys::SystemTools::DuplicateString(argv[cc]);
    }
  new_argv[argc++] = vtksys::SystemTools::DuplicateString(filename.c_str());
  int status = ::ObjectTreeDemo(argc, new_argv);
  for (int kk=0; kk < argc; kk++)
    {
    delete [] new_argv[kk];
    }
  delete [] new_argv;
  return status;
}
