/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMap.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:06:43 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafTimeMap_h
#define __mafTimeMap_h

#include "mafObject.h"
#include "mafTimeStamped.h"
#include "mafSmartPointer.h"
#include "mafMTime.h"
#include "mafString.h"
#include <map>
#include <vector>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<mafTimeStamp> mmuTimeVector;

/** a dynamic associative sorted array of timestamped objects indexed by their "timestamp".
  This class wraps an STL set (associative sorted array) of timestamped datasets,
  indexed by "timestamp". Its implementation depends on the undergoing data structure
  (currently an STL "vector", next release could be based on "map" or "hash_map"). 
  Some utility functions are also provided.
  @todo
  -
*/

template <class T>
class MAF_EXPORT mafTimeMap : public mafObject, public mafTimeStamped
{
public:

  typedef std::map<mafTimeStamp,mafAutoPointer<T> > TimeMap;
  typedef std::pair<mafTimeStamp,mafAutoPointer<T> > mmuTimePair;

  mafTimeMap();  
  virtual ~mafTimeMap();

  mafTimeMap(const mafTimeMap<T>&);  
  void operator=(const mafTimeMap<T>&);

  //mafAbstractTypeMacro(mafTimeMap<T>,mafObject);

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

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(T *m);

  /** Find an item index given its pointer*/
  mafTimeMap<T>::TimeMap::iterator FindItem(T *m);

  /** Remove an item given its iterator */
  virtual void RemoveItem(mafTimeMap<T>::TimeMap::iterator it);

  /** Remove an item given its index*/
  int RemoveItem(int idx);

  /** Remove all the items*/
  virtual void RemoveAllItems();

  /** Return the timestamp of the i-th item*/
  mafTimeStamp GetItemTime(int idx);

  /** Return the list of timestamp of the key matrixes in the given vector*/
  void GetTimeStamps(mmuTimeVector &kframes) const;

  /** Return the number of ITEMS stored in this object*/
  int GetNumberOfItems() const {return m_TimeMap.size();};
  
  /** Set/Get the Current time for this object*/
  //mafTimeStamp GetCurrentTime() {return m_CurrentTime;}
  //void SetCurrentTime(mafTimeStamp t);

  /** Return the time bounds for this vector, i.e. minimum and maximum time stamps */
  void GetTimeBounds(mafTimeStamp tbounds[2]);

  /** Copy data from another array*/
  void DeepCopy(mafTimeMap *vitem);

  /**
    Compare two different arrays for equality. They are considered
    equivalent if their key matrixes are equivalent.*/
  bool Equals(mafTimeMap *vmat);

  /** Find the item with the timestamp nearest to t*/
  mafTimeMap<T>::TimeMap::iterator FindNearestItem(mafTimeStamp t);

  /** Find the item with timestamp <=t*/
  mafTimeMap<T>::TimeMap::iterator FindItemBefore(mafTimeStamp t);

  /**
    Find the item with the timestamp==t. Returns the item index, and 
    set "item" to its pointer. Return -1 and NULL if not found.
    not*/
  mafTimeMap<T>::TimeMap::iterator FindItem(mafTimeStamp t);

  /** return iterator of item with given index */
  mafTimeMap<T>::TimeMap::iterator FindItemByIndex(int idx);

  /** return index of the given item. return -1 if not found. */
  mafID FindItemIndex(mafTimeStamp t);

  /** find and return item corresponding to timestamp t. return NULL if not found. */
  T *GetItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T *GetNearestItem(mafTimeStamp t);

  /**
    Return the pointer to the item with timestamp nearest the given one. NULL 
    is returned if not found.*/
  T *GetItemBefore(mafTimeStamp t);

  /** return  the item with given its order index. NULL is returned if not found. */
  T *GetItemByIndex(int idx);

  virtual void Print(std::ostream& os, const int tabs=0) const;

  mafTimeMap<T>::TimeMap::iterator Begin() {return m_TimeMap.begin();}
  mafTimeMap<T>::TimeMap::iterator End() {return m_TimeMap.end();}

protected:

  TimeMap         m_TimeMap;        ///< the set storing the datasets
  mafString       m_ItemTypeName;   ///< the name of the item type accepted by this container
};

#endif
