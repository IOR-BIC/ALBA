/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.h,v $
  Language:  C++
  Date:      $Date: 2004-11-29 21:17:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mflVME_h
#define __mflVME_h

#include "vtkTree.h"
#include "vtkTagArray.h"
#include "mflCoreWin32Header.h"
#include "mflString.h"
#include "mflMatrix.h"
#include "mflVMEItem.h"

#include "mflEvent.h"


//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
namespace std
{
  template <class AType> class allocator;
  template <class VType, class AType = allocator<VType> > class vector;
}

typedef std::vector<mflTimeStamp> TimeVector;
template <class ClassType> class mflSmartPointer;

class mflVMEIterator;
class mflVMEItemArray;
class mflMatrixVector;
class mflDataPipe;
class mflMatrixPipe;
class mflBounds;
class mflTransform;
class vtkLinearTransform;
class mflMatrixPipeDirectCinematic;
class vtkDoubleArray;

/** mflVME - a tree node implementation with a matrix and a tag array for each node.
  mflVMERoot is a specialized tree node which can store, a metadata array
  (vtkTagArray), a timevariant matrix (mflMatrixVector) and an array of dataset
  wrapped inside specialized objects (mflVMEItem)

  @sa vtkTree mflVMEItem mflMatrixVector vtkTagArray

  @todo
  - implement the GetMTime() function: it's used by pipeline to trigger the update
  - rewrite all the AbsMatrix stuff, implementing a mflMatrixPipeDirectCinematic
  - Change the SetParent to update the AbsMatrixPipe connections
  */
class MFL_CORE_EXPORT mflVME : public vtkTree
{
public:

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  MFL_EVT_DEC(TimeEvent); // Rised by time changes
  MFL_EVT_DEC(MatrixPipeChangedEvent); // Rised when the MatrixPipe is changed
  MFL_EVT_DEC(MatrixUpdateEvent); // Rised by pose Matrix updates
  MFL_EVT_DEC(AbsMatrixUpdateEvent); // Rised by AbsMatrix updates
  MFL_EVT_DEC(OutputDataUpdateEvent); // Rised by Current Udaptes (this re-routes the same DataPipe event)
  MFL_EVT_DEC(DataPipeChangedEvent); // Rised when the DataPipe is changed
  MFL_EVT_DEC(OutputDataChangedEvent); // Rised when the OutputData object is changed
  MFL_EVT_DEC(DetachFromTreeEvent); // Event rised by detachment from tree
  MFL_EVT_DEC(AttachToTreeEvent); // Event rised by atachment to tree
  MFL_EVT_DEC(DestroyEvent); // Event rised by VME destroying

  vtkTypeMacro(mflVME,vtkTree);

  void PrintSelf(ostream& os, vtkIndent indent);

  /**
    Initialize this agnet. Subclasses can redefine InternalInitialize() to customize
    the initialisation. */
  int Initialize();

  /**
    Shutdown the agent, Subclasses can redefine InternalShutdown() to customize 
    actions for shutting down */
  void Shutdown();

  /** Return true if this agent has been initialized */
  int IsInitialized() {return Initialized;}

  /**
  Return the array of Tags for this object*/
  virtual vtkTagArray *GetTagArray() {return this->TagArray;}
  
  /**
  Find a node in all the subtrees matching the given TagName/TagValue pair.*/
  mflVME *FindInTree(const char *name,const char *value="",int type=vtkTagItem::MFL_STRING_TAG); 
  
  /**
  Find a node in all the subtrees matching the given VME Name.*/
  mflVME *FindInTreeByName(const char *name); 

  /**
  Return the pointer to the parent node (if present)*/
  mflVME *GetParent() {return (mflVME *)this->Superclass::GetParent();};
  
  /**
  Get the specified VMEItem*/
  mflVMEItem *GetItem(int id);

  /** */
  int FindItem(mflTimeStamp t,mflVMEItem *&item);

  /** */
  int GetItem(int id,mflVMEItem *&item);

  /**
  Add a VMEItem to this VME*/
  virtual int AddItem(mflVMEItem *item);

  /**
  Remove an item from this VME*/
  int RemoveItem(int id);
  int RemoveItem(mflVMEItem *item);

  /**
  Equivalent to RemoveItem(olditem)+AddItem(newitem), but
  preserve the index inside the Item list*/
  int ReplaceItem(int idx) {vtkGenericWarningMacro("ReplaceItem not yet implemented"); return VTK_ERROR;}
  int ReplaceItem(mflVMEItem *item) {vtkGenericWarningMacro("ReplaceItem not yet implemented"); return VTK_ERROR;}

  /**
  Remove all items form this VME*/
  void RemoveAllItems();

  /**
  Return the number of items (time samples) stored in this VME*/
  int GetNumberOfItems();

  /**
  Return the pointer to the item with timestamp==t. Return NULL 
  if not found.*/
  mflVMEItem *GetItemByTimeStamp(mflTimeStamp t);

  /**
  Copy the contents of another VME into this one. Notice that subtrees
  are not copied, i.e. copy is not recursive!
  Concrete class should reimplement this function to verify admitted
  conversion. */
  virtual int DeepCopy(mflVME *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function!
  */  
  virtual int ShallowCopy(mflVME *a);

  /**
  Test if the given VME instance can be copied into this. This function can
  be reimplemented into inherited classes*/
  virtual bool CanCopy(mflVME *vme);

  /**
  Create a copy of this VME*/
  static mflVME *MakeCopy(mflVME *a);
  mflVME *MakeCopy() {return this->MakeCopy(this);}

  /** */  
  int FindChildIdx(const char *name);

  /**
  Redefine superclass functions to accept and return only VME nodes*/
  mflVME *GetChild(vtkIdType idx) {return (mflVME *)this->Superclass::GetChild(idx);}
  mflVME *GetChild(const char *name) {return GetChild(this->FindChildIdx(name));}
  mflVME *GetFirstChild() {return (mflVME *)this->Superclass::GetFirstChild();}
  mflVME *GetLastChild() {return (mflVME *)this->Superclass::GetLastChild();}
  virtual int AddChild(mflVME *node) {return this->Superclass::AddChild(node);}
  virtual int AppendChild(mflVME *node) {return this->AddChild(node);}

  /**
  Set/Get CurrentTime for this VME. This time is propagated to
  the matrix vector and is used to interpolate items' data.*/
  mflTimeStamp GetTime() {return this->CurrentTime;}
  mflTimeStamp GetCurrentTime() {return this->CurrentTime;}

  /**
   Set the time for this VME.*/
  virtual void SetCurrentTime(mflTimeStamp t);
  

  /**
  Set/Get CurrentTime for this VME and all subtree.*/
  void SetTreeTime(mflTimeStamp t);

  /**
  Return the matrix vector associated with this VME*/
  mflMatrixVector *GetVMatrix() {return this->MatrixVector;}
  mflMatrixVector *GetMatrixVector() {return this->GetVMatrix();}

  /**
  Set the Pose of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetPose(mflMatrix *mat);
  /** Set VME pose */
	virtual void SetPose(vtkMatrix4x4 *mat, mflTimeStamp t);
  /** Set VME pose */
  void SetMatrix(mflMatrix *mat) {this->SetPose(mat);}
  /** Set VME pose */
  void SetMatrix(vtkMatrix4x4 *mat, mflTimeStamp t=-1) {this->SetPose(mat,t);}
  /** Set VME pose */
  void SetPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t=-1);
  /** Set VME pose */
  virtual void SetPose(double xyz[3],double rxyz[3], mflTimeStamp t=-1);

  /** apply a transform to the VME pose matrix */
  void ApplyTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t=-1);
  /** apply a matrix to the VME pose matrix */
  void ApplyMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t=-1);

  /* Return the VME pose, this function queries the MatrixPipe for producing a matrix */
  void GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t=-1);
  /* Return the VME pose */
  void GetPose(double xyz[3],double rxyz[3],mflTimeStamp t=-1);
  /* Return the VME pose */
  virtual void GetPose(mflMatrix *matrix,mflTimeStamp t=-1);
  /* Return the VME pose */
  virtual mflMatrix *GetPose();
  /* Return the VME pose */
  mflMatrix *GetMatrix() {return this->GetPose();}
  /* Return the VME pose */
  void GetMatrix(mflMatrix *matrix,mflTimeStamp t=-1) {this->GetPose(matrix,t);}
  
  /**
  Set/Get the position of the VME, preserving the orientation. This function modifies the MatrixVector.*/
  void SetPosition(double x,double y,double z, mflTimeStamp t=-1);
  virtual void SetPosition(double xyz[3], mflTimeStamp t);
  void GetPosition(double &x,double &y,double &z,mflTimeStamp t=-1);
  void GetPosition(double xyz[3],mflTimeStamp t=-1);

  /**
  Set/Get the orientation of the VME, preserving the position. This function modifies the MatrixVector.*/
  void SetOrientation(double rx,double ry,double rz, mflTimeStamp t=-1);
  virtual void SetOrientation(double rxyz[3], mflTimeStamp t);
  void GetOrientation(double &rx,double &ry,double &rz,mflTimeStamp t=-1);
  void GetOrientation(double rxyz[3],mflTimeStamp t=-1);
  
  /**
  Set/Get the scale factor for the matrix at the given time*/
  //void SetScale(double sx,double sy,double sz,mflTimeStamp t=-1);
  //void SetScale(double sxyz[3],mflTimeStamp t=-1);
  void GetScale(double sxyz[3],mflTimeStamp t=-1);
  void GetScale(double &sx,double &sy,double &sz,mflTimeStamp t=-1);

   /**
  Set/Get the position of the VME, preserving the orientation. This function modifies the MatrixVector.*/
  void SetAbsPosition(double x,double y,double z, mflTimeStamp t=-1);
  void SetAbsPosition(double xyz[3], mflTimeStamp t);
  void GetAbsPosition(double &x,double &y,double &z,mflTimeStamp t=-1);
  void GetAbsPosition(double xyz[3],mflTimeStamp t=-1);

  /**
  Set/Get the orientation of the VME, preserving the position. This function modifies the MatrixVector.*/
  void SetAbsOrientation(double rx,double ry,double rz, mflTimeStamp t=-1);
  void SetAbsOrientation(double rxyz[3], mflTimeStamp t);
  void GetAbsOrientation(double &rx,double &ry,double &rz,mflTimeStamp t=-1);
  void GetAbsOrientation(double rxyz[3],mflTimeStamp t=-1);

  /** Set the global pose of this VME for the given time "t"*/
  void SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mflTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t"*/
  void SetAbsPose(double xyz[3],double rxyz[3], mflTimeStamp t);
  /** Set the global pose of this VME for the given time "t"*/
  void SetAbsPose(mflMatrix *matrix);
  /** Set the global pose of this VME for the given time "t"*/
  void SetAbsPose(vtkMatrix4x4 *matrix,mflTimeStamp t = -1);
  /** Set the global pose of this VME for the given time "t"*/

  /** Get the global pose of this VME for the given time "t"*/
  void GetAbsPose(mflMatrix *matrix,mflTimeStamp t=-1);
  /** Get the global pose of this VME for the given time "t"*/
  mflMatrix *GetAbsPose();
  /** Get the global pose of this VME for the given time "t"*/
  void GetAbsPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mflTimeStamp t=-1);
  /** Get the global pose of this VME for the given time "t"*/
  void GetAbsPose(double xyz[3],double rxyz[3],mflTimeStamp t=-1);

  /** apply a transform to the VME abs pose matrix */
  void ApplyAbsTransform(vtkLinearTransform *transform,int premultiply,mflTimeStamp t=-1);
  /** apply a matrix to the VME abs pose matrix */
  void ApplyAbsMatrix(vtkMatrix4x4 *matrix,int premultiply,mflTimeStamp t=-1);
 
  /**
  Update and return the AbsMatrix stored into this VME. */
  mflMatrix *GetAbsMatrix() {return this->GetAbsPose();}
  void GetAbsMatrix(mflMatrix *mat,mflTimeStamp t=-1) {this->GetAbsPose(mat,t);}

  /**
  Update the Absolute Matrix of this VME. This is called automatically by 
  SetCurrentTime if AutoUpdateAbsMatrix flag is ON.*/
  //void UpdateAbsMatrix(mflTimeStamp t=-1);

  /**
  return a new Tree iterator already set to traverse 
  the sub tree starting a this node*/
  mflVMEIterator *NewIterator();

  /**
  return the name of this VME*/
  const char *GetName() {return this->Name.GetCStr();};
  void GetName(mflString &name) {name=this->Name;};
  void SetName(const char *name) {this->Name=name;this->Modified();};
  void SetName(mflString name) {this->Name=name;this->Modified();};

  /**
  Compare two VME. Two VME are considered equivalent if they have equivalent 
  items, TagArrays, MatrixVectors, Name and Type. */
  bool Equals(mflVME *vme);

  /**
  Compare the two subtrees starting at this VME and at the given one. Two trees
  are considered equivalent if they have equivalent VME, disposed in the same hierarchy.
  Order of children VME is significative for comparison. */
  bool CompareTree(mflVME *vme);

  /**
  Copy the given VME tree into a new tree. In case a parent is provided, link the new
  root node to it. Return the root of the new tree.*/
  static mflVME *CopyTree(mflVME *vme, mflVME *parent=NULL);

  /**
  Make a copy of the whole subtree and return its pointer*/
  mflVME *CopyTree() {return this->CopyTree(this);}

  /**
  Reparent this VME into a different place of the same tree
  or into a different tree. If the tree is not the same, the data of
  all items of all sub vme is read into memory and Id is reset to -1, 
  to allow the VMEStorage to write new data as new files on file.
  In case of error during operation return NULL, otherwise return
  this node pointer: this is to be compatible with nodes that during
  reparenting make copy of the VME (mflVMERoot)*/
  virtual mflVME *ReparentTo(mflVME *parent);

  /**
  return true if VME can be reparented under specified node*/
  virtual int CanReparentTo(mflVME *parent) {return parent==NULL||!this->IsInTree(parent);}

  /**
  Import all children of a VME-tree into the Output */
  void Import(mflVME *tree);

  /**
  Set/Get the flag to make this VME visible to tree traversal. mflVMEIterator, 
  GetSpaceBounds and Get4DBounds will skip this VME if the flag is OFF.*/
  vtkSetMacro(VisibleToTraverse,int);
  vtkGetMacro(VisibleToTraverse,int);
  vtkBooleanMacro(VisibleToTraverse,int);

  /**
  Return true if visible to tree traversal*/
  bool IsVisible() {return this->VisibleToTraverse!=0;}
	
	/**
  Set auxiliary reference system and its name*/
	int SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  int SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);

	/**
  Get auxiliary reference system from its name*/
  int GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  int GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);

  /**
  Set/Get the matrix pipe object, i.e. the source of the output matrix. */
  int SetMatrixPipe(mflMatrixPipe *pipe);
  mflMatrixPipe *GetMatrixPipe() { if (!MatrixPipe) SetDefaultMatrixPipe(); return MatrixPipe;}
  //vtkGetObjectMacro(MatrixPipe,mflMatrixPipe);
  mflMatrixPipe *GetTransform() {return this->GetMatrixPipe();}

  /**
  return the matrix pipe used for computing the AbsMatrix.*/
  //vtkGetObjectMacro(AbsMatrixPipe,mflMatrixPipeDirectCinematic);
  mflMatrixPipeDirectCinematic *GetAbsMatrixPipe();

  
  /**
  Set/Get the data pipe object, i.e. the source of the output dataset. */
  virtual int SetDataPipe(mflDataPipe *dpipe);
  mflDataPipe *GetDataPipe() { if (!DataPipe) SetDefaultDataPipe(); return DataPipe;}
  //vtkGetObjectMacro(DataPipe,mflDataPipe);

  /**
  Set the DataPipe to the default one*/
  virtual void SetDefaultDataPipe();

  /**
  return the name of the default data pipe for this class*/
  virtual const char *GetDefaultDataPipe();

  /**
  Set the MatrixPipe to the default one*/
  virtual void SetDefaultMatrixPipe();

  /**
  return the name of the default matrix pipe for this class*/
  virtual const char *GetDefaultMatrixPipe();

  /**
  Retrieve the dataset corresponding to the current time. This is
  the output of the DataPipe currently attached to the VME.
  Usually the output is a  "smart copy" of one of the dataset in 
  the DataArray.
  If the number of stored Items is 0, or the DataPipe is not set
  a NULL pointer is returned!
  Moreover, notice the output data changes in case the DataPipe is changed. An
  event is rised when the data pipe is changed to allow attached classes to 
  update their input.*/
  vtkDataSet *GetCurrentData();
  vtkDataSet *GetOutput() {return this->GetCurrentData();}

  /**
  this function make the current data pointer to point the right output
  data, usually the DataPipe output data but suclasses can redefine this
  fuction. By default if no data pipe is defined, current data is set to
  NULL.*/
  virtual void UpdateCurrentData();

  /**
  Update data stored in the VMEItems of this object. This function
  propagate the update event in the tree up to the root
  and from there to the source object that can update the item's data.
  By default (no parameters) the data of the first item (idx=0) 
  is updated.*/
  void UpdateData(int idx=0);

  /**
  Update data of all items*/
  void UpdateAllData();
  
  /**
  Return the DataType associated with this specific VME.*/
  virtual const char *GetDataType();

  /**
  Get the pointer to the array of VMEItem's*/
  mflVMEItemArray *GetDataArray() {return this->DataArray;}

  /**
  Return the list of time stamps of the VMEItemArray stored in this VME. */
  virtual void GetDataTimeStamps(mflTimeStamp *&kframes);
  void GetDataTimeStamps(TimeVector &kframes);

  /**
  Return the list of time stamps of the MatrixVector stored in this VME. */
  void GetMatrixTimeStamps(mflTimeStamp *&kframes);
  void GetMatrixTimeStamps(TimeVector &kframes);

  /**
  Utility function to merge two different time-stamps  vectors*/
  static void MergeTimeVectors(TimeVector &outv,TimeVector v1,TimeVector v2);

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  void GetLocalTimeStamps(mflTimeStamp *&kframes);
  virtual void GetLocalTimeStamps(TimeVector &kframes);
  void GetLocalTimeStamps(vtkDoubleArray *kframes);

	/**
  Return the list of timestamps considering all parents timestamps. Timestamps list is
  obtained merging timestamps for matrixes and VME items*/
	void GetAbsTimeStamps(mflTimeStamp *&kframes);
	void GetAbsTimeStamps(TimeVector &kframes);
  
	/**
  Return the list of timestamps for the full sub tree. Timestamps list is
  obtained merging timestamps for matrixes and VME items*/
  void GetTimeStamps(mflTimeStamp *&kframes);
  void GetTimeStamps(TimeVector &kframes);  
  void GetTimeStamps(vtkDoubleArray *kframes);

  /**
  Return the number of time stamps in the whole tree*/
  int GetNumberOfTimeStamps();

  /**
  Return the number of time stamps in the whole tree*/
  int GetNumberOfLocalTimeStamps();

  /**
  Return true if the number of local time stamps is > 1*/
  virtual int IsAnimated();

  /**
  Return Time bounds interval of the only VMEItems stored in this VME. */
  void GetDataTimeBounds(mflTimeStamp tbounds[2]);

  /**
  Get TimeBounds for this VME. TimeBounds interval is defined by the minimum
  and maximum time stamps against the MatrixVector and VMEItems time stamps.
  If only the pose time stamps are required use the mflMatrixVector::GetTimeBounds()
  function. For the time bounds of the VME items only use the mflVME::GetItemsTimesList()*/
  virtual void GetLocalTimeBounds(mflTimeStamp tbounds[2]);

  /**
  Return the TimeBounds of the whole subtree, i.e recurse the GetLocalTimeBounds()
  function over all the VME in the subtree.*/
  void GetTimeBounds(mflTimeStamp tbounds[2]);

  /**
  Extract the 4D bounds for this VME only, i.e. the space bounds along all the time range
  for this VME. The first of these two functions allows to specify the time
  interval for which the time bound is required*/
  //void GetLocal4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6]);
  void GetVME4DBounds(double bounds[6]);
  void GetVME4DBounds(mflBounds &bounds);

  /**
  Extract the 4D bounds for all the subtree starting a this VME, i.e. the space bounds 
  along all the time for the VME in the subtree. The first of these two functions allows
  to specify the time interval for which the time bound is required*/
  void Get4DBounds(mflTimeStamp start, mflTimeStamp end, double bounds[6]);
  void Get4DBounds(double bounds[6]);
  void Get4DBounds(mflBounds &bounds);

  /**
  Return the space bound of the subtree for the current time, taking in consideration
  the current data bounds and the current pose matrix*/
  void GetSpaceBounds(double bounds[6]);
  virtual void GetSpaceBounds(mflBounds &bounds,mflTimeStamp t=-1, mflVMEIterator *iter=NULL);

    /**
  Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
  the current data bounds and the current pose matrix*/
  void GetVMESpaceBounds(double bounds[6]);
  virtual void GetVMESpaceBounds(mflBounds &bounds,mflTimeStamp t=-1, mflVMEIterator *iter=NULL);

  /**
  Return the local space bound (in local coordinates) of the VME (not the subtree) for the current time,
  taking in consideration the current data bounds and the current pose matrix*/
  void GetVMELocalSpaceBounds(double bounds[6]);
  virtual void GetVMELocalSpaceBounds(mflBounds &bounds,mflTimeStamp t=-1, mflVMEIterator *iter=NULL);

  /**
  Set the crypting status for the vme. */
  void SetCrypting(int crypting);

  /**
  Get the crypting status of the vme. */
  int GetCrypting();

protected:
  mflVME();
  virtual ~mflVME();

  /**
  This function is overridden by subclasses to perform custom initialization*/
  virtual int InternalInitialize() {return 0;};

  /** to be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

  virtual void UpdateData(mflVMEItem *item=NULL);

  vtkSetObjectMacro(TagArray,vtkTagArray);

  void SetAbsMatrixPipe(mflMatrixPipeDirectCinematic *pipe);

  /**
  Reimplemented as protected to avoid attaching of vtkTree parent. 
  This reimplementation sends event when VME is detached from the tree.*/
  virtual int SetParent(vtkTree *parent);

  /**
   To be use to override the current data pointer. By default CurrentData
   stores a pointer to the DataPipe output. */
  void SetCurrentData(vtkDataSet *data);

  static void OutputDataUpdateCallback(void *arg);
  static void MatrixUpdateCallback(void *arg);
  static void AbsMatrixUpdateCallback(void *arg);

  vtkTagArray *TagArray;

  mflMatrixVector *MatrixVector;
  mflVMEItemArray *DataArray;

  mflDataPipe     *DataPipe;
  mflMatrixPipe   *MatrixPipe;

  mflTimeStamp CurrentTime;

  mflString Name;

  mflMatrixPipeDirectCinematic *AbsMatrixPipe;

  unsigned long MatrixUpdateTag;
  unsigned long AbsMatrixUpdateTag;
  unsigned long DataUpdateTag;

  int VisibleToTraverse;///< disable traversing visit
  int Crypting;         ///< enable flag for this VME
  int Initialized;      ///< flag set true by Initialize()

  mflAutoPointer<mflMatrix>  CurrentMatrix;
  mflAutoPointer<vtkDataSet> CurrentData;
  
  /**
  Internally used to keep a unique ID for the VMEs*/
  static unsigned long GetNextVMEId() {return ++VMEIdCounter;}
  
  static unsigned long VMEIdCounter;

private:
  mflVME(const mflVME&); // Not implemented
  void operator=(const mflVME&); // Not implemented

  friend void mflVMEItem::UpdateData();
  static mflVME *New() {return NULL;}
};

#endif
