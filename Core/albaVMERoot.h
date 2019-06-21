/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMERoot
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMERoot_h
#define __albaVMERoot_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEStorage;

/** albaVMERoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from albaRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa albaVME
  */
class ALBA_EXPORT albaVMERoot : public albaVME
{
public:
  albaTypeMacro(albaVMERoot,albaVME);

  /** compare two VME-Root. */
  virtual bool Equals(albaVME *vme);

  enum ROOT_GUI_ID
  {
    ID_APPLICATION_STAMP = Superclass::ID_LAST,
    ID_LAST
  };

  virtual int DeepCopy(albaVME *a);

    /** 
    Return highest ItemId assigned for this tree. Return -1 if no one has
    been assigned yet.*/
  albaID GetMaxItemId() {return m_MaxItemId;}

  /** Return next available ItemId and increment the internal counter.*/
  albaID GetNextItemId() { this->Modified();return ++m_MaxItemId;}

  /**
    Set the ItemMaxId. Beware when using this function to avoid non unique
    Ids. */
  void SetMaxItemId(albaID id) { m_MaxItemId=id; Modified();}
  void ResetMaxItemId() { this->SetMaxItemId(0);}

  /** Clean the tree and all the data structures of this root*/
  void CleanTree() { \
    albaVME::CleanTree(); \
    m_Attributes.clear(); \
  };

  /** set the tree reference system */
  virtual void SetMatrix(const albaMatrix &mat);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** return no timestamps */
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);
	  
  void OnEvent(albaEventBase *alba_event);

  /** Redefined to update the gui. */
  virtual void Update();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:  

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeBox");};
	
	/**
	root node cannot be reparented. Root nodes should redefine CanReparent
	to call this function. */
	virtual bool CanReparentTo(albaVME *parent) { return parent == NULL; }
	
	/**
	Return highest NodeId assigned for this tree. Return -1 if no one has
	been assigned yet.*/
	albaID GetMaxNodeId() { return m_MaxNodeId; }

	/** Return next available NodeId and increment the internal counter.*/
	albaID GetNextNodeId() { return ++m_MaxNodeId; }

	/**
	Set the NodeMaxId. Beware when using this function to avoid non unique
	Ids. */
	void SetMaxNodeId(albaID id) { m_MaxNodeId = id; }
	void ResetMaxNodeId() { this->SetMaxNodeId(0); }


	/** Returns Storage */
	albaVMEStorage * GetStorage() const;

	/** Sets Storage */
	void SetStorage(albaVMEStorage * storage);

protected:
  /** allowed only dynamic allocation via New() */
  albaVMERoot();           
  virtual ~albaVMERoot();  

  /** Create GUI for the VME */
  virtual albaGUI  *CreateGui();

	virtual int StoreRoot(albaStorageElement *parent);
	virtual int RestoreRoot(albaStorageElement *element);
	
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  albaID         m_MaxItemId; ///< id counter for VME items
  albaTransform* m_Transform; ///< pose matrix for the root

  albaString m_ApplicationStamp;

	albaID       m_MaxNodeId; ///< Counter for node Ids

	albaVMEStorage *m_Storage;

private:
  albaVMERoot(const albaVMERoot&); // Not implemented
  void operator=(const albaVMERoot&); // Not implemented
};
#endif
