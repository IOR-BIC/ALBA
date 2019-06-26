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
#ifndef __mmuTimeSet_h
#define __mmuTimeSet_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaUtility.h"
#include "albaDefines.h"
#include <set>
#include <vector>
#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaTimeStamp);
#endif
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
typedef std::vector<albaTimeStamp> mmuTimeVector;

//------------------------------------------------------------------------------
// mmuTimeSet
//------------------------------------------------------------------------------
/** class wrapping an STL "set" of albaTimeStamps providing API to query and merge.
  mmuTimeSet 
  @todo
  - test Merge of variuos case of time vectors
*/
class ALBA_EXPORT mmuTimeSet: public albaUtility
{
public:
  mmuTimeSet();
  virtual ~mmuTimeSet();

//  albaTypeMacro(mmuTimeSet,albaUtility);
 
  /** copy constructor, this makes a copy of the set */
  mmuTimeSet(const mmuTimeSet& c);
 
  /** assign operator, to copy set value from another set */
  mmuTimeSet &operator=(const mmuTimeSet &o);

  bool operator==(const mmuTimeSet &o);

  typedef std::set<albaTimeStamp> TSet;
  typedef std::set<albaTimeStamp>::iterator Iterator;
 
  /** insert a timestamp in the set */
  void Insert(albaTimeStamp t);

  /** 
    insert a timestamp forcing insertion point search to start
    from the end of the set. In case times are inserted already 
    ordered this ensures a constant time for insertion */
  void Append(albaTimeStamp t);

  /** 
    insert a timestamp forcing insertion point search to start
    from the start of the set. In case times are inserted already 
    ordered this ensures a constant time for insertion */
  void Prepend(albaTimeStamp t);

  /** remove timestamp from the set*/
  void Remove(Iterator ts);

  /** remove all items from the internal time set */
  void Clear() {m_TSet.clear();}

  /** return reference to inner TSet object */
  TSet &GetTSet() {return m_TSet;}

  /** use this in case you have a to access the TSet of a const mmuTimeSet */ 
  const TSet &GetConstTSet() const{return m_TSet;}

  /**  Return the idx-th item of the set. return -1 if not found. */
  albaTimeStamp GetByIndex(albaID idx);
  
  /** Return the timestamp with value clsest to t */
  albaTimeStamp GetNearestTimeStamp(albaTimeStamp t);

  /** Return timestamp with value <= t */
  albaTimeStamp GetTimeStampBefore(albaTimeStamp t);

  /** return index of the given timestamp. return -1 if not found. */
  albaID FindTimeStampIndex(albaTimeStamp t);

  /** 
    Return the iterator pointing to the item with value "t". Return
    TSet::end() if not found.*/
  Iterator FindTimeStamp(albaTimeStamp t);
  
  /** 
    Return the iterator pointing to the item with value "t". Return
    TSet::end() if none found.*/
  Iterator FindNearestTimeStamp(albaTimeStamp t);

  /** 
    Return the iterator pointing to the item with lower or equal to "t". Return
    TSet::end() if none found.*/
  Iterator FindTimeStampBefore(albaTimeStamp t);

  /** Merge two different time-stamps sets*/
  static void Merge(const mmuTimeSet &v1,const mmuTimeSet &v2,mmuTimeSet &outv);

  /** Merge a time-stamps set into this set*/
  void Merge(const mmuTimeSet &v);

  /** Merge two different time-stamps vectors*/
  static void Merge(const std::vector<albaTimeStamp> &v1,const std::vector<albaTimeStamp> &v2,std::vector<albaTimeStamp> &outv);

  /** Merge a time-stamps vector into this set*/
  void Merge(const std::vector<albaTimeStamp> &v);

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
