/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNodeRoot.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:03:35 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __mafNodeRoot_h
#define __mafNodeRoot_h

#include "mafDefines.h"
#include "mafEventSender.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafNode;
class mafObject;
class mafObserver;
class mafStorageElement;
class mafStorage;

/** mafNodeRoot - this class represent an interface for the root nodes of a MAF tree
  mafNodeRoot is an interface class to specialize node to become a root node. It forces
  a NULL parent node and support a Listener object to forward up events coming from
  the tree. The listener is typically the object responsible for creating the tree.
  The root node is also responible for generating Ids for new nodes attached to the
  tree.
  @sa  mafNode
*/
class MAF_EXPORT mafNodeRoot: public mafEventSender
{
public:

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** 
    Return highest NodeId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  mafID GetMaxNodeId() {return m_MaxNodeId;}

  /** Return next available NodeId and increment the internal counter.*/
  mafID GetNextNodeId() { return ++m_MaxNodeId;}

  /**
    Set the NodeMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxNodeId(mafID id) { m_MaxNodeId=id;}
  void ResetMaxNodeId() { this->SetMaxNodeId(0);}

  /** 
    root node cannot be reparented. Root nodes should redefine CanReparent
    to call this function. */
  virtual bool CanReparentTo(mafNode *parent) {return parent==NULL;}

  static mafNodeRoot* SafeDownCast(mafObject *o);

  /** process event for the root node */
  void OnRootEvent(mafEventBase *e);

  mafStorage *GetStorage();
  
protected:
  mafNodeRoot();
  virtual ~mafNodeRoot();

  virtual int StoreRoot(mafStorageElement *parent);
  virtual int RestoreRoot(mafStorageElement *element);

  mafID       m_MaxNodeId; ///< Counter for node Ids

private:
  mafNodeRoot(const mafNodeRoot&); // Not implemented
  void operator=(const mafNodeRoot&); // Not implemented
  
};

#endif
