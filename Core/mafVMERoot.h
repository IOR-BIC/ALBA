/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERoot.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:25:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMERoot_h
#define __mafVMERoot_h

#include "mafVME.h"
#include "mafNodeRoot.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafVMERoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from mafNodeRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa mafNodeRoot mafVME

  @todo
  - 
  */
class MAF_EXPORT mafVMERoot : public mafVME
{
public:
  mafTypeMacro(mafVMERoot,mafVME);

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

  /** Clean the tree and all the data structures of this root*/
  void CleanTree() { \
    mafNode::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs);

protected:
  mafID m_MaxItemId;
private:
  mafVMERoot(const mafVMERoot&); // Not implemented
  void operator=(const mafVMERoot&); // Not implemented
};

#endif
