/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTagArray.cpp,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:44:19 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafTagArray.h"
#include "mafStorageElement.h"
#include <sstream>
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
    mmuTagItem &titem=it->second;
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
mmuTagItem *mafTagArray::GetTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &((*it).second);

  return NULL;
}

//-------------------------------------------------------------------------
bool mafTagArray::GetTag(const char *name,mmuTagItem &item)
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
bool mafTagArray::IsTagPresent(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  return it!=m_Tags.end();
}

//-------------------------------------------------------------------------
void mafTagArray::SetTag(const mmuTagItem &value)
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
void mafTagArray::SetTag(const char *name, const char *value,int type)
//-------------------------------------------------------------------------
{
  mmuTagItem tmp(name,value,type);
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
    mmuTagItem &titem=it->second;
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
void mafTagArray::GetTagsByType(int type, std::vector<mmuTagItem *> &array)
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
int mafTagArray::InternalRestore(mafStorageElement *node)
//-------------------------------------------------------------------------
{
  mafString numAttrs;
  node->GetAttribute("NumberOfTags",numAttrs);
  int num=(int)atof(numAttrs);

  mafStorageElement::ChildrenVector &children=node->GetChildren();
  int ret=MAF_OK;
  int idx=0;
  for (int i=0;(idx<num)&&(i<children.size())&&(ret==MAF_OK);i++)
  {
    if (mafString().Set(children[i]->GetName())=="TItem")
    {
      mmuTagItem new_titem;
      ret=new_titem.Restore(children[i]);
      SetTag(new_titem);
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
