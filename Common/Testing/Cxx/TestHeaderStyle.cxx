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

#include "vtkTestUtilities.h"
#include "VTKEdgeConfigure.h"
#include "vtkDirectory.h"

void CleanLine( char *line )
{
  while ( line[strlen(line)-1] == ' ' ||
          line[strlen(line)-1] == '\n' )
    {
    line[strlen(line)-1] = 0;
    }
}

int ProcessFile( const char *fileName, const char *newFileName )
{
//  cout << "  Processing file " << fileName << endl;

  int status = 0;

  FILE *fp = fopen(fileName, "r");

  FILE *newfp = fopen(newFileName, "w");


  if ( !fp )
    {
    cout << "Error opening file " << fileName << endl;
    return 1;
    }

  if ( !newfp )
    {
    cout << "Error opening file " << fileName << endl;
    return 1;
    }

  char line[256];

  const char *slashText[20] =
    {"//=============================================================================",
     "//   This file is part of VTKEdge. See vtkedge.org for more information.",
     "//",
     "//   Copyright (c) 2008 Kitware, Inc.",
     "//",
     "//   VTKEdge may be used under the terms of the GNU General Public License",
     "//   version 3 as published by the Free Software Foundation and appearing in",
     "//   the file LICENSE.txt included in the top level directory of this source",
     "//   code distribution. Alternatively you may (at your option) use any later",
     "//   version of the GNU General Public License if such license has been",
     "//   publicly approved by Kitware, Inc. (or its successors, if any).",
     "//",
     "//   VTKEdge is distributed \"AS IS\" with NO WARRANTY OF ANY KIND, INCLUDING",
     "//   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR",
     "//   PURPOSE. See LICENSE.txt for additional details.",
     "//",
     "//   VTKEdge is available under alternative license terms. Please visit",
     "//   vtkedge.org or contact us at kitware@kitware.com for further information.",
     "//",
     "//============================================================================="};

  const char *slashTextNew[20] =
    {"//=============================================================================",
     "//   This file is part of VTKEdge. See vtkedge.org for more information.",
     "//",
     "//   Copyright (c) 2010 Kitware, Inc.",
     "//",
     "//   VTKEdge may be used under the terms of the BSD License",
     "//   Please see the file Copyright.txt in the root directory of",
     "//   VTKEdge for further information.",
     "//",
     "//   Alternatively, you may see: ",
     "//",
     "//   http://www.vtkedge.org/vtkedge/project/license.html",
     "//",
     "//",
     "//   For custom extensions, consulting services, or training for",
     "//   this or any other Kitware supported open source project, please",
     "//   contact Kitware at sales@kitware.com.",
     "//",
     "//",
     "//============================================================================="};

    const char *hashText[20] =
    {"##=============================================================================",
     "##   This file is part of VTKEdge. See vtkedge.org for more information.",
     "##",
     "##   Copyright (c) 2008 Kitware, Inc.",
     "##",
     "##   VTKEdge may be used under the terms of the GNU General Public License",
     "##   version 3 as published by the Free Software Foundation and appearing in",
     "##   the file LICENSE.txt included in the top level directory of this source",
     "##   code distribution. Alternatively you may (at your option) use any later",
     "##   version of the GNU General Public License if such license has been",
     "##   publicly approved by Kitware, Inc. (or its successors, if any).",
     "##",
     "##   VTKEdge is distributed \"AS IS\" with NO WARRANTY OF ANY KIND, INCLUDING",
     "##   THE WARRANTIES OF DESIGN, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR",
     "##   PURPOSE. See LICENSE.txt for additional details.",
     "##",
     "##   VTKEdge is available under alternative license terms. Please visit",
     "##   vtkedge.org or contact us at kitware@kitware.com for further information.",
     "##",
     "##============================================================================="};

  const char *hashTextNew[20] =
    {"##=============================================================================",
     "##   This file is part of VTKEdge. See vtkedge.org for more information.",
     "##",
     "##   Copyright (c) 2010 Kitware, Inc.",
     "##",
     "##   VTKEdge may be used under the terms of the BSD License",
     "##   Please see the file Copyright.txt in the root directory of",
     "##   VTKEdge for further information.",
     "##",
     "##   Alternatively, you may see: ",
     "##",
     "##   http://www.vtkedge.org/vtkedge/project/license.html",
     "##",
     "##",
     "##   For custom extensions, consulting services, or training, please",
     "##   this or any other Kitware supported open source project, please",
     "##   contact Kitware at sales@kitware.com.",
     "##",
     "##",
     "##============================================================================="};

  char *result=0;

  int i = 0;
  while ( result=fgets( line, 255, fp ) )
    {
    CleanLine(line);

    if ( status==0 && i < 20 && line[0] == '/' )
      {
      fputs( slashTextNew[i], newfp );
      if ( strcmp(slashText[i], line) )
        {
        cout << "Error at line " << i << " in file " << fileName << endl;
        cout << "  correct:  " << strlen(slashText[i]) << " " << slashText[i] << endl;
        cout << "    found:  " << strlen(line) << " " << line << endl;
        status = 1;
        break;
        }
      }
    else if ( status == 0 && i < 20 && line[0] == '#')
      {
      fputs( hashTextNew[i], newfp );
      if ( strcmp(hashText[i], line) )
        {
        cout << "Error at line " << i << " in file " << fileName << endl;
        cout << "  correct:  " << strlen(hashText[i]) << " " << hashText[i] << endl;
        cout << "    found:  " << strlen(line) << " " << line << endl;
        status = 1;
        break;
        }
      }
    else if ( i < 20 )
      {
      status = 1;
      break;
      }
    else
      {
      fputs( line, newfp );
      }
    fputs( "\n", newfp );
    i++;
    }

  fclose(fp);
  fclose(newfp);


  return status;
}


int ProcessDirectory(const char *dirName)
{
  cout << "Processing directory " << dirName << endl;

  int numFailures = 0;

  vtkDirectory *dir = vtkDirectory::New();
  int opened = dir->Open(dirName);

  if (!opened)
    {
    cout << "Could not open directory: " << dirName << endl;
    dir->Delete();
    return 1;
    }

  vtkIdType numFiles = dir->GetNumberOfFiles();

  vtkIdType i;
  for ( i = 0; i < numFiles; i++ )
    {
    if (strcmp(dir->GetFile(i), ".") == 0 ||
        strcmp(dir->GetFile(i), "..") == 0 ||
        strcmp(dir->GetFile(i), ".svn") == 0)
      {
      continue;
      }

    if (dir->GetFile(i)[strlen(dir->GetFile(i))-1] == '~' )
      {
        continue;
      }

    char *fullName;
    size_t size = strlen(dir->GetFile(i)) + strlen(dirName) + 2;

    fullName = new char[size];
    sprintf( fullName, "%s/%s", dirName, dir->GetFile(i) );

    if (dir->FileIsDirectory(dir->GetFile(i)))
      {
      numFailures += ProcessDirectory(fullName);
      }
    else
      {
      char newFileName[512];
      sprintf(newFileName, "%s.new", fullName );

      int numFailuresBefore = numFailures;
      numFailures += ProcessFile(fullName, newFileName);
      if ( numFailuresBefore == numFailures )
        {
        cout << "Attempting the rename" << newFileName << " " << fullName << endl;
        remove(fullName);
        dir->Rename(newFileName, fullName);
        }
      }

    delete [] fullName;
    }

  dir->Delete();

  return numFailures;
}

int TestHeaderStyle(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  int numFailures = 0;

  numFailures = ProcessDirectory(VTKEdge_SOURCE_DIR);

  if ( numFailures > 0 )
    {
    cout << numFailures << " files failed header check." << endl;
    return 1;
    }

  return 0;
}
