/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeMapScalarUX
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaTimeMapScalar_h
#define __albaTimeMapScalar_h

#define ALBA_SCALAR_MIN -1.0e+299
#define ALBA_SCALAR_MAX 1.0e+299

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaTimeStamped.h"

#include "albaMTime.h"
#include "albaString.h"
#include <map>
#include <vector>
#include "albaTimeMapScalar.h"
#include "albaIndent.h"
#include <math.h>
#include <assert.h>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<albaTimeStamp> mmuTimeVectorScalars;

/** a dynamic associative sorted array of scalars indexed by their "timestamp".
This class wraps an STL set (associative sorted array) of timestamped scalars,
indexed by "timestamp". Its implementation depends on the undergoing data structure
(currently an STL "vector", next release could be based on "map" or "hash_map"). 
Some utility functions are also provided.
@todo
-
*/

template <class T>
class ALBA_EXPORT albaTimeMapScalar : public albaObject, public albaTimeStamped
{
public:
  typedef std::map<albaTimeStamp, T > TimeMapScalars;
  typedef std::pair<albaTimeStamp, T > mmuTimePairScalars;

  albaTimeMapScalar();  
  virtual ~albaTimeMapScalar();

  albaTimeMapScalar(const albaTimeMapScalar<T>&);  
  void operator=(const albaTimeMapScalar<T>&);

  /** set the TypeName of the kind of item accepted by this container */
  void SetItemTypeName(const char *tname) {m_ItemTypeName = tname;}
  const char *GetItemTypeName() {return m_ItemTypeName;}
  
  /**
    Insert an item to the vector trying to append it, anyway the array
    is kept sorted. */
  virtual void AppendItem(albaTimeStamp t, T m);

  /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  virtual void PrependItem(albaTimeStamp t, T m);

  /** Find an item index given its pointer*/
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator FindItemByValue(T m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(albaTimeStamp t, T m);
  
  /** Remove an item given its iterator */
  virtual void RemoveItem(typename albaTimeMapScalar<T>::TimeMapScalars::iterator it);

  /** Remove an item given its index*/
  int RemoveItem(int idx);

  /** Remove all the items*/
  virtual void RemoveAllItems();

  /** Return the timestamp of the i-th item*/
  albaTimeStamp GetItemTime(int idx);

  /** Return the list of timestamp of the key scalars in the given vector*/
  void GetTimeStamps(mmuTimeVectorScalars &kframes) const;

  /** Return the number of ITEMS stored in this object*/
  int GetNumberOfItems() const {return m_TimeMap.size();};
  
  /** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
  void GetTimeBounds(albaTimeStamp tbounds[2]);

  /** Copy data from another array*/
  void DeepCopy(albaTimeMapScalar *vitem);

  /**
  Compare two different arrays for equality. They are considered
  equivalent if their key scalars are equivalent.*/
  bool Equals(albaTimeMapScalar *vmat);

  /** Find the item with the timestamp nearest to t*/
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator FindNearestItem(albaTimeStamp t);

  /** Find the item with timestamp <=t*/
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator FindItemBefore(albaTimeStamp t);

  /**
  Find the item with the timestamp==t. Returns the item index, and 
  set "item" to its pointer. Return iterator.end() if not found.*/
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator FindItem(albaTimeStamp t);

  /** return iterator of item with given index */
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator FindItemByIndex(int idx);

  /** return index of the given item. return -1 if not found. */
  albaID FindItemIndex(albaTimeStamp t);

  /** find and return item corresponding to timestamp t. return NULL if not found. */
  T GetItem(albaTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T GetNearestItem(albaTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T GetItemBefore(albaTimeStamp t);

  /** return  the item with given its order index. NULL is returned if not found. */
  T GetItemByIndex(int idx);

  virtual void Print(std::ostream& os, const int tabs=0) const;

  typename albaTimeMapScalar<T>::TimeMapScalars::iterator BeginScalarVector() {return m_TimeMap.begin();}
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator EndScalarVector() {return m_TimeMap.end();}
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator LastScalarVector() {return --(m_TimeMap.end());}

protected:
  TimeMapScalars  m_TimeMap;        ///< the set storing the scalars
  albaString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};


//////////////////////////////////TEMPLATE  IMPLEMENTATION////////////////////////////


//-----------------------------------------------------------------------
template <class T>
albaTimeMapScalar<T>::albaTimeMapScalar()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
albaTimeMapScalar<T>::~albaTimeMapScalar()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::AppendItem(albaTimeStamp t, T m)
//-----------------------------------------------------------------------
{
	m_TimeMap.insert(m_TimeMap.end(), mmuTimePairScalars(t, m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::PrependItem(albaTimeStamp t, T m)
//-----------------------------------------------------------------------
{
  m_TimeMap.insert(m_TimeMap.begin(), mmuTimePairScalars(t,m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::InsertItem(albaTimeStamp t, T m)
//-------------------------------------------------------------------------
{
  m_TimeMap[t] = m;	
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::GetTimeBounds(albaTimeStamp tbounds[2])
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
void albaTimeMapScalar<T>::GetTimeStamps(mmuTimeVectorScalars &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (typename albaTimeMapScalar<T>::TimeMapScalars::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::DeepCopy(albaTimeMapScalar *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();

  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it;
  for (it = o->BeginScalarVector(); it != o->EndScalarVector();it++)
  {
    T new_item = it->second;
    AppendItem(it->first, new_item);
  }
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
bool albaTimeMapScalar<T>::Equals(albaTimeMapScalar *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it;
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it2;
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
void albaTimeMapScalar<T>::RemoveItem(typename albaTimeMapScalar<T>::TimeMapScalars::iterator it)
//-------------------------------------------------------------------------
{
  m_TimeMap.erase(it);
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
int albaTimeMapScalar<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemByIndex(idx);
  if (it!=m_TimeMap.end())
  {
    RemoveItem(it);
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
typename albaTimeMapScalar<T>::TimeMapScalars::iterator albaTimeMapScalar<T>::FindNearestItem(albaTimeStamp t)
//-------------------------------------------------------------------------
{
/** Added by Losi 08/17/2009:
After creating the test class albaScalarVectorTest I saw that the GetNearestScalar method returned the scalar with the
minimum timestamp greater or equal to the specified one instead of the scalar with the timestamp nearest to the specified one.
GetNearestScalar method of albaScalarVectorTest class only call this method of albaTimeMapScalar. */

  albaTimeMapScalar<T>::TimeMapScalars::iterator lowIt;
  albaTimeMapScalar<T>::TimeMapScalars::iterator upIt;
	
  lowIt = FindItemBefore(t); // find first item < t
  upIt = m_TimeMap.lower_bound(t); // find first item >= t

  if(lowIt != m_TimeMap.end())
  {
    if(upIt != m_TimeMap.end())
    {
      if(fabs(lowIt->first - t) < fabs(upIt->first - t))
      {
        return lowIt;
      }
      else
      {
        return upIt;
      }
    }
    return lowIt;
  }
  else if(upIt != m_TimeMap.end())
  {
    return upIt;
  }
  return --upIt;
}


//-------------------------------------------------------------------------
template <class T>
typename albaTimeMapScalar<T>::TimeMapScalars::iterator albaTimeMapScalar<T>::FindItemBefore(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeMap.size()>0)
  {
    typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=m_TimeMap.lower_bound(t); // find first item >= t
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
typename albaTimeMapScalar<T>::TimeMapScalars::iterator albaTimeMapScalar<T>::FindItem(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeMap.find(t);
}

//----------------------------------------------------------------------------
template <class T>
typename albaTimeMapScalar<T>::TimeMapScalars::iterator albaTimeMapScalar<T>::FindItemByValue(T m)
//----------------------------------------------------------------------------
{
  for (typename albaTimeMapScalar<T>::TimeMapScalars::iterator it = m_TimeMap.begin(); it != m_TimeMap.end(); it++)
  {
    T m2 = it->second;
    if (m == m2)
      return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
void albaTimeMapScalar<T>::Print(std::ostream& os, const int tabs) const
//----------------------------------------------------------------------------
{
  albaIndent indent(tabs);
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
albaTimeStamp albaTimeMapScalar<T>::GetItemTime(int idx)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it = FindItemByIndex(idx);
  return (it != m_TimeMap.end()) ? it->first : -1;
}

//----------------------------------------------------------------------------
template <class T>
typename albaTimeMapScalar<T>::TimeMapScalars::iterator albaTimeMapScalar<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if ((unsigned int)idx<m_TimeMap.size() && idx>=0)
  {
     typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=m_TimeMap.begin();
     std::advance(it,idx);
     return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
albaID albaTimeMapScalar<T>::FindItemIndex(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindItem(t);
  return it!=m_TimeMap.end()?std::distance(m_TimeMap.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T albaTimeMapScalar<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemByIndex(idx);
  return (it!=m_TimeMap.end())?it->second:ALBA_SCALAR_MIN;
}

//----------------------------------------------------------------------------
template <class T>
T albaTimeMapScalar<T>::GetItem(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second:ALBA_SCALAR_MIN;
}
//----------------------------------------------------------------------------
template <class T>
T albaTimeMapScalar<T>::GetNearestItem(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second:ALBA_SCALAR_MIN;
}
//----------------------------------------------------------------------------
template <class T>
T albaTimeMapScalar<T>::GetItemBefore(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  typename albaTimeMapScalar<T>::TimeMapScalars::iterator it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second:ALBA_SCALAR_MIN;
}
#endif
