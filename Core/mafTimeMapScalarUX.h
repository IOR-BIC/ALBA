/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMapScalarUX.h,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:16:59 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafTimeMapScalar_h
#define __mafTimeMapScalar_h

#define MAF_SCALAR_MIN -1.0e+299
#define MAF_SCALAR_MAX 1.0e+299

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafTimeStamped.h"

#include "mafMTime.h"
#include "mafString.h"
#include <map>
#include <vector>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<mafTimeStamp> mmuTimeVectorScalars;

/** a dynamic associative sorted array of scalars indexed by their "timestamp".
This class wraps an STL set (associative sorted array) of timestamped scalars,
indexed by "timestamp". Its implementation depends on the undergoing data structure
(currently an STL "vector", next release could be based on "map" or "hash_map"). 
Some utility functions are also provided.
@todo
-
*/

template <class T>
class MAF_EXPORT mafTimeMapScalar : public mafObject, public mafTimeStamped
{
public:
  typedef std::map<mafTimeStamp, T > TimeMapScalars;
  typedef std::pair<mafTimeStamp, T > mmuTimePairScalars;

  mafTimeMapScalar();  
  virtual ~mafTimeMapScalar();

  mafTimeMapScalar(const mafTimeMapScalar<T>&);  
  void operator=(const mafTimeMapScalar<T>&);

  /** set the TypeName of the kind of item accepted by this container */
  void SetItemTypeName(const char *tname) {m_ItemTypeName = tname;}
  const char *GetItemTypeName() {return m_ItemTypeName;}
  
  /**
    Insert an item to the vector trying to append it, anyway the array
    is kept sorted. */
  virtual void AppendItem(mafTimeStamp t, T m);

  /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  virtual void PrependItem(mafTimeStamp t, T m);

  /** Find an item index given its pointer*/
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator FindItemByValue(T m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(mafTimeStamp t, T m);
  
  /** Remove an item given its iterator */
  virtual void RemoveItem(typename mafTimeMapScalar<T>::TimeMapScalars::iterator it);

  /** Remove an item given its index*/
  int RemoveItem(int idx);

  /** Remove all the items*/
  virtual void RemoveAllItems();

  /** Return the timestamp of the i-th item*/
  mafTimeStamp GetItemTime(int idx);

  /** Return the list of timestamp of the key scalars in the given vector*/
  void GetTimeStamps(mmuTimeVectorScalars &kframes) const;

  /** Return the number of ITEMS stored in this object*/
  int GetNumberOfItems() const {return m_TimeMap.size();};
  
  /** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
  void GetTimeBounds(mafTimeStamp tbounds[2]);

  /** Copy data from another array*/
  void DeepCopy(mafTimeMapScalar *vitem);

  /**
  Compare two different arrays for equality. They are considered
  equivalent if their key scalars are equivalent.*/
  bool Equals(mafTimeMapScalar *vmat);

  /** Find the item with the timestamp nearest to t*/
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator FindNearestItem(mafTimeStamp t);

  /** Find the item with timestamp <=t*/
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator FindItemBefore(mafTimeStamp t);

  /**
  Find the item with the timestamp==t. Returns the item index, and 
  set "item" to its pointer. Return iterator.end() if not found.*/
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator FindItem(mafTimeStamp t);

  /** return iterator of item with given index */
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator FindItemByIndex(int idx);

  /** return index of the given item. return -1 if not found. */
  mafID FindItemIndex(mafTimeStamp t);

  /** find and return item corresponding to timestamp t. return NULL if not found. */
  T GetItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T GetNearestItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T GetItemBefore(mafTimeStamp t);

  /** return  the item with given its order index. NULL is returned if not found. */
  T GetItemByIndex(int idx);

  virtual void Print(std::ostream& os, const int tabs=0) const;

  typename mafTimeMapScalar<T>::TimeMapScalars::iterator BeginScalarVector() {return m_TimeMap.begin();}
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator EndScalarVector() {return m_TimeMap.end();}
  typename mafTimeMapScalar<T>::TimeMapScalars::iterator LastScalarVector() {return --(m_TimeMap.end());}

protected:
  TimeMapScalars  m_TimeMap;        ///< the set storing the scalars
  mafString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};
#endif
