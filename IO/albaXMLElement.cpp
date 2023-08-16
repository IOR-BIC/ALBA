/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaXMLElement
 Authors: Marco Petrone m.petrone@cineca.it
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mmuXMLDOMElement.h"
#include "mmuXMLDOM.h"
#include "albaXMLElement.h"

#include "albaXMLStorage.h"
#include "albaXMLString.h"

#include "albaMatrix.h"
#include "albaStorable.h"
#include "albaObjectFactory.h"

#include <vector>
#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
albaXMLElement::albaXMLElement(mmuXMLDOMElement *element,albaXMLElement *parent,albaXMLStorage *storage) :
  albaStorageElement(parent,storage)
//------------------------------------------------------------------------------
{
  assert(storage);
  assert(element);
  assert(element->m_XMLElement);
  m_DOMElement = element;
  m_Name = new albaXMLString(element->m_XMLElement->getTagName());
}

//------------------------------------------------------------------------------
albaXMLElement::~albaXMLElement()
//------------------------------------------------------------------------------
{
  // the XML element is destroyed by its creator (the DOMDocument)
  cppDEL(m_DOMElement);
  cppDEL(m_Name);
}

//------------------------------------------------------------------------------
template <class T>
int InternalParseData(const char *text,T *vector,int size)
//------------------------------------------------------------------------------
{
  std::istringstream instr(text);

  for (int i=0;i<size;i++)
  {
    if (instr.eof())
      return i;

    instr>>vector[i];
  }

  return size;
}


//------------------------------------------------------------------------------
template <class T>
int InternalParseData(const char *text,std::vector<T> &vector,int size)
//------------------------------------------------------------------------------
{
  std::istringstream instr(text);

	vector.resize(size);

  for (int i=0;i<size;i++)
  {
    if (instr.eof())
      return i;

    T tmp;
    instr>>tmp;
    vector[i]=tmp;
  }

  return size;
}

//------------------------------------------------------------------------------
int albaXMLElement::ParseData(std::vector<double> &vector,int size)
//------------------------------------------------------------------------------
{
  albaXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
  
  return 0;
}
//------------------------------------------------------------------------------
int albaXMLElement::ParseData(std::vector<int> &vector,int size)
//------------------------------------------------------------------------------
{
  albaXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
 
  return 0;
}

//------------------------------------------------------------------------------
int albaXMLElement::ParseData(double *vector,int size)
//------------------------------------------------------------------------------
{
  albaXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
  
  return 0;
}
//------------------------------------------------------------------------------
int albaXMLElement::ParseData(int *vector,int size)
//------------------------------------------------------------------------------
{
  albaXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
 
  return 0;
}


//------------------------------------------------------------------------------
mmuXMLDOMElement *albaXMLElement::GetXMLElement()
//------------------------------------------------------------------------------
{
  return m_DOMElement;
}

//------------------------------------------------------------------------------
albaXMLElement *albaXMLElement::FindNestedXMLElement(const char *name)
//------------------------------------------------------------------------------
{
  return (albaXMLElement *)FindNestedElement(name);
}
//------------------------------------------------------------------------------
const char *albaXMLElement::GetName()
//------------------------------------------------------------------------------
{
  return *m_Name;
}

//------------------------------------------------------------------------------
albaXMLStorage *albaXMLElement::GetXMLStorage()
//------------------------------------------------------------------------------
{
  return (albaXMLStorage *)m_Storage;
}

//------------------------------------------------------------------------------
albaXMLElement *albaXMLElement::GetXMLParent()
//------------------------------------------------------------------------------
{
  return (albaXMLElement *)m_Parent;
}

//------------------------------------------------------------------------------
albaStorageElement::ChildrenVector &albaXMLElement::GetChildren()
//------------------------------------------------------------------------------
{
  
  if (!m_Children)
  {
    // create and fill in new children list with element nodes
    m_Children = new ChildrenVector;
    XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *children=m_DOMElement->m_XMLElement->getChildNodes();
    
	int length = children->getLength();
	for (unsigned int i = 0; i<length;i++)
    {
      XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *child_element=children->item(i);
      if (child_element->getNodeType()==XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
      {
        albaXMLElement *child=new albaXMLElement(
			new mmuXMLDOMElement((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)child_element),this,GetXMLStorage());
        m_Children->push_back(child);
      }      
    }
  }
  return *m_Children;
}

//------------------------------------------------------------------------------
albaStorageElement *albaXMLElement::AppendChild(const char *name)
//------------------------------------------------------------------------------
{
 return AppendXMLChild(name); 
}

//------------------------------------------------------------------------------
albaXMLElement *albaXMLElement::AppendXMLChild(const char *name)
//------------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *child_element=GetXMLStorage()->GetXMLDOM()->m_XMLDoc->createElement(albaXMLString(name));
  m_DOMElement->m_XMLElement->appendChild(child_element);
  albaXMLElement *child=new albaXMLElement(new mmuXMLDOMElement(child_element),this,GetXMLStorage());
  GetChildren().push_back(child);
  return child;
}

//------------------------------------------------------------------------------
void albaXMLElement::SetAttribute(const char *name,const char *value)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(value);
  
  m_DOMElement->m_XMLElement->setAttribute(albaXMLString(name),albaXMLString(value));
}

//------------------------------------------------------------------------------
bool albaXMLElement::GetAttribute(const char *name, albaString &value)
//------------------------------------------------------------------------------
{
  assert(name);

	if (!m_DOMElement->m_XMLElement->hasAttribute(albaXMLString(name)))
		return false;

  const XMLCh *xml_value=m_DOMElement->m_XMLElement->getAttribute(albaXMLString(name));
	value=albaXMLString(xml_value);

	// Convert old MAF format 
	wxString val = value;
	if (val.SubString(0, 2) == "maf")
	{
		val.Replace("maf", "alba");
		value = val;
	}

  return true;
}

//------------------------------------------------------------------------------
void albaXMLElement::WriteXMLText(const char *text)
//------------------------------------------------------------------------------
{
  XERCES_CPP_NAMESPACE_QUALIFIER DOMText *text_node=GetXMLStorage()->GetXMLDOM()->m_XMLDoc->createTextNode(albaXMLString(text));
  m_DOMElement->m_XMLElement->appendChild(text_node);
}

//------------------------------------------------------------------------------
int albaXMLElement::StoreText(const char *name, const char *text)
//------------------------------------------------------------------------------
{
  assert(text);
  assert(name);

  albaXMLElement *text_node=AppendXMLChild(name);
  text_node->WriteXMLText(text);
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaXMLElement::StoreMatrix(const char *name,const albaMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);
  assert(name);

  // Write all the 16 elements into as a single 16-tupla
  albaString elements;
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    { 
      elements << albaString(matrix->GetElements()[i][j]) << " ";
    }
    elements << "\n"; // cr for read-ability
  }

  albaXMLElement *matrix_node=AppendXMLChild(name);
  matrix_node->WriteXMLText(elements);

  // add also the timestamp as an attribute
  matrix_node->SetAttribute("TimeStamp",albaString(matrix->GetTimeStamp()));
  return ALBA_OK;
}

//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(albaXMLElement *element,T *comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  albaString elements;
  for (int i=0;i<num;i++)
  { 
    elements << albaString(comps[i]) << " ";
  }

  albaXMLElement *vector_node=element->AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}


//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(albaXMLElement *element,const std::vector<T> &comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  albaString elements;
  for (int i=0;i<num;i++)
  { 
    elements << albaString(comps[i]) << " ";
  }

  albaXMLElement *vector_node=element->AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}

//------------------------------------------------------------------------------
int albaXMLElement::StoreVectorN(const char *name,double *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaXMLElement::StoreVectorN(const char *name,int *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::StoreVectorN(const char *name,const std::vector<double> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int albaXMLElement::StoreVectorN(const char *name,const std::vector<int> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::StoreVectorN(const char *name,const std::vector<albaString> &comps,int num,const char *tag)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(tag);

  albaStorageElement *subelement = AppendChild(name);
  for (int i=0;i<num;i++)
  {
    subelement->StoreText(tag,comps[i]);
  }
  return ALBA_OK;
}
/*
//------------------------------------------------------------------------------
int albaXMLElement::StoreData(const char *name,const char *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::StoreData16(const char *name,const short *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::StoreData32(const char *name,const long *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::RestoreData(char *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::RestoreData16( short *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::RestoreData32( long *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return ALBA_OK;
}
*/
//------------------------------------------------------------------------------
int albaXMLElement::RestoreMatrix(albaMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);

  matrix->Zero();
  
  double *elem=*matrix->GetElements();
  if (this->ParseData(elem,16)==16)
  {
    albaTimeStamp time_stamp;
    this->GetAttributeAsDouble("TimeStamp",time_stamp);
    matrix->SetTimeStamp(time_stamp);
    return ALBA_OK; 
  }

  albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element" );

  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaXMLElement::RestoreVectorN(double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return ALBA_OK;

  albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaXMLElement::RestoreVectorN(int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return ALBA_OK;

  albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaXMLElement::RestoreVectorN(std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return ALBA_OK;

  albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int albaXMLElement::RestoreVectorN(std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return ALBA_OK;

  albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return ALBA_ERROR;
}
//------------------------------------------------------------------------------
int albaXMLElement::RestoreVectorN(std::vector<albaString> &comps,unsigned int num,const char *tag)
//------------------------------------------------------------------------------
{
  assert(tag);

  albaString tag_name=tag;

// force children list creation
  ChildrenVector &children=this->GetChildren();

  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    albaXMLElement *item_node=(albaXMLElement*)children[i];
    if (tag_name==item_node->GetName())
    {
      comps[i]=albaXMLString(item_node->GetXMLElement()->m_XMLElement->getTextContent());
    }
    else
    {
      albaWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> item_node: wrong sub-element inside nested XML element <"<<(tag_name.GetCStr())<<">" );
      return ALBA_ERROR;
    }
  }
  return ALBA_OK;
}
//------------------------------------------------------------------------------
int albaXMLElement::RestoreText(albaString &buffer)
//------------------------------------------------------------------------------
{
  buffer=albaXMLString(this->GetXMLElement()->m_XMLElement->getTextContent());

  return ALBA_OK;
}


