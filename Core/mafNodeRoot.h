/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:27:14 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafNodeRoot_h
#define __mafNodeRoot_h

#include "mafNode.h"

/** mafNodeRoot - this class represent the root of a MAF tree
  mafNodeRoot is a specialized node, it has a NULL parent node but 
  has a listener attached to it. The listener is typically the object
  responsible for creating the tree.
  @sa  mafNode
*/
class MAF_EXPORT mafNodeRoot : public mafNode
{
public:
  mafTypeMacro(mafNodeRoot,mafNode);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs);

  /** Return the pointer to the parent node (if present)*/
  mafNode *GetParent() {return NULL;};

  /** 
    Return highest NodeId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  mafID GetMaxNodeId() {return m_MaxNodeId;}

  /** Return next available NodeId and increment the internal counter.*/
  mafID GetNextNodeId() { this->Modified();return ++m_MaxNodeId;}

  /** 
    Return highest ItemId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  mafID GetMaxItemId() {return m_MaxItemId;}

  /** Return next available ItemId and increment the internal counter.*/
  mafID GetNextItemId() { this->Modified();return ++m_MaxItemId;}

  /**
    Set the ItemMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxItemId(mafID id) { m_MaxItemId=id; Modified();}
  void ResetMaxItemId() { this->SetMaxItemId(0);}

  /**
    Set the NodeMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxNodeId(mafID id) { m_MaxNodeId=id; Modified();}
  void ResetMaxNodeId() { this->SetMaxNodeId(0);}

  /**
    Reimplement the corresponding mafNode function: since root nodes cannot be
    reparented, this function instead of reparenting, creates a new VME and then
    import all the children in the new VME.*/
  int ReparentTo(mafNode *parent);

  /** Clean the tree and all the data structures of this root*/
  void CleanTree();

protected:
  mafNodeRoot();
  virtual ~mafNodeRoot();

  mafID m_MaxItemId;
  mafID m_MaxNodeId;
  mafObserver *m_Observer;

private:
  mafNodeRoot(const mafNodeRoot&); // Not implemented
  void operator=(const mafNodeRoot&); // Not implemented

  /**
  Hidden VME functions: the root node does not have items,
  its a simple grouping node, and does not have a parent...*/
  
};

#endif
