/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-29 18:00:27 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafXMLElement.h"
#include "mafXMLStorage.h"
#include "mafXMLString.h"
#include "mafMatrix.h"
#include "mafVector.txx"

#include "mmuXMLDOM.h"

#include <sstream>
#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
mafXMLElement::mafXMLElement(DOMElement *element,mafXMLElement *parent,mafXMLStorage *storage) :
  mafStorageElement(parent,storage)
//------------------------------------------------------------------------------
{
  assert(storage);
  assert(element);
  m_XMLElement = element;
  m_Name = new mafXMLString(element->getTagName());
}

//------------------------------------------------------------------------------
mafXMLElement::~mafXMLElement()
//------------------------------------------------------------------------------
{
  // the XML element is destroyed by its creator (the DOMDocument)
  m_XMLElement = NULL;
}

//------------------------------------------------------------------------------
int mafXMLElement::ParseData(const char *text,double *vector,int size)
//------------------------------------------------------------------------------
{
  std::istringstream instr(text);

  for (int i=0;i<size;)
  {
    if (instr.eof())
      return i;

    instr>>vector[i];
  }

  return size;
}

//------------------------------------------------------------------------------
int mafXMLElement::ParseData(double *vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    return ParseData(text_data,vector,size);
  }
  
  return 0;
}
//------------------------------------------------------------------------------
int mafXMLElement::ParseData(int *vector,int size)
//------------------------------------------------------------------------------
{
  mafXMLString text_data(m_XMLElement->getTextContent());
  if (text_data.GetCStr())
  {
    double *tmp=new double[size];
    int ret=ParseData(text_data,tmp,size);
    for (int i=0;i<ret;i++)
      vector[i]=tmp[i];
    delete [] tmp;
    return ret;
  }
  
  return 0;
}

//------------------------------------------------------------------------------
DOMElement *mafXMLElement::GetXMLElement()
//------------------------------------------------------------------------------
{
  return m_XMLElement;
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
mafVector<mafStorageElement *> *mafXMLElement::GetChildren()
//------------------------------------------------------------------------------
{
  
  if (!m_Children)
  {
    // create and fill in new children list with element nodes
    m_Children = new mafVector<mafStorageElement *>;
    DOMNodeList *children=m_XMLElement->getChildNodes();
    for (int i = 0; i<children->getLength();i++)
    {
      DOMNode *child_element=children->item(i);
      if (children->item(i)->getNodeType()==DOMNode::ELEMENT_NODE)
      {
        mafXMLElement *child=new mafXMLElement((DOMElement *)child_element,this,GetXMLStorage());
        m_Children->AppendItem(child);
      }      
    }
  }
  return m_Children;
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
  mafXMLElement *child=new mafXMLElement(child_element,this,GetXMLStorage());
  GetChildren()->AppendItem(child);
  return child;
}

//------------------------------------------------------------------------------
void mafXMLElement::SetXMLAttribute(const char *name,const char *value)
//------------------------------------------------------------------------------
{
  assert(name);
  assert(value);
  
  m_XMLElement->setAttribute(mafXMLString(name),mafXMLString(value));
}

//------------------------------------------------------------------------------
bool mafXMLElement::GetXMLAttribute(const char *name, mafString &value)
//------------------------------------------------------------------------------
{
  assert(name);
  const XMLCh *xml_value=m_XMLElement->getAttribute(mafXMLString(name));
  if (xml_value)
  {
    value.Copy(mafXMLString(xml_value));
    return true;
  }
  return false; 
}

//------------------------------------------------------------------------------
void mafXMLElement::WriteXMLText(const char *text)
//------------------------------------------------------------------------------
{
  DOMText *text_node=GetXMLStorage()->GetXMLDOM()->m_XMLDoc->createTextNode(mafXMLString(text));
  m_XMLElement->appendChild(text_node);
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreText(const const char *text,const char *name)
//------------------------------------------------------------------------------
{
  assert(text);
  assert(name);

  mafXMLElement *text_node=AppendXMLChild(name);
  text_node->WriteXMLText(text);
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreDouble(const double &value,const char *name)
//------------------------------------------------------------------------------
{
  StoreText(mafString(value),name);
}
//------------------------------------------------------------------------------
void mafXMLElement::StoreInteger(const int &value,const char *name)
//------------------------------------------------------------------------------
{
  StoreText(mafString(value),name);
}
//------------------------------------------------------------------------------
void mafXMLElement::StoreMatrix(mafMatrix *matrix,const char *name)
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
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreObjectVector(mafVector<mafStorable *> *vector,const char *name)
//------------------------------------------------------------------------------
{
  assert(vector);
  assert(name);

  // create sub node for storing the vector
  mafXMLElement *vector_node = AppendXMLChild(name);
  vector_node->SetXMLAttribute("Size",mafString(vector->GetNumberOfItems()));
  
  for (int i=0;i<vector->GetNumberOfItems();i++)
  {
    mafStorable *obj=vector->GetItem(i);
    assert(obj);
    obj->Store(vector_node);
  }
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreVector3(double comps[3],const char *name)
//------------------------------------------------------------------------------
{
  assert(name);

  // Write all the 3 elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<3;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }

  mafXMLElement *vector_node=AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreVectorN(double *comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(comps);
  assert(name);

  // Write all the 3 elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements << mafString(comps[i]) << " ";
  }

  mafXMLElement *vector_node=AppendXMLChild(name);
  vector_node->WriteXMLText(elements);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreMatrix(mafMatrix *matrix,const char *name)
//------------------------------------------------------------------------------
{
  assert(matrix);

  matrix->Zero();

  mafXMLElement *subnode=FindNestedXMLElement(name);
  if (subnode)
  {
    double *elem=*matrix->GetElements();
    if (subnode->ParseData(elem,16)==16)
      return 0; 

    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside nested XML element <"<<name<<">" );
  }
  else
  {
    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );
  }

  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(double *comps,unsigned int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(comps);

  mafXMLElement *subnode=FindNestedXMLElement(name);
  if (subnode)
  {
    if (subnode->ParseData(comps,num)==num)
      return 0;

    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: wrong number of fields inside nested XML element <"<<name<<">" );
  }
  else
  {
    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );
  }
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafString &buffer,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);
  mafXMLElement *subnode=FindNestedXMLElement(name);
  if (subnode)
  {
    buffer.Copy(mafXMLString(subnode->GetXMLElement()->getTextContent()));    
    return 0;
  }
  else
  {
    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: cannot find XML element <"<<name<<">" );
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(char *&buffer,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);
  mafString tmp;
  if (!RestoreText(tmp,name))
  {
    buffer=tmp.Duplicate();
    return 0;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreDouble(double &value,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);
  mafXMLElement *subnode=FindNestedXMLElement(name);
  if (subnode)
  {
    if (subnode->ParseData(&value,1)==1)     
      return 0;

    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: error parsing nested XML element <"<<name<<">" );
  }
  else
  {
    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: cannot find nested XML element <"<<name<<">" );
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreInteger(int &value,const char *name)
//------------------------------------------------------------------------------
{
  assert(name);
  mafXMLElement *subnode=FindNestedXMLElement(name);
  if (subnode)
  {
    if (subnode->ParseData(&value,1)==1)     
      return 0;

    mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: error parsing nested XML element <"<<name<<">" );
  }  

  mafWarningMacro("XML Parse Error while parsing <"<<GetName()<<"> element: cannot find XML nested element <"<<name<<">" );
  return -1;
}
