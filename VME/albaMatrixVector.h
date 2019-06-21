/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMatrixVector
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaMatrixVector_h
#define __albaMatrixVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaTimeMap.h"
#include "albaStorable.h"
#include "albaStorageElement.h"
#include "albaMatrix.h"

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,albaTimeStamp, albaAutoPointer<albaMatrix>);
#endif
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** a dynamic associative sorted vector of 4x4 matrices indexed by their "timestamp".
  
  @todo
  -
*/
class ALBA_EXPORT albaMatrixVector : public albaTimeMap<albaMatrix>, public albaStorable
{
public:
  typedef albaTimeMap<albaMatrix>::TimeMap::iterator Iterator;

  albaMatrixVector();  
  virtual ~albaMatrixVector();

  // set as parent albaObject since type macro does not work for template classes
  albaTypeMacro(albaMatrixVector,albaObject); 
  
  /**
    Append a matrix to the vector setting its timestamp to the highest one + 1
    The matrix content is copied and not referenced.*/
  void AppendKeyMatrix(const albaMatrix &m);
  /**
    Append a matrix to the vector setting its timestamp to the highest one + 1
    The matrix object is <U>referenced</U> and <B>NOT</B> <U>copied</U>. same effect of AppendItem(). */
  void AppendKeyMatrix(albaMatrix *m) {AppendItem(m);}

  /** 
    Return a matrix in the vector, given its order index. Return NULL
    if idx out of range. Beware this function take o(n) n=idx to return the matrix */
  albaMatrix *GetKeyMatrix(int idx) {return GetItemByIndex(idx);}

  /** return a vector of key matrices. */
  void GetKeyMatrixVector(std::vector<albaMatrix *> &mvector);

  /** Return the number of matrixes stored in this object*/
  int GetNumberOfMatrices() const {return GetNumberOfItems();}
  
  /** return the matrix with the timestamp nearest to t*/
  albaMatrix *GetNearestMatrix(albaTimeStamp t) {return GetNearestItem(t);}
  /** return the matrix with timestamp <=t */
  albaMatrix *GetMatrixBefore(albaTimeStamp t) {return GetItemBefore(t);}
  /** return  the matrix with the timestamp==t. return NULL if not found.*/
  albaMatrix *GetMatrix(albaTimeStamp t) {return GetItem(t);}

  /**
    Set the matrix for a specified time. If no key-matrix with the same time exist
    a new key-matrix is created. If a key-matrix with the same time exist, simply
    overwrite the old 4x4 Matrix value. The matrix content is always <U>copied<U> and
    <B>NOT</B> <U>referenced</U>.*/
  void SetMatrix(const albaMatrix &mat);
  
  /**
    Set the matrix for a specified time. If no key-matrix with the same time exist
    a new key-matrix is created. If a key-matrix with the same time exist, simply
    overwrite the old 4x4 Matrix value. The given matrix object is referenced and
    not copied.*/
  void SetMatrix(albaMatrix *mat) {InsertItem(mat);}

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
};

#endif
