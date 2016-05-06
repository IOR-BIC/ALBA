/*=========================================================================

 Program: MAF2
 Module: mafRoot
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
 
#ifndef __mafRoot_h
#define __mafRoot_h

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafEventSender.h"
#include "mafVMEOutputNULL.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVME;
class mafObject;
class mafObserver;
class mafStorageElement;
class mafStorage;

/** mafRoot - this class represent an interface for the root nodes of a MAF tree
  mafRoot is an interface class to specialize node to become a root node. It forces
  a NULL parent node and support a Listener object to forward up events coming from
  the tree. The listener is typically the object responsible for creating the tree.
  The root node is also responible for generating Ids for new nodes attached to the
  tree.
  @sa  mafVME
*/
class MAF_EXPORT mafRoot: public mafEventSender
{
public:

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

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
  virtual bool CanReparentTo(mafVME *parent) {return parent==NULL;}

  static mafRoot* SafeDownCast(mafObject *o);

  /** process event for the root node */
  void OnRootEvent(mafEventBase *e);

  mafStorage *GetStorage();
  
protected:
  mafRoot();
  virtual ~mafRoot();

  virtual int StoreRoot(mafStorageElement *parent);
  virtual int RestoreRoot(mafStorageElement *element);

  mafID       m_MaxNodeId; ///< Counter for node Ids

private:
  mafRoot(const mafRoot&); // Not implemented
  void operator=(const mafRoot&); // Not implemented
  
};

#endif
