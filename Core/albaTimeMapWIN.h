/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTimeMapWIN
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaTimeMap_h
#define __albaTimeMap_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaObject.h"
#include "albaTimeStamped.h"
#include "albaSmartPointer.h"
#include "albaMTime.h"
#include "albaString.h"
#include <map>
#include <vector>
#include "albaTimeMap.h"
#include "albaIndent.h"
#include <math.h>
#include <assert.h>


//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<albaTimeStamp> mmuTimeVector;

/** a dynamic associative sorted array of timestamped objects indexed by their "timestamp".
  This class wraps an STL set (associative sorted array) of timestamped datasets,
  indexed by "timestamp". Its implementation depends on the undergoing data structure
  (currently an STL "vector", next release could be based on "map" or "hash_map"). 
  Some utility functions are also provided.
  @todo
  -
*/

template <class T>
class albaTimeMap : public albaObject, public albaTimeStamped
{
public:
  typedef std::map<albaTimeStamp, albaAutoPointer<T> > TimeMap;
  typedef std::pair<albaTimeStamp, albaAutoPointer<T> > mmuTimePair;

  albaTimeMap();  
  virtual ~albaTimeMap();

  albaTimeMap(const albaTimeMap<T>&){};  
  void operator=(const albaTimeMap<T>&){};

  //albaAbstractTypeMacro(albaTimeMap<T>,albaObject);

  /** set the TypeName of the kind of item accepted by this container */
  void SetItemTypeName(const char *tname) {m_ItemTypeName=tname;}
  const char *GetItemTypeName() {return m_ItemTypeName;}
  
  /**
    Insert an item to the vector trying to append it, anyway the array
    is kept sorted. */
  virtual void AppendItem(T *m);

  /** append item setting its timestamp to the highest one + 1 */
  virtual void AppendAndSetItem(T *m);

   /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  virtual void PrependItem(T *m);

  /** Find an item index given its pointer*/
  typename albaTimeMap<T>::TimeMap::iterator FindItem(T *m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(T *m);
  
  /** Remove an item given its iterator */
  virtual void RemoveItem(typename albaTimeMap<T>::TimeMap::iterator it);

  /** Remove an item given its index*/
  int RemoveItem(int idx);

  /** Remove all the items*/
  virtual void RemoveAllItems();

  /** Return the timestamp of the i-th item*/
  albaTimeStamp GetItemTime(int idx);

  /** Return the list of timestamp of the key matrixes in the given vector*/
  void GetTimeStamps(mmuTimeVector &kframes) const;

  /** Return the number of ITEMS stored in this object*/
  int GetNumberOfItems() const {return m_TimeMap.size();};
  
  /** Set/Get the Current time for this object*/
  //albaTimeStamp GetTimeStamp() {return m_CurrentTime;}
  //void SetTimeStamp(albaTimeStamp t);

  /** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
  void GetTimeBounds(albaTimeStamp tbounds[2]);

  /** Copy data from another array*/
  void DeepCopy(albaTimeMap *vitem);

  /**
    Compare two different arrays for equality. They are considered
    equivalent if their key matrixes are equivalent.*/
  bool Equals(albaTimeMap *vmat);

  /** Find the item with the timestamp nearest to t*/
  typename albaTimeMap<T>::TimeMap::iterator FindNearestItem(albaTimeStamp t);

  /** Find the item with timestamp <=t*/
  typename albaTimeMap<T>::TimeMap::iterator FindItemBefore(albaTimeStamp t);

  /**
    Find the item with the timestamp==t. Returns the item index, and 
    set "item" to its pointer. Return -1 and NULL if not found.
    not*/
  typename albaTimeMap<T>::TimeMap::iterator FindItem(albaTimeStamp t);

  /** return iterator of item with given index */
  typename albaTimeMap<T>::TimeMap::iterator FindItemByIndex(int idx);

  /** return index of the given item. return -1 if not found. */
  albaID FindItemIndex(albaTimeStamp t);

  /** find and return item corresponding to timestamp t. return NULL if not found. */
  T *GetItem(albaTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T *GetNearestItem(albaTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T *GetItemBefore(albaTimeStamp t);

  /** return  the item with given its order index. NULL is returned if not found. */
  T *GetItemByIndex(int idx);

  virtual void Print(std::ostream& os, const int tabs=0) const;

  typename albaTimeMap<T>::TimeMap::iterator Begin() {return m_TimeMap.begin();}
  typename albaTimeMap<T>::TimeMap::iterator End() {return m_TimeMap.end();}
  typename albaTimeMap<T>::TimeMap::iterator Last() {return --(m_TimeMap.end());}

protected:
  TimeMap         m_TimeMap;        ///< the set storing the datasets
  albaString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};



//////////////////////////////////TEMPLATE  IMPLEMENTATION////////////////////////////

//-----------------------------------------------------------------------
template <class T>
albaTimeMap<T>::albaTimeMap()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
albaTimeMap<T>::~albaTimeMap()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::AppendAndSetItem(T *m)
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
void albaTimeMap<T>::AppendItem(T *m)
//-----------------------------------------------------------------------
{
	assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      albaErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Append item!");
      return;
    }
  }
	m_TimeMap.insert(m_TimeMap.end(),mmuTimePair(m->GetTimeStamp(),m));
	Modified();
}

//-----------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::PrependItem(T *m)
//-----------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      albaErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Prepend item!");
      return;
    }
  }
  m_TimeMap.insert(m_TimeMap.begin(),mmuTimePair(m->GetTimeStamp(),m));
  Modified();
}
//-------------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::InsertItem(T *m)
//-------------------------------------------------------------------------
{
  assert(m);
  if (!m_ItemTypeName.IsEmpty())
  {
    assert(m->IsA(m_ItemTypeName));
    if (!m->IsA(m_ItemTypeName))
    {
      albaErrorMacro("Unsupported Item type \""<<m->GetTypeName()<<"\", allowed type is \""<<m_ItemTypeName<<"\": cannot Insert item!");
      return;
    }
  }

  m_TimeMap[m->GetTimeStamp()]=m;	
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::GetTimeBounds(albaTimeStamp tbounds[2])
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
void albaTimeMap<T>::GetTimeStamps(mmuTimeVector &kframes) const
//-------------------------------------------------------------------------
{
  kframes.clear();

  for (albaTimeMap<T>::TimeMap::const_iterator it=m_TimeMap.begin();it!=m_TimeMap.end();it++)
  {
    kframes.push_back(it->first);
  }
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::DeepCopy(albaTimeMap *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  //m_TimeMap=o->m_TimeMap;
  albaTimeMap<T>::TimeMap::iterator it;
  for (it=o->Begin();it!=o->End();it++)
  {
    T *m=it->second;
    T *new_item=m->NewInstance();
    new_item->DeepCopy(m);
    AppendItem(new_item);
  }
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
bool albaTimeMap<T>::Equals(albaTimeMap *o)
//-------------------------------------------------------------------------
{
  if (o==NULL)
    return false;

  if (GetNumberOfItems()!=o->GetNumberOfItems())
    return false;

  albaTimeMap<T>::TimeMap::iterator it;
  albaTimeMap<T>::TimeMap::iterator it2;
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
void albaTimeMap<T>::RemoveItem(typename albaTimeMap<T>::TimeMap::iterator it)
//-------------------------------------------------------------------------
{
  m_TimeMap.erase(it);
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
int albaTimeMap<T>::RemoveItem(int idx)
//-------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindItemByIndex(idx);
  if (it!=m_TimeMap.end())
  {
    RemoveItem(it);
    return ALBA_OK;
  }

  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::RemoveAllItems()
//-------------------------------------------------------------------------
{
  m_TimeMap.clear();
  Modified();
}

//-------------------------------------------------------------------------
template <class T>
typename albaTimeMap<T>::TimeMap::iterator albaTimeMap<T>::FindNearestItem(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  std::pair<albaTimeMap<T>::TimeMap::iterator,albaTimeMap<T>::TimeMap::iterator> range=m_TimeMap.equal_range(t);
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
typename albaTimeMap<T>::TimeMap::iterator albaTimeMap<T>::FindItemBefore(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  if (m_TimeMap.size()>0)
  {
    albaTimeMap<T>::TimeMap::iterator it=m_TimeMap.lower_bound(t); // find first item >= t
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
typename albaTimeMap<T>::TimeMap::iterator albaTimeMap<T>::FindItem(albaTimeStamp t)
//-------------------------------------------------------------------------
{
  return m_TimeMap.find(t);
}

//----------------------------------------------------------------------------
template <class T>
typename albaTimeMap<T>::TimeMap::iterator albaTimeMap<T>::FindItem(T *m)
//----------------------------------------------------------------------------
{
  assert(m);
  return m_TimeMap.find(m->GetTimeStamp());
}

//----------------------------------------------------------------------------
template <class T>
void albaTimeMap<T>::Print(std::ostream& os, const int tabs) const
//----------------------------------------------------------------------------
{
  albaIndent indent(tabs);
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
albaTimeStamp albaTimeMap<T>::GetItemTime(int idx)
//----------------------------------------------------------------------------
{
  T *m=GetItemByIndex(idx);
  if (m) return m->GetTimeStamp();
  return -1;
}

//----------------------------------------------------------------------------
template <class T>
typename albaTimeMap<T>::TimeMap::iterator albaTimeMap<T>::FindItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  if ((unsigned int)idx<m_TimeMap.size() && idx>=0)
  {
     albaTimeMap<T>::TimeMap::iterator it=m_TimeMap.begin();
     std::advance(it,idx);
     return it;
  }
  
  return m_TimeMap.end();
}

//----------------------------------------------------------------------------
template <class T>
albaID albaTimeMap<T>::FindItemIndex(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindItem(t);
  return it!=m_TimeMap.end()?std::distance(m_TimeMap.begin(),it):-1;
}

//----------------------------------------------------------------------------
template <class T>
T *albaTimeMap<T>::GetItemByIndex(int idx)
//----------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindItemByIndex(idx);
  return (it!=m_TimeMap.end())?it->second:NULL;
}

//----------------------------------------------------------------------------
template <class T>
T *albaTimeMap<T>::GetItem(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *albaTimeMap<T>::GetNearestItem(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindNearestItem(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}
//----------------------------------------------------------------------------
template <class T>
T *albaTimeMap<T>::GetItemBefore(albaTimeStamp t)
//----------------------------------------------------------------------------
{
  albaTimeMap<T>::TimeMap::iterator it=FindItemBefore(t);
  return (it!=m_TimeMap.end())?it->second:NULL;
}


#endif
