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
