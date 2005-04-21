/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTagArray.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:58:27 $
  Version:   $Revision: 1.5 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafTagArray.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include <assert.h>

mafCxxTypeMacro(mafTagArray)

//-------------------------------------------------------------------------
mafTagArray::mafTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafTagArray::~mafTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafTagArray::operator=(const mafTagArray &a)
//-------------------------------------------------------------------------
{
  Superclass::operator =(a);
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    mafTagItem &titem=it->second;
    SetTag(titem);
  }
}

//-------------------------------------------------------------------------
void mafTagArray::DeepCopy(const mafTagArray *a)
//-------------------------------------------------------------------------
{
  assert(a);
  *this=*a;
}

//-------------------------------------------------------------------------
mafTagItem *mafTagArray::GetTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &((*it).second);

  return NULL;
}

//-------------------------------------------------------------------------
bool mafTagArray::GetTag(const char *name,mafTagItem &item)
//-------------------------------------------------------------------------
{
  mafTagItem *tmp_item=GetTag(name);
  if (tmp_item)
  {
    item=*tmp_item;

    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
bool mafTagArray::IsTagPresent(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  return it!=m_Tags.end();
}

//-------------------------------------------------------------------------
void mafTagArray::SetTag(const mafTagItem &value)
//-------------------------------------------------------------------------
{
  mafTagItem *tmp_item=GetTag(value.GetName());
  if (tmp_item)
  {
    *tmp_item=value;
  }
  else
  {
    m_Tags[value.GetName()]=value;
  }
}

//-------------------------------------------------------------------------
void mafTagArray::SetTag(const char *name, const char *value,int type)
//-------------------------------------------------------------------------
{
  mafTagItem tmp(name,value,type);
  m_Tags[name]=tmp;
}

//-------------------------------------------------------------------------
void mafTagArray::DeleteTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    m_Tags.erase(it);
}

//-------------------------------------------------------------------------
void mafTagArray::GetTagList(std::vector<std::string> &list)
//-------------------------------------------------------------------------
{
  list.clear();
  list.resize(GetNumberOfTags());
  int i=0;
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end();it++,i++)
  {
    mafTagItem &titem=it->second;
    list[i]=titem.GetName();
  }
}

//-------------------------------------------------------------------------
bool mafTagArray::operator==(const mafTagArray &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
bool mafTagArray::Equals(const mafTagArray *array) const
//-------------------------------------------------------------------------
{
  assert(array);
  if (GetNumberOfTags()!=array->GetNumberOfTags())
    return false;

  mmuTagsMap::const_iterator it=m_Tags.begin();
  mmuTagsMap::const_iterator it2=array->m_Tags.begin();
  int i=0;
  for (;it!=m_Tags.end();it++,it2++,i++)
  {
    if (it->second!=it2->second)
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mafTagArray::GetTagsByType(int type, std::vector<mafTagItem *> &array)
//-------------------------------------------------------------------------
{
  array.clear();
  mmuTagsMap::iterator it=m_Tags.begin();
  for (;it!=m_Tags.end();it++)
  {
    if (it->second.GetType()==type)
    {
      array.push_back(&(it->second));
    }
  }
}
//-------------------------------------------------------------------------
int mafTagArray::GetNumberOfTags() const
//-------------------------------------------------------------------------
{
  return m_Tags.size();
}

//-------------------------------------------------------------------------
int mafTagArray::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  int ret=Superclass::InternalStore(parent);
  if (ret==MAF_OK)
  {  
    parent->SetAttribute("NumberOfTags",mafString(GetNumberOfTags()));
  
    for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end()&&ret==MAF_OK;it++)
    {
      mafStorageElement *item_element=parent->AppendChild("TItem");
      ret=it->second.Store(item_element)!=MAF_OK;
    }
  }
  return ret;
}

//-------------------------------------------------------------------------
int mafTagArray::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafID numAttrs=-1;
    node->GetAttributeAsInteger("NumberOfTags",numAttrs);
  
    mafStorageElement::ChildrenVector &children=node->GetChildren();
    int ret=MAF_OK;
    int idx=0;
    for (int i=0;(idx < numAttrs) && (i < children.size()) && (ret == MAF_OK);i++)
    {
      if (mafString().Set(children[i]->GetName())=="TItem")
      {
        mafTagItem new_titem;
        ret=new_titem.Restore(children[i]);
        SetTag(new_titem);
        idx++;
      }
    }

    if (idx<numAttrs)
    {
      mafErrorMacro("Error Restoring TagArray: wrong number of restored items, should be "<<numAttrs<<", found "<<children.size());
      return MAF_ERROR;
    }

    return ret;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
void mafTagArray::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  
  mafIndent indent(tabs);
  os << indent << "Tags:"<<std::endl;
  mafIndent next_indent(indent.GetNextIndent());

  for (mmuTagsMap::const_iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    it->second.Print(os,next_indent);
  }
}