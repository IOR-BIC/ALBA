/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuTSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-02-25 07:24:16 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mmuTSet.h"
#include <math.h>
//------------------------------------------------------------------------------
//mafCxxTypeMacro(mmuTSet)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mmuTSet::mmuTSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTSet::~mmuTSet()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mmuTSet::mmuTSet(const mmuTSet& c)
//------------------------------------------------------------------------------
{ 
  m_TSet=c.m_TSet;
}

//------------------------------------------------------------------------------
mmuTSet &mmuTSet::operator=(const mmuTSet &o)
//------------------------------------------------------------------------------
{
  m_TSet=o.m_TSet;
  return *this;
}
//------------------------------------------------------------------------------
void mmuTSet::Insert(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  m_TSet.insert(t);
}
//------------------------------------------------------------------------------
void mmuTSet::Remove(Iterator ts)
//------------------------------------------------------------------------------
{
  m_TSet.erase(ts);
}

//------------------------------------------------------------------------------
mafTimeStamp mmuTSet::GetByIndex(mafID idx)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.begin();
  std::advance(it,idx);
  return *it;
}
//------------------------------------------------------------------------------
mafTimeStamp mmuTSet::GetNearestTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindNearestTimeStamp(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
mafTimeStamp mmuTSet::GetLowerTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=FindLowerTimeStamp(t);
  return it!=m_TSet.end()?*it:-1;
}
//------------------------------------------------------------------------------
mmuTSet::Iterator mmuTSet::FindTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  return m_TSet.find(t);
}
//------------------------------------------------------------------------------
mmuTSet::Iterator mmuTSet::FindNearestTimeStamp(mafTimeStamp t)
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

  return m_TSet.end();
}
//------------------------------------------------------------------------------
mmuTSet::Iterator mmuTSet::FindLowerTimeStamp(mafTimeStamp t)
//------------------------------------------------------------------------------
{
  Iterator it=m_TSet.lower_bound(t); // find item < t
  if (it!=m_TSet.end()&&*it>t)
  {
    return it--;
  }
  return it;
}