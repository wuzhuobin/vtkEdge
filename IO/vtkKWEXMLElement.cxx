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
#include "vtkKWEXMLElement.h"

#include "vtkCollection.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkKWEXMLElement, "$Revision: 1774 $");
vtkStandardNewMacro(vtkKWEXMLElement);

#include <vtkstd/string>
#include <vtkstd/vector>
#include <vtksys/ios/sstream>

#if defined(_WIN32) && !defined(__CYGWIN__)
# define SNPRINTF _snprintf
#else
# define SNPRINTF snprintf
#endif

struct vtkKWEXMLElementInternals
{
  vtkstd::vector<vtkstd::string> AttributeNames;
  vtkstd::vector<vtkstd::string> AttributeValues;
  typedef vtkstd::vector<vtkSmartPointer<vtkKWEXMLElement> > VectorOfElements;
  VectorOfElements NestedElements;
  vtkstd::string CharacterData;
};

//----------------------------------------------------------------------------
vtkKWEXMLElement::vtkKWEXMLElement()
{
  this->Name = 0;
  this->Id = 0;
  this->Parent = 0;

  this->Internal = new vtkKWEXMLElementInternals;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement::~vtkKWEXMLElement()
{
  this->SetName(0);
  this->SetId(0);

  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Id: " << (this->Id?this->Id:"<none>") << endl;
  os << indent << "Name: " << (this->Name?this->Name:"<none>") << endl;
  unsigned int numNested = this->GetNumberOfNestedElements();
  for (unsigned int i=0; i< numNested; i++)
    {
    if (this->GetNestedElement(i))
      {
      this->GetNestedElement(i)->PrintSelf(os, indent.GetNextIndent());
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName,
                                   unsigned int attrValue)
{
  vtksys_ios::ostringstream valueStr;
  valueStr << attrValue << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName, int attrValue)
{
  vtksys_ios::ostringstream valueStr;
  valueStr << attrValue << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName, unsigned long attrValue)
{
  vtksys_ios::ostringstream valueStr;
  valueStr << attrValue << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

#if defined(VTK_USE_64BIT_IDS)
//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName, vtkIdType attrValue)
{
  vtksys_ios::ostringstream valueStr;
  valueStr << attrValue << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}
#endif

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName, double attrValue)
{
  vtksys_ios::ostringstream valueStr;
  valueStr << attrValue << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName,
                                   const char* attrValue)
{
  if (!attrName || !attrValue)
    {
    return;
    }

  this->Internal->AttributeNames.push_back(attrName);
  this->Internal->AttributeValues.push_back(attrValue);
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName,
                                    double* vals,
                                    unsigned int length)
{
  if (!attrName || !vals || length == 0)
    {
    return;
    }

  vtksys_ios::ostringstream valueStr;
  for(unsigned int i=0; i<length; i++)
    {
    valueStr << vals[i];

    if (i < (length-1)) {valueStr << " ";}
    }
  valueStr << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char *attrName,
                                    int *vals,
                                    unsigned int length)
{
  if (!attrName || !vals || length == 0)
    {
    return;
    }

  vtksys_ios::ostringstream valueStr;
  for(unsigned int i=0; i<length; i++)
    {
    valueStr << vals[i] << " ";
    }
  valueStr << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName,
                                    unsigned long* vals,
                                    unsigned int length)
{
  if (!attrName || !vals || length == 0)
    {
    return;
    }

  vtksys_ios::ostringstream valueStr;
  for(unsigned int i=0; i<length; i++)
    {
    valueStr << vals[i] << " ";
    }
  valueStr << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}

#if defined(VTK_USE_64BIT_IDS)
//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddAttribute(const char* attrName,
                                    vtkIdType* vals,
                                    unsigned int length)
{
  if (!attrName || !vals || length == 0)
    {
    return;
    }

  vtksys_ios::ostringstream valueStr;
  for(unsigned int i=0; i<length; i++)
    {
    valueStr << vals[i] << " ";
    }
  valueStr << ends;
  this->AddAttribute(attrName, valueStr.str().c_str());
}
#endif

//----------------------------------------------------------------------------
void vtkKWEXMLElement::SetAttribute(const char *attrName,
                                    const char *attrValue)
{
  if (!attrName || !attrValue)
    {
    return;
    }

  // iterate over the names, and find if the attribute name exists.
  size_t numAttributes = this->Internal->AttributeNames.size();
  size_t i;
  for(i=0; i < numAttributes; ++i)
    {
    if(strcmp(this->Internal->AttributeNames[i].c_str(), attrName) == 0)
      {
      this->Internal->AttributeValues[i] = attrValue;
      return;
      }
    }
  // add the attribute.
  this->AddAttribute(attrName, attrValue);
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::ReadXMLAttributes(const char** atts)
{
  this->Internal->AttributeNames.clear();
  this->Internal->AttributeValues.clear();

  if(atts)
    {
    const char** attsIter = atts;
    unsigned int count=0;
    while(*attsIter++) { ++count; }
    unsigned int numberOfAttributes = count/2;

    unsigned int i;
    for(i=0;i < numberOfAttributes; ++i)
      {
      this->AddAttribute(atts[i*2], atts[i*2+1]);
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::RemoveAllNestedElements()
{
  this->Internal->NestedElements.clear();
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::RemoveNestedElement(vtkKWEXMLElement* element)
{
  vtkstd::vector<vtkSmartPointer<vtkKWEXMLElement> >::iterator iter
    = this->Internal->NestedElements.begin();
  for ( ; iter != this->Internal->NestedElements.end(); ++iter)
    {
    if (iter->GetPointer() == element)
      {
      this->Internal->NestedElements.erase(iter);
      break;
      }
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddNestedElement(vtkKWEXMLElement* element)
{
  this->AddNestedElement(element, 1);
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddNestedElement(vtkKWEXMLElement* element, int setParent)
{
  if (setParent)
    {
    element->SetParent(this);
    }
  this->Internal->NestedElements.push_back(element);
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::AddCharacterData(const char* data, int length)
{
  this->Internal->CharacterData.append(data, length);
}

//----------------------------------------------------------------------------
const char *vtkKWEXMLElement::GetAttribute(const char *name)
{
  size_t numAttributes = this->Internal->AttributeNames.size();
  size_t i;
  for(i=0; i < numAttributes; ++i)
    {
    if(strcmp(this->Internal->AttributeNames[i].c_str(), name) == 0)
      {
      return this->Internal->AttributeValues[i].c_str();
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
const char* vtkKWEXMLElement::GetCharacterData()
{
  return this->Internal->CharacterData.c_str();
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::PrintXML()
{
  this->PrintXML(cout, vtkIndent());
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::PrintXML(ostream& os, vtkIndent indent)
{
  os << indent << "<" << (this->Name?this->Name:"NoName");
  size_t numAttributes = this->Internal->AttributeNames.size();
  size_t i;
  for(i=0;i < numAttributes; ++i)
    {
    const char* aName = this->Internal->AttributeNames[i].c_str();
    const char* aValue = this->Internal->AttributeValues[i].c_str();

    // we always print the encoded value. The expat parser processes encoded
    // values when reading them, hence we don't need any decoding when reading
    // the values back.
    const vtkStdString& sanitizedValue = vtkKWEXMLElement::Encode(aValue);
    os << " " << (aName?aName:"NoName")
       << "=\"" << (aValue?sanitizedValue.c_str():"NoValue") << "\"";
    }
  size_t numberOfNestedElements = this->Internal->NestedElements.size();
  if(numberOfNestedElements > 0)
    {
    os << ">\n";
    for(i=0;i < numberOfNestedElements;++i)
      {
      vtkIndent nextIndent = indent.GetNextIndent();
      this->Internal->NestedElements[i]->PrintXML(os, nextIndent);
      }
    os << indent << "</" << (this->Name?this->Name:"NoName") << ">\n";
    }
  else
    {
    os << "/>\n";
    }
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::SetParent(vtkKWEXMLElement* parent)
{
  this->Parent = parent;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::GetParent()
{
  return this->Parent;
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetNumberOfNestedElements()
{
  return static_cast<unsigned int>(this->Internal->NestedElements.size());
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::GetNestedElement(unsigned int index)
{
  if(static_cast<size_t>(index) < this->Internal->NestedElements.size())
    {
    return this->Internal->NestedElements[index];
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::LookupElement(const char* id)
{
  return this->LookupElementUpScope(id);
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::FindNestedElement(const char* id)
{
  size_t numberOfNestedElements = this->Internal->NestedElements.size();
  size_t i;
  for(i=0;i < numberOfNestedElements;++i)
    {
    const char* nid = this->Internal->NestedElements[i]->GetId();
    if(nid && strcmp(nid, id) == 0)
      {
      return this->Internal->NestedElements[i];
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::FindNestedElementByName(const char* name)
{
  vtkKWEXMLElementInternals::VectorOfElements::iterator iter =
    this->Internal->NestedElements.begin();
  for(; iter != this->Internal->NestedElements.end(); ++iter)
    {
    const char* cur_name = (*iter)->GetName();
    if(name && cur_name && strcmp(cur_name, name) == 0)
      {
      return (*iter);
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::LookupElementInScope(const char* id)
{
  // Pull off the first qualifier.
  const char* end = id;
  while(*end && (*end != '.')) ++end;
  size_t len = end - id;
  char* name = new char[len+1];
  strncpy(name, id, len);
  name[len] = '\0';

  // Find the qualifier in this scope.
  vtkKWEXMLElement* next = this->FindNestedElement(name);
  if(next && (*end == '.'))
    {
    // Lookup rest of qualifiers in nested scope.
    next = next->LookupElementInScope(end+1);
    }

  delete [] name;
  return next;
}

//----------------------------------------------------------------------------
vtkKWEXMLElement* vtkKWEXMLElement::LookupElementUpScope(const char* id)
{
  // Pull off the first qualifier.
  const char* end = id;
  while(*end && (*end != '.')) ++end;
  size_t len = end - id;
  char* name = new char[len+1];
  strncpy(name, id, len);
  name[len] = '\0';

  // Find most closely nested occurrence of first qualifier.
  vtkKWEXMLElement* curScope = this;
  vtkKWEXMLElement* start = 0;
  while(curScope && !start)
    {
    start = curScope->FindNestedElement(name);
    curScope = curScope->GetParent();
    }
  if(start && (*end == '.'))
    {
    start = start->LookupElementInScope(end+1);
    }

  delete [] name;
  return start;
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  int* value)
{
  return this->GetVectorAttribute(name, 1, value);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  unsigned int* value)
{
  return this->GetVectorAttribute(name, 1, value);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  unsigned long* value)
{
  return this->GetVectorAttribute(name, 1, value);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  float* value)
{
  return this->GetVectorAttribute(name, 1, value);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  double* value)
{
  return this->GetVectorAttribute(name, 1, value);
}

#if defined(VTK_USE_64BIT_IDS)
//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetScalarAttribute(const char* name,
                                                  vtkIdType* value)
{
  return this->GetVectorAttribute(name, 1, value);
}
#endif

//----------------------------------------------------------------------------
template <class T>
unsigned int vtkKWEXMLVectorAttributeParse(const char* str,
                                           unsigned int length,
                                           T* data)
{
  if(!str || !length) { return 0; }
  vtksys_ios::stringstream vstr;
  vstr << str << ends;
  for(unsigned int i=0; i < length; ++i)
    {
    vstr >> data[i];
    if(!vstr) { return i; }
    }
  return length;
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  int* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  unsigned int* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  unsigned long* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  float* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  double* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}

#if defined(VTK_USE_64BIT_IDS)
//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetVectorAttribute(const char* name,
                                                  unsigned int length,
                                                  vtkIdType* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetAttribute(name), length, data);
}
#endif

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetCharacterDataAsVector(unsigned int length,
                                                        int* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetCharacterData(), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetCharacterDataAsVector(unsigned int length,
                                                        float* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetCharacterData(), length, data);
}

//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetCharacterDataAsVector(unsigned int length,
                                                        double* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetCharacterData(), length, data);
}

//----------------------------------------------------------------------------
void vtkKWEXMLElement::GetElementsByName(const char* name, vtkCollection* elements)
{
  if (!elements)
    {
    vtkErrorMacro("elements cannot be NULL.");
    return;
    }
  if (!name)
    {
    vtkErrorMacro("name cannot be NULL.");
    return;
    }

  unsigned int numChildren = this->GetNumberOfNestedElements();
  unsigned int cc;
  for (cc=0; cc < numChildren; cc++)
    {
    vtkKWEXMLElement* child = this->GetNestedElement(cc);
    if (child && child->GetName() && strcmp(child->GetName(), name) == 0)
      {
      elements->AddItem(child);
      }
    }

  for (cc=0; cc < numChildren; cc++)
    {
    vtkKWEXMLElement* child = this->GetNestedElement(cc);
    if (child)
      {
      child->GetElementsByName(name, elements);
      }
    }
}


//----------------------------------------------------------------------------
vtkStdString vtkKWEXMLElement::Encode(const char* plaintext)
{
  //escape any characters that are not allowed in XML
  vtkStdString sanitized = "";
  if (!plaintext)
    {
    return sanitized;
    }

  const char toescape[] = { '&', '\'', '<', '>', '\"', '\r', '\n', '\t', 0};

  size_t pt_length = strlen(plaintext);
  for (size_t cc = 0; cc < pt_length; cc++)
    {
    const char* escape_char = toescape;
    for ( ; *escape_char != 0; escape_char++)
      {
      if (plaintext[cc] == *escape_char)
        {
        break;
        }
      }

    if (*escape_char)
      {
      char temp[20];
      SNPRINTF(temp, 20, "&#x%x;", static_cast<int>(*escape_char));
      sanitized += temp;
      }
    else
      {
      sanitized += plaintext[cc];
      }
    }

  return sanitized;
}


#if defined(VTK_USE_64BIT_IDS)
//----------------------------------------------------------------------------
unsigned int vtkKWEXMLElement::GetCharacterDataAsVector(unsigned int length,
                                                        vtkIdType* data)
{
  return vtkKWEXMLVectorAttributeParse(this->GetCharacterData(), length, data);
}
#endif

void vtkKWEXMLElement::Merge(vtkKWEXMLElement* element, const char* attributeName)
{
  if(!element || 0 != strcmp(this->GetName(), element->GetName()))
    {
    return;
    }
  if(attributeName)
    {
    const char* attr1 = this->GetAttribute(attributeName);
    const char* attr2 = element->GetAttribute(attributeName);
    if(attr1 && attr2 && 0 != strcmp(attr1, attr2))
      {
      return;
      }
    }

  // override character data if there is some
  if(!element->Internal->CharacterData.empty())
    {
    this->Internal->CharacterData = element->Internal->CharacterData;
    }

  // add attributes from element to this, or override attribute values on this
  size_t numAttributes = element->Internal->AttributeNames.size();
  size_t numAttributes2 = this->Internal->AttributeNames.size();

  for(size_t i=0; i < numAttributes; ++i)
    {
    bool found = false;
    for(size_t j=0; !found && j < numAttributes2; ++j)
      {
      if(element->Internal->AttributeNames[i] ==
        this->Internal->AttributeNames[j])
        {
        this->Internal->AttributeValues[j] =
          element->Internal->AttributeValues[i];
        found = true;
        }
      }
    // if not found, add it
    if(!found)
      {
      this->AddAttribute(element->Internal->AttributeNames[i].c_str(),
                         element->Internal->AttributeValues[i].c_str());
      }
    }

  // now recursively merge the children with the same names

  vtkKWEXMLElementInternals::VectorOfElements::iterator iter;
  vtkKWEXMLElementInternals::VectorOfElements::iterator iter2;

  for(iter = element->Internal->NestedElements.begin();
      iter != element->Internal->NestedElements.end(); ++iter)
    {
    bool found = false;
    for(iter2 = this->Internal->NestedElements.begin();
        iter2 != this->Internal->NestedElements.end(); ++iter2)
      {
      const char* attr1 = attributeName ? this->GetAttribute(attributeName) : NULL;
      const char* attr2 = attributeName ? element->GetAttribute(attributeName) : NULL;
      if(0 == strcmp((*iter)->Name, (*iter2)->Name) &&
        (!attributeName || (!attr1 || !attr2 || 0 == strcmp(attr1, attr2))))
        {
        (*iter2)->Merge(*iter, attributeName);
        found = true;
        }
      }
    // if not found, add it
    if(!found)
      {
      vtkSmartPointer<vtkKWEXMLElement> newElement =
        vtkSmartPointer<vtkKWEXMLElement>::New();
      newElement->SetName((*iter)->GetName());
      newElement->SetId((*iter)->GetId());
      newElement->Internal->AttributeNames = (*iter)->Internal->AttributeNames;
      newElement->Internal->AttributeValues = (*iter)->Internal->AttributeValues;
      this->AddNestedElement(newElement);
      newElement->Merge(*iter, attributeName);
      }
    }
}

