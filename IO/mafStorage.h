/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafStorage.h,v $
  Language:  C++
  Date:      $Date: 2004-12-27 18:22:26 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafStorage_h__
#define __mafStorage_h__

#include "mafObject.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafStorageElement;

/** mafStorage.
  to be written
 */  
class mafStorage
{
public:
  /** perform storing. the argument is the tag of the root node */
  void Store(const char *root_tag);
  
  /** perform restoring. the argument is the tag of the root node  */
  int Restore(const char *root_tag);
  
  /** set the root element to be stored */
  void SetRoot (mafStorable *root);
    
protected:
  /**
    This is called by Store() and must be reimplemented by subclasses. */
  virtual void InternalStore(const char *root_tag)=0;

  /** 
    This is called by Restore() and must be reimplemented by subclasses. */
  virtual int InternalRestore(const char *root_tag)=0;

  mafStorable *m_Root;  ///< root object to be stored, or being restored
  mafStorageElement *m_RootElement; ///< root stored element
};
#endif // _mafStorage_h_
