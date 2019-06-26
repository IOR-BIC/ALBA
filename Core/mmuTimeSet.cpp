/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmuTimeSet
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


#include "mmuTimeSet.h"
#include <math.h>
//------------------------------------------------------------------------------
//albaCxxTypeMacro(mmuTimeSet)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmuTimeSet::mmuTimeSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTimeSet::~mmuTimeSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTimeSet::mmuTimeSet(const mmuTimeSet& c)
//------------------------------------------------------------------------------
{ 
  m_TSet=c.m_TSet;
}

//------------------------------------------------------------------------------
mmuTimeSet &mmuTimeSet::operator=(const mmuTimeSet &o)
//------------------------------------------------------------------------------
{
  m_TSet=o.m_TSet;
  return *this;
}

//------------------------------------------------------------------------------
bool mmuTimeSet::operator==(const mmuTimeSet &o)
//------------------------------------------------------------------------------
{
  if (GetNumberOfTimeStamps()!=o.GetNumberOfTimeStamps())
    return false;

  Iterator it;
  TSet::const_iterator it2;
  for (it=m_TSet.begin(),it2=o.m_TSet.begin();it!=m_TSet.end();it++,it2++)
  {
    if (*it!=*it2)
      return false;
  }

  return true;
}
//------------------------------------------------------------------------------
void mmuTimeSet::Insert(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Append(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(m_TSet.end(),t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Prepend(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(m_TSet.begin(),t);
}
//------------------------------------------------------------------------------
void mmuTimeSet::Remove(Iterator ts)
//------------------------------------------------------------------------------
{
  m_TSet.erase(ts);
}

//------------------------------------------------------------------------------
albaID mmuTimeSet::FindTimeStampIndex(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindTimeStamp(t);
  return it!=m_TSet.end()?std::distance(m_TSet.begin(),it):-1;
}
//------------------------------------------------------------------------------
albaTimeStamp mmuTimeSet::GetByIndex(albaID idx)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.begin();
  std::advance(it,idx);
  return (it!=m_TSet.end()?*it:-1);
}
//------------------------------------------------------------------------------
albaTimeStamp mmuTimeSet::GetNearestTimeStamp(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindNearestTimeStamp(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
albaTimeStamp mmuTimeSet::GetTimeStampBefore(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindTimeStampBefore(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindTimeStamp(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  return m_TSet.find(t);
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindNearestTimeStamp(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  std::pair<Iterator,Iterator> range=m_TSet.equal_range(t);
  if (range.first!=m_TSet.end())
  {
    if (range.second!=m_TSet.end())
    {
      return fabs(*(range.first)-t)>fabs(*(range.second)-t)?range.second:range.first;
    }
    else
    {
      return range.first;
    }
  }
  else if (range.second!=m_TSet.end())
  {
    return range.second;
  }

  return --range.second;
}
//------------------------------------------------------------------------------
mmuTimeSet::Iterator mmuTimeSet::FindTimeStampBefore(albaTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.lower_bound(t); // find item < t
 
  if (it==m_TSet.end()||it!=m_TSet.begin()&&*it>t)
    --it;
  return it;
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const mmuTimeSet &v1,const mmuTimeSet &v2,mmuTimeSet &outv)
//-------------------------------------------------------------------------
{
  if (&outv!=&v1 && &outv!=&v2)
  {
    outv.Clear();
  }

  if (&outv!=&v1)   
  {
    if (&outv!=&v2)
    {
      outv=v1;

      for (TSet::const_iterator it=v2.GetConstTSet().begin();it!=v2.GetConstTSet().end();it++)
      {
        outv.Insert(*it);
      }
    }
    else
    {
      for (TSet::const_iterator it=v1.GetConstTSet().begin();it!=v1.GetConstTSet().end();it++)
      {
        outv.Insert(*it);
      }
    }
  }
  else
  {
    for (TSet::const_iterator it=v2.GetConstTSet().begin();it!=v2.GetConstTSet().end();it++)
    {
      outv.Insert(*it);
    }
  }
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const mmuTimeSet &v)
//-------------------------------------------------------------------------
{
  Merge(*this,v,*this);
}

//-------------------------------------------------------------------------
int mmuTimeSet::GetNumberOfTimeStamps() const
//-------------------------------------------------------------------------
{
  return m_TSet.size();
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const std::vector<albaTimeStamp> &v1,const std::vector<albaTimeStamp> &v2,std::vector<albaTimeStamp> &outv)
//-------------------------------------------------------------------------
{
  std::set<albaTimeStamp> outset;
  for (unsigned int idx=0;idx<v1.size();idx++)
  {
    outset.insert(v1[idx]);
  }

  for (unsigned int idx2=0;idx2<v2.size();idx2++)
  {
    outset.insert(v2[idx2]);
  }

  outv.clear();
  for (std::set<albaTimeStamp>::iterator it=outset.begin();it!=outset.end();it++)
  {
    outv.push_back(*it);
  }
}

//-------------------------------------------------------------------------
void mmuTimeSet::Merge(const std::vector<albaTimeStamp> &v)
//-------------------------------------------------------------------------
{
  for (unsigned int i=0;i<v.size();i++)
    Insert(v[i]);
}