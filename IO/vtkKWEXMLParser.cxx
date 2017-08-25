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
#include "vtkKWEXMLParser.h"
#include "vtkObjectFactory.h"
#include "vtkKWEXMLElement.h"
#include "vtksys/ios/sstream"

vtkCxxRevisionMacro(vtkKWEXMLParser, "$Revision: 651 $");
vtkStandardNewMacro(vtkKWEXMLParser);

//----------------------------------------------------------------------------
vtkKWEXMLParser::vtkKWEXMLParser()
{
  this->FileName = 0;
  this->InputString = 0;
  this->NumberOfOpenElements = 0;
  this->OpenElementsSize = 10;
  this->OpenElements = new vtkKWEXMLElement*[this->OpenElementsSize];
  this->ElementIdIndex = 0;
  this->RootElement = 0;
}

//----------------------------------------------------------------------------
vtkKWEXMLParser::~vtkKWEXMLParser()
{
  unsigned int i;
  for(i=0;i < this->NumberOfOpenElements;++i)
    {
    this->OpenElements[i]->Delete();
    }
  delete [] this->OpenElements;
  if(this->RootElement)
    {
    this->RootElement->Delete();
    }
  this->SetFileName(0);
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: " << (this->FileName? this->FileName : "(none)")
     << "\n";
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::StartElement(const char* name, const char** atts)
{
  vtkKWEXMLElement* element = vtkKWEXMLElement::New();
  element->SetName(name);
  element->ReadXMLAttributes(atts);
  const char* id = element->GetAttribute("id");
  if(id)
    {
    element->SetId(id);
    }
  else
    {
    vtksys_ios::ostringstream idstr;
    idstr << this->ElementIdIndex++ << ends;
    element->SetId(idstr.str().c_str());
    }
  this->PushOpenElement(element);
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::EndElement(const char* vtkNotUsed(name))
{
  vtkKWEXMLElement* finished = this->PopOpenElement();
  unsigned int numOpen = this->NumberOfOpenElements;
  if(numOpen > 0)
    {
    this->OpenElements[numOpen-1]->AddNestedElement(finished);
    finished->Delete();
    }
  else
    {
    this->RootElement = finished;
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::CharacterDataHandler(const char* data, int length)
{
  unsigned int numOpen = this->NumberOfOpenElements;
  if(numOpen > 0)
    {
    this->OpenElements[numOpen-1]->AddCharacterData(data, length);
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::PushOpenElement(vtkKWEXMLElement* element)
{
  if(this->NumberOfOpenElements == this->OpenElementsSize)
    {
    unsigned int newSize = this->OpenElementsSize*2;
    vtkKWEXMLElement** newOpenElements = new vtkKWEXMLElement*[newSize];
    unsigned int i;
    for(i=0; i < this->NumberOfOpenElements;++i)
      {
      newOpenElements[i] = this->OpenElements[i];
      }
    delete [] this->OpenElements;
    this->OpenElements = newOpenElements;
    this->OpenElementsSize = newSize;
    }

  unsigned int pos = this->NumberOfOpenElements++;
  this->OpenElements[pos] = element;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLParser::PopOpenElement()
{
  if(this->NumberOfOpenElements > 0)
    {
    --this->NumberOfOpenElements;
    return this->OpenElements[this->NumberOfOpenElements];
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkKWEXMLParser::PrintXML(ostream& os)
{
  this->RootElement->PrintXML(os, vtkIndent());
}

//----------------------------------------------------------------------------
int vtkKWEXMLParser::ParseXML()
{
  if (this->RootElement)
    {
    this->RootElement->Delete();
    this->RootElement = 0;
    }
  return this->Superclass::ParseXML();
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLParser::GetRootElement()
{
  return this->RootElement;
}
