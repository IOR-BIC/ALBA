/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataVector.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:25:25 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafDataVector_h
#define __mafDataVector_h

#include "mafTimeMap.txx"
#include "mafStorable.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------

/** a dynamic associative sorted array of datasets indexed by their "timestamp".
  
  @todo
  -
*/
template <class T>
class MAF_EXPORT mafDataVector : public mafTimeMap<T>, public mafStorable
{
public:
  mafDataVector();  
  virtual ~mafDataVector();
protected:
  
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};

#endif
