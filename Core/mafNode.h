/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.h,v $
  Language:  C++
  Date:      $Date: 2004-12-01 18:42:16 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNode_h
#define __mafNode_h

#include "mafSmartObject.h"
#include "mafSmartPointer.h"
#include "mafVector.h"
#include "mafString.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafNodeIterator;
class mafRootNode;
//template <class T> class mafVector;

//----------------------------------------------------------------------------
// mafNode
//----------------------------------------------------------------------------
/** mafNode - the base class for the MAF tree.
  This class implements a an m-way tree. You can add/remove nodes by means of AddChild
  and RemoveChild. To access the tree you can use GetChild(). You can also obtain an iterator
  to iterate through the tree with a simple for (;;) loop. This node implementation take
  advantage of the MAF smart object reference counting mechanism. To avoid confusion constructor
  and destructor have been commented. To allocate use New and to deallocate use Delete() or UnRegister().

  @sa mafRootNode
*/
class MAF_EXPORT mafNode : public mafSmartObject
{
public:
  mafAbstractTypeMacro(mafNode,mafSmartObject);

  //void PrintSelf(std::ostream& os, vtkIndent indent);

  /**
    Initialize this node. Subclasses can redefine InternalInitialize() to customize
    the initialisation. A node is typically initialized when added to the tree, or
    just after tree loading in case of load from storage. */
  int Initialize();

  /**
    Shutdown this node. Subclasses can redefine InternalShutdown() to customize 
    actions for shutting down. A node is typically shutdown when detached from
    the tree */
  void Shutdown();

  /** Return true if this agent has been initialized */
  int IsInitialized() {return m_Initialized;}

  /**
  return the name of this VME*/
  const char *GetName() {return m_Name;};
  void SetName(const char *name) {m_Name=mafString(name);Modified();}; // force string copy
  void SetName(mafString name) {m_Name=name;Modified();};

  /**
    Copy the contents of another node into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete nodes should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(mafNode *a);

  /** TODO: to be moved to mafVME
    perform a copy by simply referencing the copied node's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some nodes
    do not support such a function! */  
  //virtual int ShallowCopy(mafNode *a);

  /**
    Test if the given node instance can be copied into this. This function should
    be reimplemented into subclasses classes*/
  virtual bool CanCopy(mafNode *vme);

  /** Create a copy of this node (do not copy the sub tree,just the node) */
  static mafNode *MakeCopy(mafNode *a);
  mafNode *MakeCopy() {return MakeCopy(this);}

  /**
    Copy the given VME tree into a new tree. In case a parent is provided, link the new
    root node to it. Return the root of the new tree.*/
  static mafNode *CopyTree(mafNode *vme, mafNode *parent=NULL);

  /** Make a copy of the whole subtree and return its pointer */
  mafNode *CopyTree() {return CopyTree(this);}

  /** Return a the pointer to a child given its index */
  mafNode *GetChild(mafID idx);
  /** Get the First child in the list.*/
  mafNode *GetFirstChild();
  /** Get the Lase child in the list.*/
  mafNode *GetLastChild();
  
  /** Add a child to this node. Return MAF_OK if success.*/
  virtual int AddChild(mafNode *node);

  // TODO: to be removed
  /** Replace a child with a new node.*/ 
  //virtual void ReplaceChild(const int idx,mafNode *node);
  /** Replace a child with a new node.*/
  //virtual void ReplaceChild(mafNode *oldnode,mafNode *node) {ReplaceChild(FindNodeIdx(oldnode),node);};

  /** Remove a child node*/
  virtual void RemoveChild(const int idx);
  /** Remove a child node*/
  virtual void RemoveChild(mafNode *node) {RemoveChild(FindNodeIdx(node));};

  /**
    Find a child given its pointer and return its index. Return -1 in
    case of not found or failure.*/
  int FindNodeIdx(const mafNode *a);

  /**
  Find a node in all the subtrees matching the given TagName/TagValue pair.*/
  //mafNode *FindInTreeByTag(const char *name,const char *value="",int type=vtkTagItem::MFL_STRING_TAG); 
  
  /**
    Find a node in all the subtrees matching the given VME Name.*/
  mafNode *FindInTreeByName(const char *name);

  /**
    Find a node in all the subtrees matching the given VME Name.*/
  mafNode *FindInTreeById(const mafID id);

  /**
    Reparent this Node into a different place of the same tree. 
    BEWARE: Reparent into a different tree is allowed, but could
    generate bad problems. Inherited classes should reimplement this
    function to avoid these problems when reparenting to different trees.
    To move a node into a different tree you better use DeepCopy to copy 
    it into a Node of that tree.*/
  virtual int ReparentTo(mafNode *parent);

  /** Return true if the given one is a child of this node.*/
  bool IsAChild(mafNode *a);

  /**
    Find a node in all the subtrees, searching recursivelly into subnodes.
    Return true if found. */
  bool IsInTree(mafNode *a);

  /** Return the root of the tree this node owns to. */
  mafRootNode *GetRoot();

  bool IsEmpty() {return GetNumberOfChildren()==0;}

  /** Return the number of children of this node */
  int GetNumberOfChildren();
  
  /**
  Return the pointer to the parent node (if present)*/
  mafNode *GetParent() {return m_Parent;};

  /**
    Remove recursively all nodes from this tree, forcing all subnodes
    to detach their children. You better use RemoveAllChildren instead!!! */
  void CleanTree();

  /**
    Remove all children nodes. If the children are not referenced by other objects
    they will be automatically deallocated by UnRegister() mechanism, and
    the removal will recurse.*/
  void RemoveAllChildren();
  
  /**
    Return a new Tree iterator already set to traverse 
    the sub tree starting a this node*/
  mafNodeIterator *NewIterator();

  /** Compare two nodes. sublcasses should redefine this function. */
  virtual bool Equals(mafNode *vme);

  /**
    Compare the two subtrees starting from this node with the given one. Two trees
    are considered equivalent if they have equivalent nodes (@sa Equals() ), and are 
    disposed in the same hierarchy.
    Order of children node is significative for comparison! */
  bool CompareTree(mafNode *vme);

  /**
  return true if VME can be reparented under the specified node*/
  virtual bool CanReparentTo(mafNode *parent) {return parent==NULL||!IsInTree(parent);}

  /** redefined to cope with tree registering */
  virtual void UnRegister(void *o);

   /** increment update modification time */
  inline void Modified();

  /** return modification time */
  inline unsigned long GetMTime();

  typedef mafVector<mafAutoPointer<mafNode>> mafChildrenVector;

protected:
  mafNode();
  virtual ~mafNode();

  //This function is overridden by subclasses to perform custom initialization */
  virtual int InternalInitialize() {return 0;};

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};


  /**
    This function set the parent for this Node. It returns a value
    to allow subclasses to implement selective reparenting.*/
  virtual int SetParent(mafNode *parent);

  mafChildrenVector *m_Children;
  
  mafNode   *m_Parent;      ///< parent node

  mafString m_Name;         ///< name of this node
  mafMTime  m_MTime;        ///< Last modification time

  bool  m_VisibleToTraverse;  ///< enable/disable traversing visit of this node
  bool  m_Initialized;        ///< set true by Initialize()
  int   m_Crypting;           ///< enable crypting during storing/restoring for this node
};


//------------------------------------------------------------------------------
inline unsigned long mafNode::GetMTime()
//------------------------------------------------------------------------------
{
  return m_MTime.GetMTime();
}

//------------------------------------------------------------------------------
inline void mafNode::Modified()
//------------------------------------------------------------------------------
{
  m_MTime.Modified();
}

#endif
