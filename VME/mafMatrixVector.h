/*=========================================================================

 Program: MAF2
 Module: mafMatrixVector
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafMatrixVector_h
#define __mafMatrixVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafTimeMap.txx"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mafMatrix.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,mafTimeStamp, mafAutoPointer<mafMatrix>);
#endif
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** a dynamic associative sorted vector of 4x4 matrices indexed by their "timestamp".
  
  @todo
  -
*/
class MAF_EXPORT mafMatrixVector : public mafTimeMap<mafMatrix>, public mafStorable
{
public:
  typedef mafTimeMap<mafMatrix>::TimeMap::iterator Iterator;

  mafMatrixVector();  
  virtual ~mafMatrixVector();

  // set as parent mafObject since type macro does not work for template classes
  mafTypeMacro(mafMatrixVector,mafObject); 
  
  /**
    Append a matrix to the vector setting its timestamp to the highest one + 1
    The matrix content is copied and not referenced.*/
  void AppendKeyMatrix(const mafMatrix &m);
  /**
    Append a matrix to the vector setting its timestamp to the highest one + 1
    The matrix object is <U>referenced</U> and <B>NOT</B> <U>copied</U>. same effect of AppendItem(). */
  void AppendKeyMatrix(mafMatrix *m) {AppendItem(m);}

  /** 
    Return a matrix in the vector, given its order index. Return NULL
    if idx out of range. Beware this function take o(n) n=idx to return the matrix */
  mafMatrix *GetKeyMatrix(int idx) {return GetItemByIndex(idx);}

  /** return a vector of key matrices. */
  void GetKeyMatrixVector(std::vector<mafMatrix *> &mvector);

  /** Return the number of matrixes stored in this object*/
  int GetNumberOfMatrices() const {return GetNumberOfItems();}
  
  /** return the matrix with the timestamp nearest to t*/
  mafMatrix *GetNearestMatrix(mafTimeStamp t) {return GetNearestItem(t);}
  /** return the matrix with timestamp <=t */
  mafMatrix *GetMatrixBefore(mafTimeStamp t) {return GetItemBefore(t);}
  /** return  the matrix with the timestamp==t. return NULL if not found.*/
  mafMatrix *GetMatrix(mafTimeStamp t) {return GetItem(t);}

  /**
    Set the matrix for a specified time. If no key-matrix with the same time exist
    a new key-matrix is created. If a key-matrix with the same time exist, simply
    overwrite the old 4x4 Matrix value. The matrix content is always <U>copied<U> and
    <B>NOT</B> <U>referenced</U>.*/
  void SetMatrix(const mafMatrix &mat);
  
  /**
    Set the matrix for a specified time. If no key-matrix with the same time exist
    a new key-matrix is created. If a key-matrix with the same time exist, simply
    overwrite the old 4x4 Matrix value. The given matrix object is referenced and
    not copied.*/
  void SetMatrix(mafMatrix *mat) {InsertItem(mat);}

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};

#endif
