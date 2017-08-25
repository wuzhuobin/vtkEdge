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
#include "vtkKWEProxyManagerExtension.h"
#include "vtkSMXML_VTKEdgeExtension.h"

#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkSmartPointer.h"

#include <vtkstd/map>
#include <vtkstd/string>

struct vtkKWEProxyManagerExtensionValue
{
  vtkSmartPointer<vtkPVXMLElement> XMLElement;
  bool Added;
  vtkKWEProxyManagerExtensionValue(vtkPVXMLElement* xml=0, bool added=false)
    {
    this->XMLElement = xml;
    this->Added = added;
    }
};

class vtkKWEProxyManagerExtension::vtkMapOfElements : 
  public vtkstd::map<vtkstd::string,  vtkKWEProxyManagerExtensionValue>
{
};

#define KWE_PROXY_MANAGER_EXTENSION_SEPARATOR "-->"

vtkStandardNewMacro(vtkKWEProxyManagerExtension);
vtkCxxRevisionMacro(vtkKWEProxyManagerExtension, "$Revision: 333 $");
//----------------------------------------------------------------------------
vtkKWEProxyManagerExtension::vtkKWEProxyManagerExtension()
{
  this->MapOfElements = new vtkMapOfElements();

  char* xml_to_parse = vtkSMvtkKWEExtensionGetString();
  vtkPVXMLParser* parser = vtkPVXMLParser::New();
  if (!parser->Parse(xml_to_parse))
    {
    vtkErrorMacro("Incorrect XML. Check parsing errors. "
      "Aborting for debugging purposes.");
    abort();
    }

  vtkPVXMLElement* root = parser->GetRootElement();

  unsigned int numElems = root->GetNumberOfNestedElements();
  for (unsigned int cc=0; cc < numElems; cc++)
    {
    vtkPVXMLElement* child = root->GetNestedElement(cc);
    if (child && child->GetName() && strcmp(child->GetName(), "Extension") == 0)
      {
      vtkstd::string key = child->GetAttribute("group");
      key += KWE_PROXY_MANAGER_EXTENSION_SEPARATOR;
      key += child->GetAttribute("name");
      (*this->MapOfElements)[key] = vtkKWEProxyManagerExtensionValue(child);
      }
    }
  parser->Delete();
}

//----------------------------------------------------------------------------
vtkKWEProxyManagerExtension::~vtkKWEProxyManagerExtension()
{
  delete this->MapOfElements;
  this->MapOfElements = 0;
}

//----------------------------------------------------------------------------
bool vtkKWEProxyManagerExtension::CheckCompatibility(
  int vtkNotUsed(major), int vtkNotUsed(minor), int vtkNotUsed(patch))
{
  return true;
}


//----------------------------------------------------------------------------
vtkPVXMLElement* vtkKWEProxyManagerExtension::GetProxyElement(const char* groupName,
  const char* proxyName, vtkPVXMLElement* currentElement)
{
  if (groupName && proxyName && currentElement)
    {
    vtkstd::string key = groupName;
    key += KWE_PROXY_MANAGER_EXTENSION_SEPARATOR;
    key += proxyName;


    vtkMapOfElements::iterator iter = this->MapOfElements->find(key);
    if (iter != this->MapOfElements->end())
      {
      vtkPVXMLElement* extElem = iter->second.XMLElement.GetPointer();
      if (!iter->second.Added)
        {
        iter->second.Added = true;
        unsigned int numElems = extElem->GetNumberOfNestedElements();
        for (unsigned int cc=0; cc < numElems; cc++)
          {
          currentElement->AddNestedElement(extElem->GetNestedElement(cc), /*setParent=*/0);
          }
        }
      }
    }

  return this->Superclass::GetProxyElement(groupName, proxyName, currentElement);
}

//----------------------------------------------------------------------------
void vtkKWEProxyManagerExtension::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


