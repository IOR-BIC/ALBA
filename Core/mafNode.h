/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafNode.h,v $
  Language:  C++
  Date:      $Date: 2005-01-13 09:10:36 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafNode_h
#define __mafNode_h

#include "mafReferenceCounted.h"
#include "mafStorable.h"
#include "mafSmartPointer.h"
#include "mafEventSource.h"
#include "mafObserver.h"
#include "mafString.h"
#include "mafMTime.h"
#include <vector>
#include <map>
#include <string>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafNodeIterator;
class mafAttribute;
class mmaTagArray;

//----------------------------------------------------------------------------
// mafNode
//----------------------------------------------------------------------------
/** mafNode - the base class for the MAF tree.
  This class implements a an m-way tree. You can add/remove nodes by means of AddChild
  and RemoveChild. To access the tree you can use GetChild(). You can also obtain an iterator
  to iterate through the tree with a simple for (;;) loop. This node implementation take
  advantage of the MAF reference counting mechanism. To avoid confusion constructor
  and destructor have been protected. To allocate a node use New() and to deallocate use Delete()
  or UnRegister().
  To create a copy of the node you can use MakeCopy(). To copy node content use DeepCopy(). Any 
  node has a CanCopy() to test if copying from a different node type i possible.
  A number of functions allow to wuery the tree, like IsEmpty(), IsInTree(), GetRoot(), GetNumberOfChildren(),
  FindNodeIdx(), FindInTreeByName(), FindInTreeById(), IsAChild(), GetParent().
  A special features allow to make a node to be skipped by iterators: SetVisibleToTraverse()
  and IsVisible().
  Comparison between nodes and trees can be accomplished through Equals() and CompareTree().
  Nore reparenting can be performed through ReparentTo(). This function returns MAF_ERROR in case
  reparenting is not allowed. Each node type can decide nodes to which it can be reparented by
  redefining the CanReparentTo(( virtual function. Also each node type can decide nodes it is 
  accepting as a child by redefining the AddChild() which also can return MAF_ERROR in case of 
  denied reparenting.
  A node can detach all children RemoveAllChildren() and an entire tree can be cleaned, by detaching each sub node, 
  through CleanTree().
  Nodes have a modification time, updated each time Modified() is called, that can be retrieved with GetMTime().
  A tree can be initialized by calling Initialize() of its root, and deinitialized by means of Shutdown(). When
  attaching a node to an initialised tree the node is automatically initialized.
  @todo
  - testing
  - events invoking
  - implement the Store/Restore
  - add the attributes and the TagArray

  @sa mafRootNode
*/
class MAF_EXPORT mafNode : public mafReferenceCounted, public mafStorable, public mafObserver
{
public:
  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events */
  /** @{ */
  //MAF_ID_DEC(MAF_CH_DOWNTREE) ///< Channel used broadcast an event down in the tree
  //MAF_ID_DEC(MAF_CH_UPTREE) ///< Channel used issue an event up in the tree
  /** @} */

  mafAbstractTypeMacro(mafNode,mafReferenceCounted);

  // the base class cannot be instantiated, and thus copied
  virtual mafObject *NewInternalInstance() {return NULL;}

  // Interface for creating a copy of the node (works only for concrete subclasses)
  mafNode *NewInstance() {return SafeDownCast(NewInternalInstance());}
  
  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs);

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

  /** serialize the object on a store. @todo syntax to be changed */
  int Store();

  /** unserialized the object from a storage. @todo syntax to be changed */
  int Restore();
  
  /** return the name of this node*/
  const char *GetName();

  /** set node name */
  void SetName(const char *name);
  
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

  /** Remove a child node*/
  virtual void RemoveChild(const mafID idx);
  /** Remove a child node*/
  virtual void RemoveChild(mafNode *node) {RemoveChild(FindNodeIdx(node));};

  /**
    Find a child given its pointer and return its index. Return -1 in
    case of not found or failure.*/
  int FindNodeIdx(mafNode *a);

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
  mafNode *GetRoot();

  bool IsEmpty() {return GetNumberOfChildren()==0;}

  /** Return the number of children of this node */
  unsigned long GetNumberOfChildren();
  
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

  /**
  Set/Get the flag to make this VME visible to tree traversal. mflVMEIterator, 
  GetSpaceBounds and Get4DBounds will skip this VME if the flag is OFF.*/
  void SetVisibleToTraverse(bool flag) {m_VisibleToTraverse=flag;}
  bool GetVisibleToTraverse() {return m_VisibleToTraverse;}

  /**
  Return true if visible to tree traversal*/
  bool IsVisible() {return m_VisibleToTraverse;}

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

  /** return a reference to the event source issuing events for this object */
  mafEventSource &GetEventSource() {return m_EventSource;}

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *e);

  typedef std::vector<mafAutoPointer<mafNode> > mafChildrenVector;

  /** 
    return list of children. The returned list is a const, since it can be
    modified by means of nodes APIs */
  const mafChildrenVector *GetChildren() {return &m_Children;}

  typedef std::map<std::string,mafAutoPointer<mafAttribute> > mafAttributesMap;

  /** 
    return the list of attributes. Attributes vector can be manipulated
    directly by means of the map container APIs. */
  mafAttributesMap *GetAttributes() {return &m_Attributes;}

  /** Set a new attribute. The given attribute is */
  void SetAttribute(const char *name,mafAttribute *a);

  /** return an attribute given the name */
  mafAttribute *GetAttribute(const char *name);

  /** 
    return a pointer to the tag array attribute. If this attribute doesn't
    exist yet, create a new one. TagArray is a map storing pairs of
    Name<->components, where components are an array of mafStrings. It's a
    simple way to attach persistent attributes. For more complex attributes
    customized classes should be created, inheriting from mafAttribute
    (e.g. @sa mmaMaterial). */
  mmaTagArray  *GetTagArray();
protected:
  mafNode();
  virtual ~mafNode();

  virtual int InternalStore(mafStorageElement *parent) {return MAF_OK;}
  virtual int InternalRestore(mafStorageElement *node) {return MAF_OK;}

  //This function is overridden by subclasses to perform custom initialization */
  virtual int InternalInitialize() {return 0;};

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

  /**
    This function set the parent for this Node. It returns a value
    to allow subclasses to implement selective reparenting.*/
  virtual int SetParent(mafNode *parent);

  mafChildrenVector m_Children;  ///< list of children
  mafNode           *m_Parent;    ///< parent node

  mafAttributesMap  m_Attributes;///< vector of attached attributes

  std::string       m_Name;       ///< name of this node
  mafMTime          m_MTime;      ///< Last modification time

  mafID             m_Id;         ///< ID of this node

  bool  m_VisibleToTraverse;      ///< enable/disable traversing visit of this node
  bool  m_Initialized;            ///< set true by Initialize()

  mafEventSource m_EventSource;   ///< source of events issued by the node
};


//------------------------------------------------------------------------------
// speeds up this frequently called function
inline unsigned long mafNode::GetMTime()
//------------------------------------------------------------------------------
{
  return m_MTime.GetMTime();
}

//------------------------------------------------------------------------------
// speeds up this frequently called function
inline void mafNode::Modified()
//------------------------------------------------------------------------------
{
  m_MTime.Modified();
}

#endif
