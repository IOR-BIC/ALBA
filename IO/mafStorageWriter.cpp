/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorageWriter.cpp,v $
  Language:  C++
  Date:      $Date: 2004-12-24 15:11:10 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafStorable.h"
#include "mflXMLWriter.h"
#include "mafXMLElement.h"
#include "vtkXMLDataParser.h"
#include "vtkMatrix4x4.h"
#include "vtkSetGet.h"
#include <assert.h>
#include "stdio.h"

//------------------------------------------------------------------------------
void mafStorable::StoreText(mafXMLElement *parent,const const char *text,const char *name)
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
void mafStorable::StoreText(,const const char *text,const char *name)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafStorable::StoreMatrix(mafXMLElement *parent,vtkMatrix4x4 *matrix,const char *name)
//------------------------------------------------------------------------------
{
  assert(parent);
  writer->OpenElement(name);
  writer->DisplayXML("\r");

  // Write all the 16 elements into as a single 16-tupla
  mafString elements;
  for (int i=0;i<4;i++)
  {
    for (int j=0;j<4;j++)
    { 
      elements.Append(matrix->Element[i][j]);
      elements.Append(" ");
    }
    elements.Append("\r");
  }
  writer->DisplayXML(elements);

  writer->CloseElement(name);
}

//------------------------------------------------------------------------------
void mafStorable::StoreMatrix(mafMatrix *matrix,const char *name)
//------------------------------------------------------------------------------
{
  StoreMatrix(m_Element,matrix,name);
}

//------------------------------------------------------------------------------
void mafStorable::StoreVector3(mafXMLElement *parent,double comps[3],const char *name)
//------------------------------------------------------------------------------
{
  assert(writer);
  writer->OpenElement(name);

  // Write all the 3 elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<3;i++)
  { 
    elements.Append(comps[i]);
    elements.Append(" ");
  }
  writer->DisplayXML(elements);

  writer->CloseElement(name);

}

//------------------------------------------------------------------------------
void mafStorable::StoreVector3(double comps[3],const char *name)
//------------------------------------------------------------------------------
{
  StoreVector3(m_Element,comps,name);
}

//------------------------------------------------------------------------------
void mafStorable::StoreVectorN(mafXMLElement *parent,double *comps,int num,const char *name)
//------------------------------------------------------------------------------
{
  assert(writer);
  assert(comps);
  writer->OpenElement(name);

  // Write all the 3 elements into as a single 3-tupla  
  mafString elements;
  for (int i=0;i<num;i++)
  { 
    elements.Append(comps[i]);
    elements.Append(" ");
  }
  writer->DisplayXML(elements);

  writer->CloseElement(name);

}

//------------------------------------------------------------------------------
void mafStorable::StoreVectorN(double *comps,int num,const char *name="Vector")
//------------------------------------------------------------------------------
{
  StoreVectorN(m_Element,comps,num,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreMatrix(mafXMLElement *node,vtkMatrix4x4 *matrix,const char *name)
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
int mafStorable::RestoreMatrix(mafMatrix *matrix,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreMatrix(m_Element,matrix,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreVectorN(mafXMLElement *node,double *comps,unsigned int num,const char *name)
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
int mafStorable::RestoreVectorN(double *comps,unsigned int num,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreVectorN(m_Element,comps,num,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreText(mafXMLElement *node,mafString &buffer,const char *name)
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
int mafStorable::RestoreText(char *&buffer,const char *name="Text")
//------------------------------------------------------------------------------
{
  return RestoreText(m_Element,buffer,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreText(mafXMLElement *node,char *&buffer,const char *name)
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
int mafStorable::RestoreText(mafString &buffer,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreText(m_Element,buffer,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreDouble(mafXMLElement *node,double &value,const char *name)
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
int mafStorable::RestoreDouble(double &value,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreDouble(m_Element,value,name);
}

//------------------------------------------------------------------------------
int mafStorable::RestoreInteger(mafXMLElement *node,int &value,const char *name)
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
int mafStorable::RestoreInteger(int &value,const char *name)
//------------------------------------------------------------------------------
{
  return RestoreInteger(m_Element,value,name);
}