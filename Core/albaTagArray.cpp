/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTagArray
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaTagArray.h"
#include "albaStorageElement.h"
#include "albaIndent.h"
#include <assert.h>

albaCxxTypeMacro(albaTagArray)

//-------------------------------------------------------------------------
albaTagArray::albaTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
albaTagArray::~albaTagArray()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void albaTagArray::operator=(const albaTagArray &a)
//-------------------------------------------------------------------------
{
  Superclass::operator =(a);
}

//-------------------------------------------------------------------------
void albaTagArray::DeepCopy(const albaTagArray *a)
//-------------------------------------------------------------------------
{
  Superclass::DeepCopy(a);
  mmuTagsMap::const_iterator it;
  for (it=a->m_Tags.begin();it!=a->m_Tags.end();it++)
  {
    const albaTagItem &titem=it->second;
    SetTag(titem);
  }
}

//-------------------------------------------------------------------------
void albaTagArray::DeepCopy(const albaAttribute *a)
//-------------------------------------------------------------------------
{
  if (a->IsALBAType(albaTagArray))
  {
    DeepCopy((const albaTagArray *)a);
  }
}

//-------------------------------------------------------------------------
albaTagItem *albaTagArray::GetTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    return &((*it).second);

  return NULL;
}

//-------------------------------------------------------------------------
bool albaTagArray::GetTag(const char *name,albaTagItem &item)
//-------------------------------------------------------------------------
{
  albaTagItem *tmp_item=GetTag(name);
  if (tmp_item)
  {
    item=*tmp_item;

    return true;
  }

  return false;
}

//-------------------------------------------------------------------------
bool albaTagArray::IsTagPresent(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  return it!=m_Tags.end();
}

//-------------------------------------------------------------------------
void albaTagArray::SetTag(const albaTagItem &value)
//-------------------------------------------------------------------------
{
  albaTagItem *tmp_item=GetTag(value.GetName());
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
void albaTagArray::SetTag(const char *name, const char *value,int type)
//-------------------------------------------------------------------------
{
  albaTagItem tmp(name,value,type);
  m_Tags[name]=tmp;
}

//----------------------------------------------------------------------------
void albaTagArray::SetTag(const char *name, double value)
{
	albaTagItem tmp(name, value);
	m_Tags[name] = tmp;
}

//-------------------------------------------------------------------------
void albaTagArray::DeleteTag(const char *name)
//-------------------------------------------------------------------------
{
  mmuTagsMap::iterator it=m_Tags.find(name);
  if (it!=m_Tags.end())
    m_Tags.erase(it);
}

//-------------------------------------------------------------------------
void albaTagArray::GetTagList(std::vector<std::string> &list)
//-------------------------------------------------------------------------
{
  list.clear();
  list.resize(GetNumberOfTags());
  int i=0;
  for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end();it++,i++)
  {
    albaTagItem &titem=it->second;
    list[i]=titem.GetName();
  }
}

//-------------------------------------------------------------------------
bool albaTagArray::operator==(const albaTagArray &a) const
//-------------------------------------------------------------------------
{
  return Equals(&a);
}

//-------------------------------------------------------------------------
bool albaTagArray::Equals(const albaTagArray *array) const
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
void albaTagArray::GetTagsByType(int type, std::vector<albaTagItem *> &array)
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
int albaTagArray::GetNumberOfTags() const
//-------------------------------------------------------------------------
{
  return m_Tags.size();
}

//-------------------------------------------------------------------------
int albaTagArray::InternalStore(albaStorageElement *parent)
//-------------------------------------------------------------------------
{
  int ret=Superclass::InternalStore(parent);
  if (ret==ALBA_OK)
  {  
    parent->SetAttribute("NumberOfTags",albaString(GetNumberOfTags()));
  
    for (mmuTagsMap::iterator it=m_Tags.begin();it!=m_Tags.end()&&ret==ALBA_OK;it++)
    {
      albaStorageElement *item_element=parent->AppendChild("TItem");
      ret=it->second.Store(item_element)!=ALBA_OK;
    }
  }
  return ret;
}

//-------------------------------------------------------------------------
int albaTagArray::InternalRestore(albaStorageElement *node)
//-------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==ALBA_OK)
  {
    albaID numAttrs=-1;
    node->GetAttributeAsInteger("NumberOfTags",numAttrs);
  
    albaStorageElement::ChildrenVector &children=node->GetChildren();
    int ret=ALBA_OK;
    int idx=0;
    for (int i=0;(idx < numAttrs) && (i < children.size()) && (ret == ALBA_OK);i++)
    {
      if (albaString().Set(children[i]->GetName())=="TItem")
      {
        albaTagItem new_titem;
        ret=new_titem.Restore(children[i]);
        SetTag(new_titem);
        idx++;
      }
    }

    if (idx<numAttrs)
    {
      albaErrorMacro("Error Restoring TagArray: wrong number of restored items, should be "<<numAttrs<<", found "<<children.size());
      return ALBA_ERROR;
    }

    return ret;
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
void albaTagArray::Print(std::ostream& os, const int tabs) const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  
  albaIndent indent(tabs);
  os << indent << "Tags:"<<std::endl;
  albaIndent next_indent(indent.GetNextIndent());

  albaTagItem item;
  for (mmuTagsMap::const_iterator it=m_Tags.begin();it!=m_Tags.end();it++)
  {
    item = it->second;
    item.Print(os,next_indent);
  }
}