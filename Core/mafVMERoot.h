/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMERoot.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:03:36 $
  Version:   $Revision: 1.3 $
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
class MAF_EXPORT mafVMERoot : public mafVME, public mafNodeRoot
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

  /** set the tree reference system */
  virtual void SetMatrix(const mafMatrix &mat);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** return no timestamps */
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** force the time of the internal matrix */
  virtual void SetCurrentTime(mafTimeStamp t);

  /** allow only a NULL parent */
  virtual bool CanReparentTo(mafNode *parent) {return mafNodeRoot::CanReparentTo(parent);}

  void OnEvent(mafEventBase *e);
  
protected:
  /** allowed only dynamic allocation via New() */
  mafVMERoot();           
  virtual ~mafVMERoot();  

  mafID         m_MaxItemId; ///< id counter for VME items
  mafTransform* m_Transform; ///< pose matrix for the root
private:
  mafVMERoot(const mafVMERoot&); // Not implemented
  void operator=(const mafVMERoot&); // Not implemented
};

#endif
