/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeVector.txx,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:58:28 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafTimeVector_txx
#define __mafTimeVector_txx

#include "mafTimeVector.h"
#include "mafIndent.h"
#include <math.h>
#include <assert.h>

//-----------------------------------------------------------------------
//template <class T>
//mafCxxAbstractTypeMacro( mafTimeVector<T> )
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
template <class T>
mafTimeVector<T>::mafTimeVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
mafTimeVector<T>::~mafTimeVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::AppendAndSetItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (this->GetNumberOfItems()>0)
  {
    // Get last item
	  assert(m_TimeVector.rbegin()->second);
    // append adding 1 to the last time 
    m->SetTimeStamp(m_TimeVector.rbegin()->first+1);
  }
  else
	{
    m->SetTimeStamp(0);
  }
  AppendItem(m);
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::AppendItem(T *m)
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

	m_TimeVector.insert(m_TimeVector.end(),mmuTimePair(m->GetTimeStamp(),m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::PrependItem(T *m)
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
  m_TimeVector.insert(m_TimeVector.begin(),mmuTimePair(m->GetTimeStamp(),m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::InsertItem(T *m)
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

  m_TimeVector.insert(mmuTimePair(m->GetTimeStamp(),m));	
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::GetTimeBounds(mafTimeStamp tbounds[2])
//-------------------------------------------------------------------------
{
  // this is a sorted array
  if (m_TimeVector.size()>0)
  {
	  tbounds[0]=m_TimeVector.begin()->first;
	  tbounds[1]=m_TimeVector.rbegin()->first;
  }
  else
  {
	  tbounds[0]=-1;
	  tbounds[1]=-1;
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::GetTimeStamps(mmuTimeVector &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (mafTimeVector<T>::TimeVector::const_iterator it=m_TimeVector.begin();it!=m_TimeVector.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::DeepCopy(mafTimeVector *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  m_TimeVector=o->m_TimeVector;
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
bool mafTimeVector<T>::Equals(mafTimeVector *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  mafTimeVector<T>::TimeVector::iterator it;
  mafTimeVector<T>::TimeVector::iterator it2;
  for (it=m_TimeVector.begin(),it2=o->m_TimeVector.begin();it!=m_TimeVector.end();it++,it2++)
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
void mafTimeVector<T>::RemoveItem(mafTimeVector<T>::TimeVector::iterator it)
//-------------------------------------------------------------------------
{
  m_TimeVector.erase(it);
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
int mafTimeVector<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindItemByIndex(idx);
  if (it!=m_TimeVector.end())
  {
    RemoveItem(it);
    return MAF_OK;
  }

  return MAF_ERROR;
}

//-------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeVector.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
mafTimeVector<T>::TimeVector::iterator mafTimeVector<T>::FindNearestItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  std::pair<mafTimeVector<T>::TimeVector::iterator,mafTimeVector<T>::TimeVector::iterator> range=m_TimeVector.equal_range(t);
  if (range.first!=m_TimeVector.end())
  {
    if (range.second!=m_TimeVector.end())
    {
      if (fabs(range.first->first-t)>fabs(range.second->first-t))
         return range.second;
    }    
    return range.first;
  }
  else if (range.second!=m_TimeVector.end())
  {
    return range.second;
  }

  return --range.second;
}


//-------------------------------------------------------------------------
template <class T>
mafTimeVector<T>::TimeVector::iterator mafTimeVector<T>::FindItemBefore(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeVector.size()>0)
  {
    mafTimeVector<T>::TimeVector::iterator it=m_TimeVector.lower_bound(t); // find first item >= t
    if (it==m_TimeVector.end()||it!=m_TimeVector.begin()&&it->first>t) // if > t get the previous 
      --it;

    return it;
  }
  else
  {
    return m_TimeVector.end();
  }
  
}


//-------------------------------------------------------------------------
template <class T>
mafTimeVector<T>::TimeVector::iterator mafTimeVector<T>::FindItem(mafTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeVector.find(t);
}

//----------------------------------------------------------------------------
template <class T>
mafTimeVector<T>::TimeVector::iterator mafTimeVector<T>::FindItem(T *m)
//----------------------------------------------------------------------------
{
  assert(m);
  return m_TimeVector.find(m->GetTimeStamp());
}

//----------------------------------------------------------------------------
template <class T>
void mafTimeVector<T>::Print(std::ostream& os, const int tabs) const
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
mafTimeStamp mafTimeVector<T>::GetItemTime(int idx)
//----------------------------------------------------------------------------
{
  T *m=GetItemByIndex(idx);
  if (m) return m->GetTimeStamp();
  return -1;
}

//----------------------------------------------------------------------------
template <class T>
mafTimeVector<T>::TimeVector::iterator mafTimeVector<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if ((unsigned int)idx<m_TimeVector.size() && idx>=0)
  {
     mafTimeVector<T>::TimeVector::iterator it=m_TimeVector.begin();
     std::advance(it,idx);
     return it;
  }
  
  return m_TimeVector.end();
}

//----------------------------------------------------------------------------
template <class T>
mafID mafTimeVector<T>::FindItemIndex(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindItem(t);
  return it!=m_TimeVector.end()?std::distance(m_TimeVector.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeVector<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindItemByIndex(idx);
  return (it!=m_TimeVector.end())?it->second:NULL;
}

//----------------------------------------------------------------------------
template <class T>
T *mafTimeVector<T>::GetItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindItem(t);
  return (it!=m_TimeVector.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeVector<T>::GetNearestItem(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindNearestItem(t);
  return (it!=m_TimeVector.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *mafTimeVector<T>::GetItemBefore(mafTimeStamp t)
//----------------------------------------------------------------------------
{
  mafTimeVector<T>::TimeVector::iterator it=FindItemBefore(t);
  return (it!=m_TimeVector.end())?it->second:NULL;
}

#endif
