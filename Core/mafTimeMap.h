/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafTimeMap.h,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:44:41 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafTimeMap_h
#define __mafTimeMap_h

#include "mafObject.h"
#include "mafSmartPointer.h"
#include "mafMTime.h"
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
class MAF_EXPORT mafTimeMap : public mafObject
{
public:

  typedef std::map<mafTimeStamp,mafAutoPointer<T> > TimeMap;
  typedef std::pair<mafTimeStamp,mafAutoPointer<T> > mmuTimePair;

  mafTimeMap();  
  virtual ~mafTimeMap();

  mafTimeMap(const mafTimeMap<T>&);  
  void operator=(const mafTimeMap<T>&);

  //mafAbstractTypeMacro(mafTimeMap<T>,mafObject);
  
  /**
    Insert an item to the vector trying to append it, anyway the array is kept sorted.
     */
  void AppendItem(T *m);

  /** append item setting its timestamp to the highest one + 1 */
  void AppendAndSetItem(T *m);

   /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  void PrependItem(T *m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  int InsertItem(T *item);

  /** Find an item index given its pointer*/
  mafTimeMap<T>::TimeMap::iterator FindItem(T *m);

  /** Remove an item given its iterator */
  void RemoveItem(mafTimeMap<T>::TimeMap::iterator it);

  /** Remove an item given its index*/
  int RemoveItem(int idx);

  /** Remove all the items*/
  void RemoveAllItems();

  /** Return the timestamp of the i-th item*/
  mafTimeStamp GetItemTime(int idx);

  /** Return the list of timestamp of the key matrixes in the given vector*/
  void GetTimeStamps(mmuTimeVector &kframes);

  /**
    Return the list of timestamp of the key matrixes in the given vector. Beware, this function
    allocated memory that must be release explicitly. The array pointer is overwritten, thus
    beware of possible memory leaks.*/
  void GetTimeStamps(mafTimeStamp *&kframes);
  
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

  /** increment update modification time */
  void Modified() {m_MTime.Modified();}

  /** return modification time */
  unsigned long GetMTime() {return m_MTime.GetMTime();}

  mafTimeMap<T>::TimeMap::iterator Begin() {return m_TimeMap.begin();}

  mafTimeMap<T>::TimeMap::iterator End() {return m_TimeMap.end();}

protected:

  TimeMap        m_TimeMap;  ///< the set storing the datasets
  mafMTime       m_MTime;    ///< Last modification time
};

#endif
