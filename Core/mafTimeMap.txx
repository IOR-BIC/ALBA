/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMap.txx,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:06:44 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTimeMap_txx
#define __mafTimeMap_txx

#include "mafTimeMap.h"
#include "mafIndent.h"
#include <math.h>
#include <assert.h>
#include <sstream>

//-----------------------------------------------------------------------
//template <class T>
//mafCxxAbstractTypeMacro( mafTimeMap<T> )
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
template <class T>
mafTimeMap<T>::mafTimeMap()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
mafTimeMap<T>::~mafTimeMap()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::AppendAndSetItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (this->GetNumberOfItems()>0)
  {
    // Get last item
	  assert(m_TimeMap.rbegin()->second);
    // append adding 1 to the last time 
    m->SetTimeStamp(m_TimeMap.rbegin()->first+1);
  }
  else
	{
    m->SetTimeStamp(0);
  }
  AppendItem(m);
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::AppendItem(T *m)
//-----------------------------------------------------------------------
{
	assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Append item!");
      return;
    }
  }
	m_TimeMap.insert(m_TimeMap.end(),mmuTimePair(m->GetTimeStamp(),m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::PrependItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Prepend item!");
      return;
    }
  }
  m_TimeMap.insert(m_TimeMap.begin(),mmuTimePair(m->GetTimeStamp(),m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::InsertItem(T *m)
//-------------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      mafErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Insert item!");
      return;
    }
  }

  m_TimeMap[m->GetTimeStamp()]=m;	
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::GetTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  // this is a sorted array
  if (m_TimeMap.size()>0)
  {
	  tbounds[0]=m_TimeMap.begin()->first;
	  tbounds[1]=m_TimeMap.rbegin()->first;
  }
  else
  {
	  tbounds[0]=-1;
	  tbounds[1]=-1;
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::GetTimeStamps(mmuTimeVector &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (mafTimeMap<T>::TimeMap::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::DeepCopy(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  m_TimeMap=o->m_TimeMap;
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
bool mafTimeMap<T>::Equals(mafTimeMap *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  mafTimeMap<T>::TimeMap::iterator it;
  mafTimeMap<T>::TimeMap::iterator it2;
  for (it=m_TimeMap.begin(),it2=o->m_TimeMap.begin();it!=m_TimeMap.end();it++,it2++)
  {
    T *m=it->second;
    T *m2=it2->second;

    if (!m->Equals(m2))
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::RemoveItem(mafTimeMap<T>::TimeMap::iterator it)
//-------------------------------------------------------------------------
{
  m_TimeMap.erase(it);
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
int mafTimeMap<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindItemByIndex(idx);
  if (it!=m_TimeMap.end())
  {
    RemoveItem(it);
    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::TimeMap::iterator mafTimeMap<T>::FindNearestItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  std::pair<mafTimeMap<T>::TimeMap::iterator,mafTimeMap<T>::TimeMap::iterator> range=m_TimeMap.equal_range(t);
  if (range.first!=m_TimeMap.end())
  {
    if (range.second!=m_TimeMap.end())
    {
      if (fabs(range.first->first-t)>fabs(range.second->first-t))
         return range.second;
    }    
    return range.first;
  }
  else if (range.second!=m_TimeMap.end())
  {
    return range.second;
  }

  return --range.second;
}


//-------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::TimeMap::iterator mafTimeMap<T>::FindItemBefore(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeMap.size()>0)
  {
    mafTimeMap<T>::TimeMap::iterator it=m_TimeMap.lower_bound(t); // find first item >= t
    if (it==m_TimeMap.end()||it!=m_TimeMap.begin()&&it->first>t) // if > t get the previous 
      --it;

    return it;
  }
  else
  {
    return m_TimeMap.end();
  }
  
}


//-------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::TimeMap::iterator mafTimeMap<T>::FindItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeMap.find(t);
}

//----------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::TimeMap::iterator mafTimeMap<T>::FindItem(T *m)
//----------------------------------------------------------------------------
{
  assert(m);
  return m_TimeMap.find(m->GetTimeStamp());
}

//----------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::Print(std::ostream& os, const int tabs) const
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "Number of Items:"<<GetNumberOfItems()<<"\n";
  mmuTimeVector tvector;
  os << indent << "Time Stamps: {";
  GetTimeStamps(tvector);
  for (unsigned int i=0;i<tvector.size();i++)
  {
	  if (i!=0)
	    os << ", ";
	  os << tvector[i];
  }

  os << "}\n";
}

//----------------------------------------------------------------------------
template <class T>
mafTimeStamp mafTimeMap<T>::GetItemTime(int idx)
//----------------------------------------------------------------------------
{
  T *m=GetItemByIndex(idx);
  if (m) return m->GetTimeStamp();
  return -1;
}

//----------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::TimeMap::iterator mafTimeMap<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if ((unsigned int)idx<m_TimeMap.size() && idx>=0)
  {
     mafTimeMap<T>::TimeMap::iterator it=m_TimeMap.begin();
     std::advance(it,idx);
     return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
mafID mafTimeMap<T>::FindItemIndex(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindItem(t);
  return it!=m_TimeMap.end()?std::distance(m_TimeMap.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindItemByIndex(idx);
  return (it!=m_TimeMap.end())?it->second:NULL;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeMap<T>::TimeMap::iterator it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}

#endif
