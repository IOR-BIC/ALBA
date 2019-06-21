/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaScalarVector
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaScalarVector_h
#define __albaScalarVector_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaTimeMapScalar.h"
#include "albaStorable.h"
#include "albaStorageElement.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,albaTimeStamp, double);
#endif


/** a dynamic associative sorted vector of scalar values indexed by their "timestamp".*/
class ALBA_EXPORT albaScalarVector : public albaTimeMapScalar<double>, public albaStorable
{
public:
  typedef albaTimeMapScalar<double>::TimeMapScalars::iterator Iterator;

  albaScalarVector();
  virtual ~albaScalarVector();

  // set as parent albaObject since type macro does not work for template classes
  albaTypeMacro(albaScalarVector, albaObject); 
  
  /**
  Append a scalar to the vector setting its timestamp to the highest one + 1
  The scalar value is <U>referenced</U> and <B>NOT</B> <U>copied</U>. same effect of AppendItem(). */
  void AppendScalar(albaTimeStamp t, double s) {AppendItem(t, s);};
  
  /** 
    Return a scalar in the vector, given its order index. Return NULL
    if idx out of range. Beware this function take o(n) n = idx to return the scalar value*/
  double GetScalar(int idx) {return GetItemByIndex(idx);}

  /** return a vector of key scalar. */
  void GetScalarVector(std::vector<double> &svector);

  /** Return the number of scalar values stored in this object*/
  int GetNumberOfScalars() const {return GetNumberOfItems();}
  
  /** return the scalar with the timestamp nearest to t*/
  double GetNearestScalar(albaTimeStamp t) {return GetNearestItem(t);}
  
  /** return the scalar with timestamp <=t */
  double GetScalarBefore(albaTimeStamp t) {return GetItemBefore(t);}
  
  /** return the scalar with the timestamp == t. return NULL if not found.*/
  double GetScalar(albaTimeStamp t) {return GetItem(t);}

  /**
    Set the scalar for a specified time. If no key-scalar with the same time exist
    a new key-scalar is created. If a key-scalar with the same time exist, simply
    overwrite the old scalar value. The given scalar value is referenced and
    not copied.*/
  void SetScalar(albaTimeStamp t, double sca) {InsertItem(t, sca);}

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
};
#endif
