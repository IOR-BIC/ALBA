/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmuTSet.h,v $
  Language:  C++
  Date:      $Date: 2005-02-25 07:24:16 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmuTSet_h
#define __mmuTSet_h

#include "mmuUtility.h"
#include "mafDefines.h"
#include <set>

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// mmuTSet
//------------------------------------------------------------------------------
/** class wrapping an STL "set" of mafTimeStamps providing API to query and merge.
  mmuTSet 
  @sa mafTVector
*/
class MAF_EXPORT mmuTSet: public mmuUtility
{
public:
  mmuTSet();
  virtual ~mmuTSet();

//  mafTypeMacro(mmuTSet,mmuUtility);
 
  /** copy constructor, this makes a copy of the set */
  mmuTSet(const mmuTSet& c);
 
  /** assign operator, to copy set value from another set */
  mmuTSet &operator=(const mmuTSet &o);

  typedef std::set<mafTimeStamp> TSet;
  typedef std::set<mafTimeStamp>::iterator Iterator;
 
  /** insert a timestamp in the set */
  void Insert(mafTimeStamp t);

  /** remove timestamp from the set*/
  void Remove(Iterator ts);

  /** return reference to inner TSet object */
  TSet &GetTSet() {return m_TSet;}

  /**  Return the idx-th item of the set */
  mafTimeStamp GetByIndex(mafID idx);
  
  /** Return the timestamp with value clsest to t */
  mafTimeStamp GetNearestTimeStamp(mafTimeStamp t);

  /** Return timestamp with value <= t */
  mafTimeStamp GetLowerTimeStamp(mafTimeStamp t);

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
  Iterator FindLowerTimeStamp(mafTimeStamp t);

protected:
  TSet m_TSet;
};

#endif /* __mmuTSet_h */
