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
// .NAME vtkKWEXMLParser parses XML files.
// .SECTION Description
// This is a subclass of vtkXMLParser that constructs a DOM representation
// of parsed XML using vtkKWEXMLElement.
#ifndef __vtkKWEXMLParser_h
#define __vtkKWEXMLParser_h

#include "vtkXMLParser.h"
#include "VTKEdgeConfigure.h" // include configuration header

class vtkKWEXMLElement;

class VTKEdge_IO_EXPORT vtkKWEXMLParser : public vtkXMLParser
{
public:
  vtkTypeRevisionMacro(vtkKWEXMLParser,vtkXMLParser);
  void PrintSelf(ostream& os, vtkIndent indent);
  static vtkKWEXMLParser* New();

  // Description:
  // Write the parsed XML into the output stream.
  void PrintXML(ostream& os);

  // Description:
  // Get the root element from the XML document.
  vtkKWEXMLElement* GetRootElement();

  // Description:
  // Get/Set the file from which to read the configuration.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

protected:
  vtkKWEXMLParser();
  ~vtkKWEXMLParser();

  void StartElement(const char* name, const char** atts);
  void EndElement(const char* name);
  void CharacterDataHandler(const char* data, int length);

  void AddElement(vtkKWEXMLElement* element);
  void PushOpenElement(vtkKWEXMLElement* element);
  vtkKWEXMLElement* PopOpenElement();

  // The root XML element.
  vtkKWEXMLElement* RootElement;

  // The stack of elements currently being parsed.
  vtkKWEXMLElement** OpenElements;
  unsigned int NumberOfOpenElements;
  unsigned int OpenElementsSize;

  // Counter to assign unique element ids to those that don't have any.
  unsigned int ElementIdIndex;

  // Called by Parse() to read the stream and call ParseBuffer.  Can
  // be replaced by subclasses to change how input is read.
  virtual int ParseXML();

private:
  vtkKWEXMLParser(const vtkKWEXMLParser&);  // Not implemented.
  void operator=(const vtkKWEXMLParser&);  // Not implemented.
};

#endif
