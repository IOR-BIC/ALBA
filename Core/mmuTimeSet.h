/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuTimeSet.h,v $
  Language:  C++
  Date:      $Date: 2005-03-11 15:47:50 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmuTimeSet_h
#define __mmuTimeSet_h

#include "mmuUtility.h"
#include "mafDefines.h"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<mafTimeStamp> mmuTimeVector;

//------------------------------------------------------------------------------
// mmuTimeSet
//------------------------------------------------------------------------------
/** class wrapping an STL "set" of mafTimeStamps providing API to query and merge.
  mmuTimeSet 
  @todo
  - test Merge of variuos case of time vectors
*/
class MAF_EXPORT mmuTimeSet: public mmuUtility
{
public:
  mmuTimeSet();
  virtual ~mmuTimeSet();

//  mafTypeMacro(mmuTimeSet,mmuUtility);
 
  /** copy constructor, this makes a copy of the set */
  mmuTimeSet(const mmuTimeSet& c);
 
  /** assign operator, to copy set value from another set */
  mmuTimeSet &operator=(const mmuTimeSet &o);

  bool operator==(const mmuTimeSet &o);

  typedef std::set<mafTimeStamp> TSet;
  typedef std::set<mafTimeStamp>::iterator Iterator;
 
  /** insert a timestamp in the set */
  void Insert(mafTimeStamp t);

  /** 
    insert a timestamp forcing insertion point search to start
    from the end of the set. In case times are inserted already 
    ordered this ensures a constant time for insertion */
  void Append(mafTimeStamp t);

  /** 
    insert a timestamp forcing insertion point search to start
    from the start of the set. In case times are inserted already 
    ordered this ensures a constant time for insertion */
  void Prepend(mafTimeStamp t);

  /** remove timestamp from the set*/
  void Remove(Iterator ts);

  /** remove all items from the internal time set */
  void Clear() {m_TSet.clear();}

  /** return reference to inner TSet object */
  TSet &GetTSet() {return m_TSet;}

  /** use this in case you have a to access the TSet of a const mmuTimeSet */ 
  const TSet &GetConstTSet() const{return m_TSet;}

  /**  Return the idx-th item of the set. return -1 if not found. */
  mafTimeStamp GetByIndex(mafID idx);
  
  /** Return the timestamp with value clsest to t */
  mafTimeStamp GetNearestTimeStamp(mafTimeStamp t);

  /** Return timestamp with value <= t */
  mafTimeStamp GetTimeStampBefore(mafTimeStamp t);

  /** return index of the given timestamp. return -1 if not found. */
  mafID FindTimeStampIndex(mafTimeStamp t);

  /** 
    Return the iterator pointing to the item with value "t". Return
    TSet::end() if not found.*/
  Iterator FindTimeStamp(mafTimeStamp t);
  
  /** 
    Return the iterator pointing to the item with value "t". Return
    TSet::end() if none found.*/
  Iterator FindNearestTimeStamp(mafTimeStamp t);

  /** 
    Return the iterator pointing to the item with lower or equal to "t". Return
    TSet::end() if none found.*/
  Iterator FindTimeStampBefore(mafTimeStamp t);

  /** Merge two different time-stamps sets*/
  static void Merge(const mmuTimeSet &v1,const mmuTimeSet &v2,mmuTimeSet &outv);

  /** Merge a time-stamps set into this set*/
  void Merge(const mmuTimeSet &v);

  /** Merge two different time-stamps vectors*/
  static void Merge(const mmuTimeVector &v1,const mmuTimeVector &v2,mmuTimeVector &outv);

  /** Merge a time-stamps vector into this set*/
  void Merge(const mmuTimeVector &v);

  /** return the number of time stamps */
  int GetNumberOfTimeStamps() const;

  /** return the first time stamp */
  Iterator Begin() {return m_TSet.begin();}
  /** return the last+1 time stamp */
  Iterator End() {return m_TSet.end();}
  

protected:
  TSet m_TSet;
};

#endif /* __mmuTimeSet_h */
