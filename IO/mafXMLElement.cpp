/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 11:52:40 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmuXMLDOMElement.h"
#include "mmuXMLDOM.h"
#include "mafXMLElement.h"

#include "mafXMLStorage.h"
#include "mafXMLString.h"

#include "mafMatrix.h"
#include "mafStorable.h"
#include "mafObjectFactory.h"

#include <vector>
#include <sstream>
#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
mafXMLElement::mafXMLElement(mmuXMLDOMElement *element,mafXMLElement *parent,mafXMLStorage *storage) :
  mafStorageElement(parent,storage)
//------------------------------------------------------------------------------
{
  assert(storage);
  assert(element);
  assert(element->m_XMLElement);
  m_DOMElement = element;
  m_Name = new mafXMLString(element->m_XMLElement->getTagName());
}

//------------------------------------------------------------------------------
mafXMLElement::~mafXMLElement()
//------------------------------------------------------------------------------
{
  // the XML element is destroyed by its creator (the DOMDocument)
  cppDEL(m_DOMElement);
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
int mafXMLElement::ParseData(std::vector<double> &vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
  
  return 0;
}
//------------------------------------------------------------------------------
int mafXMLElement::ParseData(std::vector<int> &vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
 
  return 0;
}

//------------------------------------------------------------------------------
int mafXMLElement::ParseData(double *vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
  
  return 0;
}
//------------------------------------------------------------------------------
int mafXMLElement::ParseData(int *vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_DOMElement->m_XMLElement->getTextContent());
  
  if (text_data.GetCStr())
  {
    return InternalParseData(text_data,vector,size);
  }
 
  return 0;
}


//------------------------------------------------------------------------------
mmuXMLDOMElement *mafXMLElement::GetXMLElement()
//------------------------------------------------------------------------------
{
  return m_DOMElement;
}

//------------------------------------------------------------------------------
mafXMLElement *mafXMLElement::FindNestedXMLElement(const char *name)
//------------------------------------------------------------------------------
{
  return (mafXMLElement *)FindNestedElement(name);
}
//------------------------------------------------------------------------------
const char *mafXMLElement::GetName()
//------------------------------------------------------------------------------
{
  return *m_Name;
}

//------------------------------------------------------------------------------
mafXMLStorage *mafXMLElement::GetXMLStorage()
//------------------------------------------------------------------------------
{
  return (mafXMLStorage *)m_Storage;
}

//------------------------------------------------------------------------------
mafXMLElement *mafXMLElement::GetXMLParent()
//------------------------------------------------------------------------------
{
  return (mafXMLElement *)m_Parent;
}

//------------------------------------------------------------------------------
mafStorageElement::ChildrenVector &mafXMLElement::GetChildren()
//------------------------------------------------------------------------------
{
  
  if (!m_Children)
  {
    // create and fill in new children list with element nodes
    m_Children = new ChildrenVector;
    DOMNodeList *children=m_DOMElement->m_XMLElement->getChildNodes();
    for (unsigned int i = 0; i<children->getLength();i++)
    {
      DOMNode *child_element=children->item(i);
      if (children->item(i)->getNodeType()==DOMNode::ELEMENT_NODE)
      {
        mafXMLElement *child=new mafXMLElement(new mmuXMLDOMElement((DOMElement *)child_element),this,GetXMLStorage());
        m_Children->push_back(child);
      }      
    }
  }
  return *m_Children;
}

//------------------------------------------------------------------------------
mafStorageElement *mafXMLElement::AppendChild(const char *name)
//------------------------------------------------------------------------------
{
 return AppendXMLChild(name); 
}

//------------------------------------------------------------------------------
mafXMLElement *mafXMLElement::AppendXMLChild(const char *name)
//------------------------------------------------------------------------------
{
  DOMElement *child_element=GetXMLStorage()->GetXMLDOM()->m_XMLDoc->createElement(mafXMLString(name));
  m_DOMElement->m_XMLElement->appendChild(child_element);
  mafXMLElement *child=new mafXMLElement(new mmuXMLDOMElement(child_element),this,GetXMLStorage());
  GetChildren().push_back(child);
  return child;
}

//------------------------------------------------------------------------------
void mafXMLElement::SetAttribute(const char *name,const char *value)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(value);
  
  m_DOMElement->m_XMLElement->setAttribute(mafXMLString(name),mafXMLString(value));
}

//------------------------------------------------------------------------------
bool mafXMLElement::GetAttribute(const char *name, mafString &value)
//------------------------------------------------------------------------------
{
  assert(name);
  const XMLCh *xml_value=m_DOMElement->m_XMLElement->getAttribute(mafXMLString(name));
  if (xml_value)
  {
    value=mafXMLString(xml_value);
    return true;
  }
  return false; 
}

//------------------------------------------------------------------------------
void mafXMLElement::WriteXMLText(const char *text)
//------------------------------------------------------------------------------
{
  DOMText *text_node=GetXMLStorage()->GetXMLDOM()->m_XMLDoc->createTextNode(mafXMLString(text));
  m_DOMElement->m_XMLElement->appendChild(text_node);
}

//------------------------------------------------------------------------------
int mafXMLElement::StoreText(const char *name, const char *text)
//------------------------------------------------------------------------------
{
  assert(text);
  assert(name);

  mafXMLElement *text_node=AppendXMLChild(name);
  text_node->WriteXMLText(text);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafXMLElement::StoreMatrix(const char *name,mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);
  assert(name);

  // Write all the 16 elements into as a single 16-tupla
  mafString elements;
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    { 
      elements << mafString(matrix->GetElements()[i][j]) << " ";
    }
    elements << "\n"; // cr for read-ability
  }

  mafXMLElement *matrix_node=AppendXMLChild(name);
  matrix_node->WriteXMLText(elements);

  // add also the timestamp as an attribute
  matrix_node->SetAttribute("TimeStamp",mafString(matrix->GetTimeStamp()));
  return MAF_OK;
}

//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(mafXMLElement *element,T *comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }

  mafXMLElement *vector_node=element->AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}


//------------------------------------------------------------------------------
template <class T>
void InternalStoreVectorN(mafXMLElement *element,const std::vector<T> &comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }

  mafXMLElement *vector_node=element->AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}

//------------------------------------------------------------------------------
int mafXMLElement::StoreVectorN(const char *name,double *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafXMLElement::StoreVectorN(const char *name,int *comps,int num)
//------------------------------------------------------------------------------
{
  assert(comps);
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::StoreVectorN(const char *name,const std::vector<double> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mafXMLElement::StoreVectorN(const char *name,const std::vector<int> &comps,int num)
//------------------------------------------------------------------------------
{
  InternalStoreVectorN(this,comps,num,name);
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::StoreVectorN(const char *name,const std::vector<mafString> &comps,int num,const char *tag)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(tag);

  mafStorageElement *subelement = AppendChild(name);
  for (int i=0;i<num;i++)
  {
    subelement->StoreText(tag,comps[i]);
  }
  return MAF_OK;
}
/*
//------------------------------------------------------------------------------
int mafXMLElement::StoreData(const char *name,const char *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::StoreData16(const char *name,const short *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::StoreData32(const char *name,const long *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreData(char *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreData16( short *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreData32( long *data, const int size)
//------------------------------------------------------------------------------
{
  // to be implemented
  return MAF_OK;
}
*/
//------------------------------------------------------------------------------
int mafXMLElement::RestoreMatrix(mafMatrix *matrix)
//------------------------------------------------------------------------------
{
  assert(matrix);

  matrix->Zero();
  
  double *elem=*matrix->GetElements();
  if (this->ParseData(elem,16)==16)
  {
    mafTimeStamp time_stamp;
    this->GetAttributeAsDouble("TimeStamp",time_stamp);
    matrix->SetTimeStamp(time_stamp);
    return MAF_OK; 
  }

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element" );

  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(double *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(int *comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(std::vector<double> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(std::vector<int> &comps,unsigned int num)
//------------------------------------------------------------------------------
{
  if (this->ParseData(comps,num)==num)
    return MAF_OK;

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside XML element." );
  
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(std::vector<mafString> &comps,unsigned int num,const char *tag)
//------------------------------------------------------------------------------
{
  assert(tag);

  mafString tag_name=tag;

// force children list creation
  ChildrenVector &children=this->GetChildren();

  // to be rewritten as a map access
  for (unsigned int i=0;i<children.size();i++)
  {
    mafXMLElement *item_node=(mafXMLElement*)children[i];
    if (tag_name==item_node->GetName())
    {
      comps[i]=mafXMLString(item_node->GetXMLElement()->m_XMLElement->getTextContent());
    }
    else
    {
      mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> item_node: wrong sub-element inside nested XML element <"<<(tag_name.GetCStr())<<">" );
      return MAF_ERROR;
    }
  }
  return MAF_OK;
}
//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafString &buffer)
//------------------------------------------------------------------------------
{
  buffer=mafXMLString(this->GetXMLElement()->m_XMLElement->getTextContent());

  return MAF_OK;
}


