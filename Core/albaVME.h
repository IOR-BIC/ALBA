/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVME
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVME_h
#define __albaVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaReferenceCounted.h"
#include "albaStorable.h"
#include "albaSmartPointer.h"
#include "albaObserver.h"
#include "albaEventBroadcaster.h"
#include "albaTagItem.h"
#include "albaString.h"
#include "albaTimeStamped.h"
#include "albaAttribute.h"
#include "albaDecl.h"
#include <vector>
#include <map>
#include <set>
#include <string>

#include "albaVMEOutput.h"
#include "albaDataPipe.h"
#include "albaMatrixPipe.h"
#include "albaAbsMatrixPipe.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaMatrix;
class albaTransform;
class albaInteractor;
class albaVMEIterator;
class albaTagArray;
class albaGUI;


typedef enum LinkType
{
	NORMAL_LINK,
	MANDATORY_LINK,
} LinkType;

/** data structure used to store a link VME and its Id */
struct ALBA_EXPORT albaOldSubIdLink :public albaUtility
{
public:
	albaOldSubIdLink(albaString name,albaID id = -1, albaID sub_id = -1):m_Name(name), m_NodeId(id), m_NodeSubId(sub_id) {}
	albaString m_Name;
	albaID   m_NodeId;
	albaID   m_NodeSubId;
};

/** data structure used to store a link VME and its Id */
struct ALBA_EXPORT albaVMELink :public albaUtility
{
public:
	albaVMELink(albaID id = -1, albaVME *node = NULL, LinkType type = NORMAL_LINK) :m_NodeId(id), m_Node(node), m_Type(type) {}
	albaID   m_NodeId;
	albaVME *m_Node;
	LinkType m_Type;
};

/** data structure used to store a link VME and its Id */
struct ALBA_EXPORT albaVMEBackLink :public albaUtility
{
public:
	albaVMEBackLink(albaString name, albaVME *node = NULL):m_Name(name), m_Node(node) {}
	albaString m_Name;
	albaVME *m_Node;
};



#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT, albaAutoPointer<albaVME>);
#endif


#ifdef ALBA_EXPORTS
template class ALBA_EXPORT albaAutoPointer<albaDataPipe>;
template class ALBA_EXPORT albaAutoPointer<albaMatrixPipe>;
template class ALBA_EXPORT albaAutoPointer<albaAbsMatrixPipe>;
#endif

/** albaVME -

	albaVME is a node for sciViz purposes. It features a procedural core generating an output
	data structure storing a pose matrix and a VTK dataset. The albaVME generates also an absolute
	pose matrix, a matrix representing the pose of the VME in the world reference system.
	albaVME has an output with a pose matrix and a VTK dataset.
	
	This class also implements a m-way tree. A VME can be seen as a tree node.
	You can add/remove nodes by means of AddChild	and RemoveChild. To access the tree you can use GetChild(). 
	You can also obtain an iterator	to iterate through the tree with a simple for (;;) loop. 
	This node implementation take	advantage of the ALBA reference counting mechanism. 
	To avoid confusion constructor and destructor have been protected. 
	To allocate a node use New() and to deallocate use Delete()	or UnRegister().
	To create a copy of the node you can use MakeCopy(). To copy node content use DeepCopy(). Any
	node has a CanCopy() to test if copying from a different node type i possible.
	A number of functions allow to query the tree, like IsEmpty(), IsInTree(), GetRoot(), GetNumberOfChildren(),
	FindNodeIdx(), FindInTreeByName(), FindInTreeById(), IsAChild(), GetParent().
	A special features allow to make a node to be skipped by iterators: SetVisibleToTraverse()
	and IsVisible().
	Comparison between nodes and trees can be accomplished through Equals() and CompareTree().
	Nore reparenting can be performed through ReparentTo(). This function returns ALBA_ERROR in case
	reparenting is not allowed. Each node type can decide nodes to which it can be reparented by
	redefining the CanReparentTo() virtual function. Also each node type can decide nodes it is
	accepting as a child by redefining the AddChild() which also can return ALBA_ERROR in case of
	denied reparenting.
	A node can detach all children RemoveAllChildren() and an entire tree can be cleaned, by detaching each sub node,
	through CleanTree().
	Nodes inherits from albaTimeStamped a modification time updated each time Modified() is called, that can be retrieved with GetMTime().
	A tree can be initialized by calling Initialize() of its root, and deinitialized by means of Shutdown(). When
	attaching a node to an initialised tree the node is automatically initialized.
	@todo
	- events invoking
	- add storing of Id and Links
	- test Links and Id
	- test FindInTree functions
	- test node events (attach/detach from tree, destroy)
	- test DeepCopy()
		
  @sa albaVMEIterator

  @todo
  - implement the GetMTime() function: it's used by pipeline to trigger the update
  - Change the SetParent to update the AbsMatrixPipe connections
  - fix the VME_TIME_SET issuing and propagation
  - implement Update() function
  */
class ALBA_EXPORT albaVME : public albaReferenceCounted, public albaEventBroadcaster, public albaStorable, public albaObserver, public albaTimeStamped, public albaServiceClient
{
public:
  
	albaAbstractTypeMacro(albaVME, albaReferenceCounted);

	/** IDs for the GUI */
	enum VME_WIDGET_ID
	{
		ID_NAME = MINID,
		ID_PRINT_INFO,
		ID_HELP,
		ID_LAST
	};

	/** defined to allow MakeCopy implementation. For the base class return a NULL pointer. */
	virtual albaObject *NewObjectInstance() const { return NULL; }

	/** Interface to allow creation of a copy of the node (works only for concrete subclasses) */
	albaVME *NewInstance() { return SafeDownCast(NewObjectInstance()); }

	/**
	Initialize this node. Subclasses can redefine InternalInitialize() to customize
	the initialization. A node is typically initialized when added to the tree, or
	just after tree loading in case of load from storage. */
	int Initialize();

	/**
	Shutdown this node. Subclasses can redefine InternalShutdown() to customize
	actions for shutting down. A node is typically shutdown when detached from
	the tree */
	void Shutdown();

	/** Return true if this agent has been initialized */
	int IsInitialized() { return m_Initialized; }

	/** serialize the object on a store. @todo syntax to be changed */
	int Store();

	/** unserialized the object from a storage. @todo syntax to be changed */
	int Restore();

	/** return the name of this node*/
	const char *GetName() { return m_Name; }

	/** set node name */
	virtual void SetName(const char *name);

	
	/** TODO: to be moved to albaVME
	perform a copy by simply referencing the copied node's data array.
	Beware: This can allow to save memory when doing special tasks, but
	can be very dangerous making one of the VME inconsistent. Some nodes
	do not support such a function! */
	//virtual int ShallowCopy(albaVME *a);

	/** Test if the given node instance can be copied into this. This function should
	be reimplemented into subclasses classes*/
	virtual bool CanCopy(albaVME *vme);

	/** Create a copy of this node (do not copy the sub tree,just the node) */
	static albaVME *MakeCopy(albaVME *a);
	albaVME *MakeCopy() { return MakeCopy(this); }

	/** Copy the given VME tree into a new tree. In case a parent is provided, link the new
	root node to it. Return the root of the new tree.*/
	static albaVME *CopyTree(albaVME *vme, albaVME *parent = NULL);

	/** Make a copy of the whole subtree and return its pointer */
	albaVME *CopyTree() { return CopyTree(this); }

	/** Return a the pointer to a child given its index.
	If only visible is true return the idx-th visible to traverse node */
	albaVME *GetChild(albaID idx, bool onlyVisible = false);
	/** Get the First child in the list.
	If only visible is true return the first visible to traverse node */
	albaVME *GetFirstChild(bool onlyVisible = false);
	/** Get the Lase child in the list.
	If only visible is true return the last visible to traverse node */
	albaVME *GetLastChild(bool onlyVisible = false);

	/** Get A child by path.
	The pats are generated from a series of keyword divided by '\'

	The possible keywords are:
	next: return the next node at same level
	prev: return the previous node at same level
	firstPair: return the first node at same level
	lastPair: return the last node at same level
	firstChild: return the first node between children
	lastChild: return the last node between children
	pair[<number>]: return the <number>-th node between pairs
	pair{<node name>}: return the node named <node name> between pairs
	child[<number>]: return the <number>-th node between children
	child{<node name>}: return the node named <node name> between children
	..:

	An example path is:
	"../../child{sideB}/child[2]"

	By default this function search only on visible to traverse nodes
	*/
	albaVME *GetByPath(const char *path, bool onlyVisible = true);

	/** Add a child to this node. Return ALBA_OK if success.*/
	virtual int AddChild(albaVME *node);

	/** Remove a child node*/
	virtual void RemoveChild(const albaID idx, bool onlyVisible = false);
	/** Remove a child node*/
	virtual void RemoveChild(albaVME *node);

	/** Find a child given its pointer and return its index. Return -1 in case of not found or failure.
	If only visible is true return the idx of visible to traverse nodes subset */
	int FindNodeIdx(albaVME *a, bool onlyVisible = false);

	/** Find a child index given its name. Search is performed only on first level children not
	in the sub-tree. Return -1 in case of not found or failure.
	If only visible is true return the idx of visible to traverse nodes subset */
	int FindNodeIdx(const char *name, bool onlyVisible = false);

	/** Find a node in all the subtrees matching the given TagName/TagValue pair.*/
	albaVME *FindInTreeByTag(const char *name, const char *value = "", int type = ALBA_STRING_TAG);

	/** Find a node in all the subtrees matching the given VME Name.*/
	albaVME *FindInTreeByName(const char *name, bool match_case = true, bool whole_word = true);

	/** Find a node in all the subtrees matching the given VME Name.*/
	albaVME *FindInTreeById(const albaID id);

	/**
	Reparent this Node into a different place of the same tree.
	BEWARE: Reparent into a different tree is allowed, but could
	generate bad problems. Inherited classes should reimplement this
	function to avoid these problems when reparenting to different trees.
	To move a node into a different tree you better use DeepCopy to copy
	it into a Node of that tree.*/
	virtual int ReparentTo(albaVME *parent);

	/** Import all children of another tree into this tree */
	void Import(albaVME *tree);

	/** Return true if the given one is a child of this node.*/
	bool IsAChild(albaVME *a);

	/**
	Find a node in all the subtrees, searching recursively into sub nodes.
	Return true if found. */
	bool IsInTree(albaVME *a);

	/** Return the root of the tree this node owns to. */
	albaVME *GetRoot();

	bool IsEmpty() const { return GetNumberOfChildren() == 0; }

	/** Valid VMEs have m_ID >= 0. The root has m_Id = 0, other VMEs have m_Id > 0.*/
	bool IsOnVMETree() { return m_Parent !=NULL; };

	/** Return the number of children of this node
	If only visible is true return the number visible to traverse nodes */
	unsigned long GetNumberOfChildren() const;

	/** Return the number of children of this node
	If only visible is true return the number visible to traverse nodes */
	unsigned long GetNumberOfChildren(bool onlyVisible);

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
	the sub tree starting a this node. Remember to delete the iterator after use it.*/
	albaVMEIterator *NewIterator();

	/**
	Set/Get the flag to make this VME visible to tree traversal. albaVMEIterator,
	GetSpaceBounds and Get4DBounds will skip this VME if the flag is OFF.*/
	void SetVisibleToTraverse(bool flag) { m_VisibleToTraverse = flag; }
	bool GetVisibleToTraverse() { return m_VisibleToTraverse; }

	/**  Return true if visible to tree traversal*/
	bool IsVisible() { return m_VisibleToTraverse; }

	/**
	Compare the two subtrees starting from this node with the given one. Two trees
	are considered equivalent if they have equivalent nodes (@sa Equals() ), and are
	disposed in the same hierarchy.
	Order of children node is significative for comparison! */
	bool CompareTree(albaVME *vme);

	/** redefined to cope with tree registering */
	virtual void UnRegister(void *o);

	/** Precess events coming from other objects */
	virtual void OnEvent(albaEventBase *e);

	typedef std::vector<albaAutoPointer<albaVME> > albaChildrenVector;

	/**
	return list of children. The returned list is a const, since it can be
	modified by means of nodes APIs */
	const albaChildrenVector *GetChildren() { return &m_Children; }

	typedef std::map<albaString, albaAutoPointer<albaAttribute> > albaAttributesMap;

	/**
	return the list of attributes. Attributes vector can be manipulated
	directly by means of the map container APIs. */
	albaAttributesMap *GetAttributes() { return &m_Attributes; }

	/** Set a new attribute. The given attribute is */
	void SetAttribute(const char *name, albaAttribute *a);

	/** return an attribute given the name */
	albaAttribute *GetAttribute(const char *name);

	/** remove an attibute */
	void RemoveAttribute(const char *name);

	/** remove all the attributes of this node */
	void RemoveAllAttributes();

	/**
	return a pointer to the tag array attribute. If this attribute doesn't
	exist yet, create a new one. TagArray is a map storing pairs of
	Name<->components, where components are an array of albaStrings. It's a
	simple way to attach persistent attributes. For more complex attributes
	customized classes should be created, inheriting from albaAttribute
	(e.g. @sa mmaMaterial). */
	albaTagArray  *GetTagArray();
			
	typedef std::map<albaString, albaVMELink> albaLinksMap;
	typedef std::set<albaVME *> albaVMESet;
	typedef std::vector<albaVMEBackLink> albaVMEBackLinks;

	/**
	return the value of a link to another node in the tree. If no link with
	such a name exists return NULL. this function returns both links and mandatory links */
	albaVME *GetLink(const char *name);

	/** set a link to another node in the tree, if node is NULL current link is removed */
	void SetLink(const char *name, albaVME *node);

	/** set a mandatory link to another node in the tree, if node is NULL current link is removed */
	void SetMandatoryLink(const char *name, albaVME *node);

	/** set a link or a mandatory link to another node in the tree, if node is NULL current link is removed */
	void SetLink(const char *name, albaVME *node, LinkType type);


	/** set a mandatory link to another node in the tree */
	void AddBackLink(const char *name, albaVME *node);

	/** set a mandatory link to another node in the tree */
	void RemoveBackLink(const char *name, albaVME *node);

	/** This method manage olds links with subid and is here only for retro compatibility issues
			please do not use this */
	void SetOldSubIdLink(albaString link_name, albaID link_node_id, albaID link_node_subid);

	/** remove a link */
	void RemoveLink(const char *name);

	/** return the number of links stored in this Node */
	albaID GetNumberOfLinks() { return m_Links.size(); }

	/** return the number of back links of this Node */
	albaID GetNumberOfBackLinks() { return m_BackLinks.size(); }

	/** return a copy of the vector of back links of this Node */
	albaVMEBackLinks GetBackLinks() { return m_BackLinks; }

	/** remove all links */
	void RemoveAllLinks();

	/** remove all back links */
	void RemoveAllBackLinks() { m_BackLinks.clear(); }

	/** Return a set of all VME depending on this vme or is subtree*/
	albaVMESet GetDependenciesVMEs();
	
	/** Removes all the VME that are depending on the current VME */
	void RemoveDependenciesVMEs();

	/** Return true if vme will be removed on Remove Dependencies
			A VME will be removed if is contained on dependencies list or 
			if is contained in a sub-tree of the dependencies */
	bool WillBeRemovedWithDependencies(albaVME *vme);

	/** return links array: links from this node to other arrays */
	albaLinksMap *GetLinks() { return &m_Links; }

	/** used to send an event up in the tree */
	void ForwardUpEvent(albaEventBase *alba_event);
	void ForwardUpEvent(albaEventBase &alba_event);

	/** used to send an event down in the tree */
	void ForwardDownEvent(albaEventBase *alba_event);
	void ForwardDownEvent(albaEventBase &alba_event);


	/** create and return the GUI for changing the node parameters */
	albaGUI *GetGui();

	/** destroy the Gui */
	void DeleteGui();

	/** return the Id of this node in the tree */
	albaID GetId() const;

	/** return an xpm-icon that can be used to represent this node */
	static char ** GetIcon();

	/** Check if m_Id and regenerate it if is invalid (-1) */
	void UpdateId();

	/**
	Return the modification time.*/
	virtual vtkMTimeType GetMTime();

	/**
	Turn on the flag to calculate the timestamp considering also the linked nodes*/
	void DependsOnLinkedNodeOn() { m_DependsOnLinkedNode = true; };

	/**
	Turn off the flag to calculate the timestamp considering also the linked nodes*/
	void DependsOnLinkedNodeOff() { m_DependsOnLinkedNode = false; };

  enum VME_VISUAL_MODE
  {
    DEFAULT_VISUAL_MODE = 0,
    NO_DATA_VISUAL_MODE
  };

  //typedef std::vector<albaTimeStamp> std::vector<albaTimeStamp>;

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** 
    return the parent VME Node. Notice that a VME can only reparented 
    under another VME, not to other kind of nodes! */
		albaVME *GetParent() const { return m_Parent; };
  
  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(albaVME *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(albaVME *a);

  /** Compare two VMEs. The specific comparison must be implemented by subclasses. */
  virtual bool Equals(albaVME *vme);

  /**
   Set the time for this VME (not for the whole tree). Normally time 
   of the tree is set by sending an event with id VME_TIME_SET */
  virtual void SetTimeStamp(albaTimeStamp t);
  virtual albaTimeStamp GetTimeStamp();  

  /**
    Set/Get CurrentTime for this VME and all subtree. Normally time 
    of the tree is set by sending an event with id VME_TIME_SET*/
  void SetTreeTime(albaTimeStamp t);

  /**
    Set the Pose matrix of the VME. This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  virtual void SetMatrix(const albaMatrix &mat)=0;

  /** set the pose matrix for a specific time stamp (ignore the matrix internal timestamp!) */
  void SetPose(const albaMatrix &mat,albaTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double x,double y,double z,double rx,double ry,double rz, albaTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double xyz[3],double rxyz[3], albaTimeStamp t);

  /** apply a matrix to the VME pose matrix */
  void ApplyMatrix(const albaMatrix &matrix,int premultiply,albaTimeStamp t=-1);
  
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double x,double y,double z,double rx,double ry,double rz, albaTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double xyz[3],double rxyz[3], albaTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(const albaMatrix &matrix,albaTimeStamp t);
  /** Set the global pose of this VME for the current time. This function usually modifies the MatrixVector. */
  virtual void SetAbsMatrix(const albaMatrix &matrix);

  /** apply a matrix to the VME abs pose matrix */
  void ApplyAbsMatrix(const albaMatrix &matrix,int premultiply,albaTimeStamp t=-1);
 
  /** return true if the VME can be reparented under the specified node */
  virtual bool CanReparentTo(albaVME *parent);
  
  // to be revised
	/** Set auxiliary reference system and its name*/
	//int SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  // to be revises
	/** Get auxiliary reference system from its name*/
  //int GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  /** return the matrix pipe object, i.e. the source of the output matrix. */
  albaMatrixPipe *GetMatrixPipe() {return m_MatrixPipe;}
  
  /** return the matrix pipe used for computing the AbsMatrix.*/
  albaAbsMatrixPipe *GetAbsMatrixPipe() {return m_AbsMatrixPipe;}
  
  /** return the data pipe object, i.e. the source of the output dataset. */
  albaDataPipe *GetDataPipe() {return m_DataPipe;}

  /**
    this function makes the current data pointer to point the right output
    data, usually the DataPipe output data but subclasses can redefine this
    function. By default if no data pipe is defined, current data is set to
    NULL.*/
  virtual void Update();
  
  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)=0;

  /** Set the time bounds for the time varying VME (base VME is not time varying).*/
  virtual void GetLocalTimeBounds(albaTimeStamp tbounds[2]) {tbounds[0] = m_CurrentTime; tbounds[1] = m_CurrentTime;};
  
	/**
    Return the list of timestamps considering all parents timestamps. Timestamps list is
    obtained merging timestamps for matrices and VME items*/
	virtual void GetAbsTimeStamps(std::vector<albaTimeStamp> &kframes);
  
	/**
    Return the list of timestamps for the full sub tree. Timestamps list is
    obtained merging timestamps for matrices and VME items*/
  virtual void GetTimeStamps(std::vector<albaTimeStamp> &kframes);  
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfTimeStamps();
  
  /** Return the number of time stamps local to the vme*/
  int GetNumberOfLocalTimeStamps();
  
  /** Return true if the number of local time stamps is > 1*/
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();
  
  /** Set the crypting status for the vme. */
  void SetCrypting(int crypting);
  
  /** Get the crypting status of the vme. */
  int GetCrypting();

  /** return a pointer to the output data structure */
  virtual albaVMEOutput *GetOutput() {return m_Output;}

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual albaString GetVisualPipe() { return albaString(""); };

	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual albaString GetVisualPipeSlice() { return albaString(""); };

  /** Set the mode with which render the VME: DEFAULT_VISUAL_MODE or NO_DATA_VISUAL_MODE.
  Setting the visual mode to default will produce also the reset of the m_EditingVisualPipe
  to the empty string. This because the editing visual pipe is set dynamically by the class that
  use this feature.*/
  void SetVisualMode(int mode);

  /** Get the mode with which render the VME: DEFAULT_VISUAL_MODE or NO_DATA_VISUAL_MODE*/
  int GetVisualMode() {return m_VisualMode;};

  /** 
    return the interactor assigned as a behavior to this VME. This is the 
    interactor to which events coming from input devices are sent when 
    user pick this VME. Responsible of this routing is the PER (@sa albaInteractorPER) */
  albaInteractor *GetBehavior() {return m_Behavior;}

  /** set the interactor representing the behavior of this VME. @sa GetBehavior() */
  void SetBehavior(albaInteractor *bh) {m_Behavior=bh;};
  
  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of GUI components that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;};

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;};

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;};


	/** Returns if VME Was expanded when stored last time. return true for non stored VME*/
	bool WasVMEExpandedWhenStored() const { return m_VMEExpandedWhenStored; }

protected:
  albaVME(); // to be allocated with New()
	virtual ~albaVME(); // to be deleted with Delete()


	/** internally used to set the node ID */
	void SetId(albaID id);

	virtual int InternalStore(albaStorageElement *parent);
	virtual int InternalRestore(albaStorageElement *node);
		
	
	/** to be redefined by subclasses to define the shutdown actions */
	virtual void InternalShutdown() {};

	/**
	This function set the parent for this Node. It returns a value
	to allow subclasses to implement selective reparenting.*/
	virtual int SetParent(albaVME *parent);

	void OnNodeDetachedFromTree(albaEventBase *e);
	void OnNodeAttachedToTree(albaEventBase *e);
	void OnNodeDestroyed(albaEventBase *e);

	/** Get the dependecies for the specified VME */
	void GetDependenciesVMEs(albaVMESet &dependencies,albaVME *vme);

  /** used to initialize the AbsMatrixPipe */
  virtual int InternalInitialize();

  /** called to prepare the update of output */
  virtual void InternalPreUpdate() {}

  /** update the output data structure */
  virtual void InternalUpdate() {}

	/** Set the time for this VME (not for the whole tree) without notifying listeners. 
	This method is called by SetTimeStamp method (that also notifies listeners). */
	virtual void InternalSetTimeStamp(albaTimeStamp t);

	// Make a Copy of Subtree and Update new Links (Recursive Function)
	static albaVME* InternalCopyTree(albaVME * vme, albaVME * parent);

  /** 
    Set the output and connect it to the VME. This is automatically called
    by GetOutput() of specific VME's */
  void SetOutput(albaVMEOutput *output);
  
  /** Set/Get the data pipe object, i.e. the source of the output dataset. */
  virtual int SetDataPipe(albaDataPipe *dpipe);

  /** Set the matrix pipe object, i.e. the source of the output matrix. */
  int SetMatrixPipe(albaMatrixPipe *pipe);

	/** Returns typename (human readable) from vme type (class name)*/
	albaString GetTypeNameFromType(albaString typeName);

  /** Set the abs matrix pipe object, i.e. the source of the output abs matrix. */
  void SetAbsMatrixPipe(albaAbsMatrixPipe *pipe);

  /** Create GUI for the VME 
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI  *CreateGui();

	/** Precess Tree management events */
	void NodeOnEvent(albaEventBase *e);


	albaGUI            *m_Gui;         ///< pointer to the node GUI

	albaChildrenVector m_Children;     ///< list of children
	albaVME           *m_Parent;      ///< parent node

	albaAttributesMap  m_Attributes;   ///< attributes attached to this node

	albaLinksMap       m_Links;					///< links to other nodes in the tree
	albaVMEBackLinks m_BackLinks;      ///< links to other nodes in the tree
	std::vector<albaOldSubIdLink> m_OldSubIdLinks;

	albaString         m_Name;         ///< name of this node
	albaString         m_GuiName;      ///< name showed on gui
	albaID             m_Id;           ///< ID of this node

	bool m_VisibleToTraverse;         ///< enable/disable traversing visit of this node
	bool m_Initialized;               ///< set true by Initialize()
	bool m_DependsOnLinkedNode;       ///< enable/disable calculation of MTime considering links
	bool m_TestMode; ///< Flag used with cppunitTest: put this flag at true when executing tests to avoid busy-info or splash screen to be created, default is false.

  albaAutoPointer<albaDataPipe>       m_DataPipe;
  albaAutoPointer<albaMatrixPipe>     m_MatrixPipe;
  albaAutoPointer<albaAbsMatrixPipe>  m_AbsMatrixPipe;

  albaVMEOutput*   m_Output;       ///< the data structure storing the output of this VME
  albaTimeStamp    m_CurrentTime;  ///< the time parameter for generation of the output
  int             m_Crypting;     ///< enable flag for this VME
  
  albaInteractor*  m_Behavior;     ///< the interactor representing the behavior of this VME

  int             m_VisualMode; ///< Store the visual mode to allow the visual pipe to choose the right visual pipe to render the VME

	albaID		m_VMEExpandedWhenStored;
  
	typedef struct
	{
		albaVME *oldPointer;
		albaVME *newPointer;
	} VmePointerMap;

	typedef struct
	{
		albaString name;
		albaVME *vme;
	} VmeLinks;

private:
  albaVME(const albaVME&); // Not implemented
  void operator=(const albaVME&); // Not implemented

};

#endif
