/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaTagArray.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-14 10:21:18 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mmaTagArray.h"
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
bool mmaTagArray::Equals(mmaTagArray *array)
//-------------------------------------------------------------------------
{
  assert(array);
  if (GetNumberOfTags()!=array->GetNumberOfTags())
    return false;

  mmuTagsMap::iterator it=m_Tags.begin();
  mmuTagsMap::iterator it2=array->m_Tags.begin();
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
int mmaTagArray::GetNumberOfTags()
//-------------------------------------------------------------------------
{
  return m_Tags.size();
}

//-------------------------------------------------------------------------
int mmaTagArray::InternalStore(mafStorageElement *parent)
//-------------------------------------------------------------------------
{
  return MAF_OK;
}

//-------------------------------------------------------------------------
int mmaTagArray::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  return MAF_OK;
}
