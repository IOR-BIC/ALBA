/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMapScalarUX.txx,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:16:59 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTimeMapScalar_txx
#define __mafTimeMapScalar_txx

#include "mafTimeMapScalar.h"
#include "mafIndent.h"
#include <math.h>
#include <assert.h>

//-----------------------------------------------------------------------
//template <class T>
//mafCxxAbstractTypeMacro( mafTimeMapScalar<T> )
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
template <class T>
mafTimeMapScalar<T>::mafTimeMapScalar()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
mafTimeMapScalar<T>::~mafTimeMapScalar()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::AppendItem(mafTimeStamp t, T m)
//-----------------------------------------------------------------------
{
	m_TimeMap.insert(m_TimeMap.end(), mmuTimePairScalars(t, m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::PrependItem(mafTimeStamp t, T m)
//-----------------------------------------------------------------------
{
  m_TimeMap.insert(m_TimeMap.begin(), mmuTimePairScalars(t,m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::InsertItem(mafTimeStamp t, T m)
//-------------------------------------------------------------------------
{
  m_TimeMap[t] = m;	
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::GetTimeBounds(mafTimeStamp tbounds[2])
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
void mafTimeMapScalar<T>::GetTimeStamps(mmuTimeVectorScalars &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (typename mafTimeMapScalar<T>::TimeMapScalars::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::DeepCopy(mafTimeMapScalar *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();

  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it;
  for (it = o->BeginScalarVector(); it != o->EndScalarVector();it++)
  {
    T new_item = it->second;
    AppendItem(it->first, new_item);
  }
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
bool mafTimeMapScalar<T>::Equals(mafTimeMapScalar *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it;
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it2;
  for (it=m_TimeMap.begin(),it2=o->m_TimeMap.begin();it!=m_TimeMap.end();it++,it2++)
  {
    T m = it->second;
    T m2 = it2->second;

    if (m != m2)
      return false;
  }

  return true;
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::RemoveItem(typename mafTimeMapScalar<T>::TimeMapScalars::iterator it)
//-------------------------------------------------------------------------
{
  m_TimeMap.erase(it);
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
int mafTimeMapScalar<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemByIndex(idx);
  if (it!=m_TimeMap.end())
  {
    RemoveItem(it);
    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
typename mafTimeMapScalar<T>::TimeMapScalars::iterator mafTimeMapScalar<T>::FindNearestItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  std::pair<typename mafTimeMapScalar<T>::TimeMapScalars::iterator,typename mafTimeMapScalar<T>::TimeMapScalars::iterator> range=m_TimeMap.equal_range(t);
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
typename mafTimeMapScalar<T>::TimeMapScalars::iterator mafTimeMapScalar<T>::FindItemBefore(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeMap.size()>0)
  {
    typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=m_TimeMap.lower_bound(t); // find first item >= t
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
typename mafTimeMapScalar<T>::TimeMapScalars::iterator mafTimeMapScalar<T>::FindItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeMap.find(t);
}

//----------------------------------------------------------------------------
template <class T>
typename mafTimeMapScalar<T>::TimeMapScalars::iterator mafTimeMapScalar<T>::FindItemByValue(T m)
//----------------------------------------------------------------------------
{
  for (it = m_TimeMap.begin(); it != m_TimeMap.end(); it++)
  {
    T m2 = it->second;
    if (m == m2)
      return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
void mafTimeMapScalar<T>::Print(std::ostream& os, const int tabs) const
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);
  os << indent << "Number of Items:"<<GetNumberOfItems()<<"\n";
  mmuTimeVectorScalars tvector;
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
mafTimeStamp mafTimeMapScalar<T>::GetItemTime(int idx)
//----------------------------------------------------------------------------
{
  mafTimeMapScalar<T>::TimeMapScalars::iterator it = FindItemByIndex(idx);
  return (it != m_TimeMap.end()) ? it->first : -1;
}

//----------------------------------------------------------------------------
template <class T>
typename mafTimeMapScalar<T>::TimeMapScalars::iterator mafTimeMapScalar<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if ((unsigned int)idx<m_TimeMap.size() && idx>=0)
  {
     typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=m_TimeMap.begin();
     std::advance(it,idx);
     return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
mafID mafTimeMapScalar<T>::FindItemIndex(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindItem(t);
  return it!=m_TimeMap.end()?std::distance(m_TimeMap.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T mafTimeMapScalar<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemByIndex(idx);
  return (it!=m_TimeMap.end())?it->second:MAF_SCALAR_MIN;
}

//----------------------------------------------------------------------------
template <class T>
T mafTimeMapScalar<T>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second:MAF_SCALAR_MIN;
}
//----------------------------------------------------------------------------
template <class T>
T mafTimeMapScalar<T>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second:MAF_SCALAR_MIN;
}
//----------------------------------------------------------------------------
template <class T>
T mafTimeMapScalar<T>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second:MAF_SCALAR_MIN;
}
#endif
