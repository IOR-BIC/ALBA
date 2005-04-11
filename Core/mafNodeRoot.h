/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 12:59:56 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafNodeRoot_h
#define __mafNodeRoot_h

#include "mafNode.h"
#include "mafRoot.h"

/** mafNodeRoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from mafNodeRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa mafNodeRoot mafNode */
class MAF_EXPORT mafNodeRoot : public mafNode, public mafRoot
{
public:
  mafTypeMacro(mafNodeRoot,mafNode);

  /** Clean the tree and all the data structures of this root*/
  void CleanTree() { \
    mafNode::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** allow only a NULL parent */
  virtual bool CanReparentTo(mafNode *parent) {return mafRoot::CanReparentTo(parent);}

  void OnEvent(mafEventBase *e);

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:  
protected:
  mafNodeRoot() {}
  virtual ~mafNodeRoot() {}

private:
  mafNodeRoot(const mafNodeRoot&); // Not implemented
  void operator=(const mafNodeRoot&); // Not implemented
};

#endif
