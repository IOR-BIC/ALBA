/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaTagArray.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-17 00:44:27 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmaTagArray.h"
#include "mafStorageElement.h"
#include <sstream>
#include <assert.h>

mafCxxTypeMacro(mmaTagArray)

//-------------------------------------------------------------------------
mmaTagArray::mmaTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mmaTagArray::~mmaTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mmaTagArray::operator=(const mmaTagArray &a)
//-------------------------------------------------------------------------
{
  Superclass::operator =(a);
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    mmuTagItem &titem=it->second;
    SetTag(titem);
  }
}

//-------------------------------------------------------------------------
void mmaTagArray::DeepCopy(const mmaTagArray *a)
//-------------------------------------------------------------------------
{
  assert(a);
  *this=*a;
}

//-------------------------------------------------------------------------
mmuTagItem *mmaTagArray::GetTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &((*it).second);

  return NULL;
}

//-------------------------------------------------------------------------
bool mmaTagArray::GetTag(const char *name,mmuTagItem &item)
//-------------------------------------------------------------------------
{
  mmuTagItem *tmp_item=GetTag(name);
  if (tmp_item)
  {
    item=*tmp_item;

    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
bool mmaTagArray::IsTagPresent(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  return it!=m_Tags.end();
}

//-------------------------------------------------------------------------
void mmaTagArray::SetTag(const mmuTagItem &value)
//-------------------------------------------------------------------------
{
  mmuTagItem *tmp_item=GetTag(value.GetName());
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
void mmaTagArray::SetTag(const char *name, const char *value,int type)
//-------------------------------------------------------------------------
{
  mmuTagItem tmp(name,value,type);
  m_Tags[name]=tmp;
}

//-------------------------------------------------------------------------
void mmaTagArray::DeleteTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    m_Tags.erase(it);
}

//-------------------------------------------------------------------------
void mmaTagArray::GetTagList(std::vector<std::string> &list)
//-------------------------------------------------------------------------
{
  list.clear();
  list.resize(GetNumberOfTags());
  int i=0;
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end();it++,i++)
  {
    mmuTagItem &titem=it->second;
    list[i]=titem.GetName();
  }
}

//-------------------------------------------------------------------------
bool mmaTagArray::operator==(const mmaTagArray &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
bool mmaTagArray::Equals(const mmaTagArray *array) const
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
    if (*it!=*it2)
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
void mmaTagArray::GetTagsByType(int type, std::vector<mmuTagItem *> &array)
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
int mmaTagArray::GetNumberOfTags() const
//-------------------------------------------------------------------------
{
  return m_Tags.size();
}

//-------------------------------------------------------------------------
int mmaTagArray::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfTags",mafString(GetNumberOfTags()));
  int ret=MAF_OK;
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end()&&ret==MAF_OK;it++)
  {
    mafStorageElement *item_element=parent->AppendChild("TItem");
    ret=it->second.Store(item_element)!=MAF_OK;
  }
  return ret;
}

//-------------------------------------------------------------------------
int mmaTagArray::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString numAttrs;
  node->GetAttribute("NumberOfTags",numAttrs);
  int num=atof(numAttrs);

  mafStorageElement::ChildrenVector &children=node->GetChildren();
  int ret=MAF_OK;
  int idx=0;
  for (int i=0;(idx<num)&&(i<children.size())&&(ret==MAF_OK);i++)
  {
    if (mafString(children[i]->GetName())=="TItem")
    {
      mmuTagItem new_titem;
      ret=new_titem.Restore(children[i]);
      idx++;
    }
  }

  if (idx<num)
  {
    mafErrorMacro("Error Restoring TagArray: wrong number of restored items.");
    return MAF_ERROR;
  }

  return ret;
}
