/*=========================================================================

 Program: MAF2
 Module: mafNodeGeneric
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
