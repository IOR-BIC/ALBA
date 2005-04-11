/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeGeneric.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 16:40:23 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafNodeGeneric_h
#define __mafNodeGeneric_h

#include "mafNode.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafNodeGeneric - a VME is a concrete node implementing basic features.
  Just a concratization of mafNode.
  @sa mafNodeGeneric mafNode
  */
class MAF_EXPORT mafNodeGeneric : public mafNode
{
public:
  mafTypeMacro(mafNodeGeneric,mafNode);

  /** Clean the tree and all the data structures of this root*/
  void CleanTree() { \
    mafNode::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:  
protected:
  mafNodeGeneric() {}
  virtual ~mafNodeGeneric() {}

private:
  mafNodeGeneric(const mafNodeGeneric&); // Not implemented
  void operator=(const mafNodeGeneric&); // Not implemented
};

#endif
