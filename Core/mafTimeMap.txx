/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMap.txx,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:30:30 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTimeMap_txx
#define __mafTimeMap_txx

#include "mafTimeMap.h"
//#include "mmuTimeVector.h"
#include "mafIndent.h"
#include <assert.h>
#include <vector>
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
void mafTimeMap<T>::AppendItem(T *m)
//-----------------------------------------------------------------------
{
	assert(m);
	if (m->GetTimeStamp()<0)
	{
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
	}
	//m_TimeMap[m->GetTimeStamp()]=m;
	m_TimeMap.insert(m_TimeMap.end(),mmuTimePair(m->GetTimeStamp(),m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::PrependItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  m_TimeMap.insert(m_TimeMap.begin(),mmuTimePair(m->GetTimeStamp(),m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
int mafTimeMap<T>::InsertItem(T *item)
//-------------------------------------------------------------------------
{
  assert(item);

  if (item)
  {
    m_TimeMap[item->GetTimeStamp()]=item;	
    Modified();
    return MAF_OK;
  }
  else
  {
    return MAF_ERROR;
  }
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
void mafTimeMap<T>::GetTimeStamps(mmuTimeVector &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (Iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::GetTimeStamps(mafTimeStamp *&kframes)
//-------------------------------------------------------------------------
{
  kframes = new mafTimeStamp[GetNumberOfItems()];

  for (Iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes[i]=it->first;
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

  Iterator it;
  Iterator it2;
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
void mafTimeMap<T>::RemoveItem(Iterator it)
//-------------------------------------------------------------------------
{
  m_TimeMap.erase(it);
}

//-------------------------------------------------------------------------
template <class T>
int mafTimeMap<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  Iterator it=FindItemByIndex(idx);
  if (it!=m_TimeMap.end())
  {
    m_TimeMap.erase(it);
    Modified();
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
mafTimeMap<T>::Iterator mafTimeMap<T>::FindNearestItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  std::pair<Iterator,Iterator> range=m_TimeMap.equal_range(t);
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
mafTimeMap<T>::Iterator mafTimeMap<T>::FindItemBefore(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  Iterator it=m_TimeMap.lower_bound(t); // find first item >= t
  if (it==m_TimeMap.end()||it!=m_TimeMap.begin()&&it->first>t) // if > t get the previous 
    --it;

  return it;
}


//-------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::Iterator mafTimeMap<T>::FindItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeMap.find(t);
}

//----------------------------------------------------------------------------
template <class T>
mafTimeMap<T>::Iterator mafTimeMap<T>::FindItem(T *m)
//----------------------------------------------------------------------------
{
  assert(m);
  return m_TimeMap.find(m->GetTimeStamp());
}

//----------------------------------------------------------------------------
template <class T>
void mafTimeMap<T>::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "Number of Items:"<<GetNumberOfItems()<<"\n";
  mmuTimeVector tvector;
  os << indent << "Time Stamps: {";
  GetTimeStamps(tvector);
  for (int i=0;i<tvector.size();i++)
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
mafTimeMap<T>::Iterator mafTimeMap<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if (idx<m_TimeMap.size() && idx>=0)
  {
     Iterator it=m_TimeMap.begin();
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
  Iterator it=FindItem(t);
  return it!=m_TimeMap.end()?std::distance(m_TimeMap.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  Iterator it=FindItemByIndex(idx);
  return (it!=m_TimeMap.end())?it->second:NULL;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  Iterator it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  Iterator it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeMap<T>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  Iterator it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}

#endif
