/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafScalarVector.h,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:20:27 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafScalarVector_h
#define __mafScalarVector_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafTimeMapScalar.txx"
#include "mafStorable.h"
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


/** a dynamic associative sorted vector of scalar values indexed by their "timestamp".*/
class MAF_EXPORT mafScalarVector : public mafTimeMapScalar<double>, public mafStorable
{
public:
  typedef mafTimeMapScalar<double>::TimeMapScalars::iterator Iterator;

  mafScalarVector();
  virtual ~mafScalarVector();

  // set as parent mafObject since type macro does not work for template classes
  mafTypeMacro(mafScalarVector, mafObject); 
  
  /**
  Append a scalar to the vector setting its timestamp to the highest one + 1
  The scalar value is <U>referenced</U> and <B>NOT</B> <U>copied</U>. same effect of AppendItem(). */
  void AppendScalar(mafTimeStamp t, double s) {AppendItem(t, s);};
  
  /** 
    Return a scalar in the vector, given its order index. Return NULL
    if idx out of range. Beware this function take o(n) n = idx to return the scalar value*/
  double GetScalar(int idx) {return GetItemByIndex(idx);}

  /** return a vector of key scalar. */
  void GetScalarVector(std::vector<double> &svector);

  /** Return the number of scalar values stored in this object*/
  int GetNumberOfScalars() const {return GetNumberOfItems();}
  
  /** return the scalar with the timestamp nearest to t*/
  double GetNearestScalar(mafTimeStamp t) {return GetNearestItem(t);}
  
  /** return the scalar with timestamp <=t */
  double GetScalarBefore(mafTimeStamp t) {return GetItemBefore(t);}
  
  /** return the scalar with the timestamp == t. return NULL if not found.*/
  double GetScalar(mafTimeStamp t) {return GetItem(t);}

  /**
    Set the scalar for a specified time. If no key-scalar with the same time exist
    a new key-scalar is created. If a key-scalar with the same time exist, simply
    overwrite the old scalar value. The given scalar value is referenced and
    not copied.*/
  void SetScalar(mafTimeStamp t, double sca) {InsertItem(t, sca);}

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};
#endif
