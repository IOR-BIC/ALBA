/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafXMLElement.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:25 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafXMLElement.h"
#include "mafXMLStorage.h"
#include "mafMatrix.h"

#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
mafXMLElement(DOMElement *element,mafStorageElement *parent,mafStorage *storage) : mafStorageElement(parent,storage)
//------------------------------------------------------------------------------
{
  assert(parent->IsAType(mafXMLElement));
  assert(storage->IsAType(mafXMLStorage));
  assert(element);
  m_XMLElement = element;

}

//------------------------------------------------------------------------------
mafXMLElement::~mafXMLElement()
//------------------------------------------------------------------------------
{
  // the XML element is destroyed by its creator (the DOMDocument)
  m_XMLElement = NULL;
}

//------------------------------------------------------------------------------
mafStorageElement *mafXMLElement::AddChild(const char *name)
//------------------------------------------------------------------------------
{
  DOMElement *child_element=GetXMLStorage()->GetXMLDocument()->createElement(mafXMLString(name));
  mafXMLElement *child=new mafXMLElement(child_element,this,GetXMLStorage());
  m_Children->AppendItem(child);
  return child;
  
}

//------------------------------------------------------------------------------
mafXMLElement *mafXMLElement::AddXMLChild(const char *name)
//------------------------------------------------------------------------------
{
  return (mafXMLElement *)AddChild(name);
}

//------------------------------------------------------------------------------
void WriteText(mafXMLElement *element,const char *text)
//------------------------------------------------------------------------------
{
  DOMText *text_node=element->GetXMLStorage()->GetXMLDocument()->createTextNode(mafXMLString(text));
  element->m_XMLElement->appendChild(text_node);
}

//------------------------------------------------------------------------------
void mafXMLElement::StoreText(const const char *text,const char *name)
//------------------------------------------------------------------------------
{
  assert(text);
  assert(name);

  mafXMLElement *text_node=AddXMLChild(name);
  text_node->WriteText(text);
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
      elements<<matrix->Element[i][j]<<" ";
    }
    elements<<"\n"; // cr for read-ability
  }

  mafXMLElement *matrix_node=AddXMLChild(name);
  matrix_node->WriteText(elements);
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
    elements<<comps[i]<<" ";
  }

  mafXMLElement *vector_node=AddXMLChild(name);
  vector_node->WriteText(elements);
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
    elements<<comps[i]<<" ";
  }

  mafXMLElement *vector_node=AddXMLChild(name);
  vector_node->WriteText(elements);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreMatrix(mafXMLElement *node,mafMatrix *matrix,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  assert(matrix);
  
  mafXMLElement *subnode=node->FindNestedElementWithName(name);
  if (subnode)
  {
    double elem[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    if (parser->ReadInlineData(subnode,true,elem,0,16,VTK_DOUBLE)==16)
    {
      matrix->DeepCopy(elem);

      return 0;
    }
  }

  //vtkErrorWithObjectMacro(node,"XML Parse Error while parsing <"node->GetName()<<"> element: cannot find XML element <"<<name<<">" );
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreMatrix(mafMatrix *matrix,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreMatrix(m_Element,matrix,name);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(mafXMLElement *node,double *comps,unsigned int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  assert(comps);
  mafXMLElement *subnode=node->FindNestedElementWithName(name);
  if (subnode)
  {
    if (parser->ReadInlineData(subnode,true,comps,0,num,VTK_DOUBLE)==num)
    {
      return 0;
    }
  }
  //vtkErrorWithObjectMacro(node,"XML Parse Error while parsing <"node->GetName()<<"> element: cannot find XML element <"<<name<<">" );
  
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreVectorN(double *comps,unsigned int num,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreVectorN(m_Element,comps,num,name);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafXMLElement *node,mafString &buffer,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  mafXMLElement *subnode=node->FindNestedElementWithName(name);
  if (subnode)
  {
    buffer.SetMaxLength(64);

    char c;
    istream *stream=parser->GetStream();
    stream->clear(stream->rdstate() & ~ios::eofbit);
    stream->clear(stream->rdstate() & ~ios::failbit);
    stream->seekg(subnode->GetXMLByteIndex());

    while(stream->get(c) && (c != '>') && (!stream->fail()) );
    while(stream->get(c) && isspace(c) && (!stream->fail()) );
   
    int i=0;
    
    while ( c!='<' && (!stream->fail()))
    {
      buffer.GetCStr()[i]=c;
      stream->get(c);
      i++;
      if (i>=buffer.GetSize()) // enlarge memory
      {
        buffer.SetMaxLength(buffer.GetSize()*2);
      }
    }
    
    buffer.GetCStr()[i]=0;
    // resize memory to the right size...
    buffer.SetMaxLength(buffer.Length()+1);
    
    return stream->fail(); 
  }
  //vtkErrorWithObjectMacro(node,"XML Parse Error while parsing <"node->GetName()<<"> element: cannot find XML element <"<<name<<">" );
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(char *&buffer,const char *name="Text")
//------------------------------------------------------------------------------
{
  return RestoreText(m_Element,buffer,name);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafXMLElement *node,char *&buffer,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  mafString tmp;
  if (!RestoreText(node,parser,tmp,name))
  {
    buffer=tmp.Duplicate();
    return 0;
  }
  
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreText(mafString &buffer,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreText(m_Element,buffer,name);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreDouble(mafXMLElement *node,double &value,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  mafXMLElement *subnode=node->FindNestedElementWithName(name);
  if (subnode)
  {
    if (parser->ReadInlineData(subnode,true,&value,0,1,VTK_DOUBLE)==1)     
      return 0;
  }
  //vtkErrorWithObjectMacro(node,"XML Parse Error while parsing <"node->GetName()<<"> element: cannot find XML element <"<<name<<">" );
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreDouble(double &value,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreDouble(m_Element,value,name);
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreInteger(mafXMLElement *node,int &value,const char *name)
//------------------------------------------------------------------------------
{
  assert(node);
  mafXMLElement *subnode=node->FindNestedElementWithName(name);
  if (subnode)
  {
    if (parser->ReadInlineData(subnode,true,&value,0,1,VTK_INT)==1)     
      return 0;
  }  
  //vtkErrorWithObjectMacro(node,"XML Parse Error while parsing <"node->GetName()<<"> element: cannot find XML element <"<<name<<">" );
  return -1;
}

//------------------------------------------------------------------------------
int mafXMLElement::RestoreInteger(int &value,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreInteger(m_Element,value,name);
}