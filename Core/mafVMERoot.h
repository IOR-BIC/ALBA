/*=========================================================================

 Program: MAF2
 Module: mafVMERoot
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMERoot_h
#define __mafVMERoot_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafRoot.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafVMERoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from mafRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa mafRoot mafVME

  @todo
  - 
  */
class MAF_EXPORT mafVMERoot : public mafVME, public mafRoot
{
public:
  mafTypeMacro(mafVMERoot,mafVME);

  /** compare two VME-Root. */
  virtual bool Equals(mafVME *vme);

  enum ROOT_GUI_ID
  {
    ID_APPLICATION_STAMP = Superclass::ID_LAST,
    ID_LAST
  };

  virtual int DeepCopy(mafVME *a);

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
    mafVME::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** set the tree reference system */
  virtual void SetMatrix(const mafMatrix &mat);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** return no timestamps */
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** allow only a NULL parent */
  virtual bool CanReparentTo(mafVME *parent) {return mafRoot::CanReparentTo(parent);}

  void OnEvent(mafEventBase *maf_event);

  /** Redefined to update the gui. */
  virtual void Update();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:  

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeBox");};


protected:
  /** allowed only dynamic allocation via New() */
  mafVMERoot();           
  virtual ~mafVMERoot();  

  /** Create GUI for the VME */
  virtual mafGUI  *CreateGui();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mafID         m_MaxItemId; ///< id counter for VME items
  mafTransform* m_Transform; ///< pose matrix for the root

  mafString m_ApplicationStamp;

private:
  mafVMERoot(const mafVMERoot&); // Not implemented
  void operator=(const mafVMERoot&); // Not implemented
};
#endif
