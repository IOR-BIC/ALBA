/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEGeneric.h,v $
  Language:  C++
  Date:      $Date: 2005-03-10 12:36:17 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMEGeneric_h
#define __mafVMEGeneric_h

#include "mafVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDataVector;
class mafMatrixVector;

/** mafVMEGeneric - a VME featuring an internal array for matrices and data.
  mafVMEGeneric is a node featuring a standard way for managing 4D data, featuring
  an array for pose matrixes (MatrixVector) and an array for internally stored
  data (DataSetVector).
  @sa mafVME mafMatrixVector mafDataVector

  @todo
  - 
  */
class MAF_EXPORT mafVMEGeneric : public mafVME
{
public:

  mafTypeMacro(mafVMEGeneric,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs);

  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(mafVME *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(mafVME *a);
  
  /**
    Query for VME child nodes */
  mafVMEGeneric *GetVMEChild(mafID idx) {return mafVMEGeneric::SafeDownCast(this->Superclass::GetChild(idx));}
  mafVMEGeneric *GetFirstVMEChild();
  mafVMEGeneric *GetLastVMEChild();

  /**
   Set the time for this VME (not for the whole tree). Normaly time 
   of the tree is set by sending an event with id VME_TIME_SET */
  virtual void SetCurrentTime(mafTimeStamp t);
  
  /**  
    Return the matrix vector associated with this VME. Matrix vector is an array of
    time stamped 4x4 matrices, used to generate the output VME pose matrix. The matrix
    vector is made persistent by saving it in the MSF-XML file (or other kind of storage
    for metadata).
    This array can be NULL for VMEs generating the output matrix procedurally starting from from
    different sources. */
  mafMatrixVector *GetMatrixVector();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(mafMatrix *mat);
  /** Set VME pose matrix for the given time */
  void SetMatrix(vtkMatrix4x4 *mat, mafTimeStamp t=-1);
 
  /** apply a transform to the VME pose matrix */
  void ApplyTransform(vtkLinearTransform *transform,int premultiply,mafTimeStamp t=-1);
  /** apply a matrix to the VME pose matrix */
  void ApplyMatrix(vtkMatrix4x4 *matrix,int premultiply,mafTimeStamp t=-1);

  /* Query functions to be moved to Output data structure */
  /** Return the VME pose, this function queries the MatrixPipe for producing a matrix */
  //void GetPose(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1);
  /** Return the VME pose */
  //void GetPose(double xyz[3],double rxyz[3],mafTimeStamp t=-1);
  /** Return the VME pose */
  //virtual void GetPose(mafMatrix *matrix,mafTimeStamp t=-1);
  /** Return the VME pose matrix for the current time */
  //virtual mafMatrix *GetPose();
  /** Return the VME pose matrix for the current time */
  //mafMatrix *GetMatrix() {return this->GetPose();}
  /** Return the VME pose matrix for the give time */
  //void GetMatrix(mafMatrix *matrix,mafTimeStamp t=-1) {this->GetPose(matrix,t);}
  
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(double xyz[3],double rxyz[3], mafTimeStamp t);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(mafMatrix *matrix);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(vtkMatrix4x4 *matrix,mafTimeStamp t = -1);
  

  /* To be moved to the output data structure */
  /** Get the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  //void GetAbsMatrix(mafMatrix *matrix,mafTimeStamp t=-1);
  /** Get the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  //mafMatrix *GetAbsMatrix();
  /** Get the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  //void GetAbsMatrix(double &x,double &y,double &z,double &rx,double &ry,double &rz,mafTimeStamp t=-1);
  /** Get the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  //void GetAbsMatrix(double xyz[3],double rxyz[3],mafTimeStamp t=-1);

  /** apply a transform to the VME abs pose matrix */
  void ApplyAbsTransform(vtkLinearTransform *transform,int premultiply,mafTimeStamp t=-1);
  /** apply a matrix to the VME abs pose matrix */
  void ApplyAbsMatrix(vtkMatrix4x4 *matrix,int premultiply,mafTimeStamp t=-1);
 
  /**
    return a new Tree iterator already set to traverse 
    the sub tree starting a this node*/
  //mafVMEGenericIterator *NewIterator();

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(mafVME *vme);

  /**
    Compare the two subtrees starting at this VME and at the given one. Two trees
    are considered equivalent if they have equivalent VME, disposed in the same hierarchy.
    Order of children VME is significative for comparison. */
  bool CompareTree(mafVMEGeneric *vme);

  /**
    Copy the given VME tree into a new tree. In case a parent is provided, link the new
    root node to it. Return the root of the new tree.*/
  static mafVMEGeneric *CopyTree(mafVMEGeneric *vme, mafVMEGeneric *parent=NULL);

  /** Make a copy of the whole subtree and return its pointer*/
  mafVMEGeneric *CopyTree() {return this->CopyTree(this);}

  /**
    Reparent this VME into a different place of the same tree
    or into a different tree. If the tree is not the same, the data of
    all items of all sub vme is read into memory and Id is reset to -1, 
    to allow the VMEStorage to write new data as new files on file.
    In case of error during operation return NULL, otherwise return
    this node pointer: this is to be compatible with nodes that during
    reparenting make copy of the VME (mafVMEGenericRoot)*/
  virtual mafVMEGeneric *ReparentTo(mafVMEGeneric *parent);

  /** return true if VME can be reparented under specified node*/
  virtual int CanReparentTo(mafVMEGeneric *parent) {return parent==NULL||!this->IsInTree(parent);}

  /** To be moved to mafVME */
  /** Import all children of a VME-tree into the Output */
  void Import(mafVMEGeneric *tree);

  
	/** Set auxiliary reference system and its name*/
	//int SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);

	/** Get auxiliary reference system from its name*/
  //int GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);

  // OK
  /** Set/Get the matrix pipe object, i.e. the source of the output matrix. */
  //int SetMatrixPipe(mafMatrixPipe *pipe);
  //mafMatrixPipe *GetMatrixPipe() { if (!MatrixPipe) SetDefaultMatrixPipe(); return MatrixPipe;}

  // OK
  /** return the matrix pipe used for computing the AbsMatrix.*/
  //mafMatrixPipeDirectCinematic *GetAbsMatrixPipe();

  // OK
  /** Set/Get the data pipe object, i.e. the source of the output dataset. */
  //virtual int SetDataPipe(mflDataPipe *dpipe);
  //mflDataPipe *GetDataPipe() { if (!DataPipe) SetDefaultDataPipe(); return DataPipe;}

  // To be moved to output data structure
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
  //vtkDataSet *GetCurrentData();
  //vtkDataSet *GetOutput() {return this->GetCurrentData();}

  /**
    this function make the current data pointer to point the right output
    data, usually the DataPipe output data but suclasses can redefine this
    fuction. By default if no data pipe is defined, current data is set to
    NULL.*/
  virtual void Update();
  
  // To be moved to output data structure
  /**
    Return the DataType associated with this specific VME.*/
  //virtual const char *GetDataType();

  /** Get the pointer to the array of VMEItem's*/
  mafDataVector *GetDataArray() {return this->DataArray;}

  /** Return the list of time stamps of the VMEItemArray stored in this VME. */
  virtual void GetDataTimeStamps(mafTimeStamp *&kframes);
  void GetDataTimeStamps(TimeVector &kframes);

  /** Return the list of time stamps of the MatrixVector stored in this VME. */
  void GetMatrixTimeStamps(mafTimeStamp *&kframes);
  void GetMatrixTimeStamps(TimeVector &kframes);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  void GetLocalTimeStamps(mafTimeStamp *&kframes);
  virtual void GetLocalTimeStamps(TimeVector &kframes);
  void GetLocalTimeStamps(vtkDoubleArray *kframes);

	/**
    Return the list of timestamps considering all parents timestamps. Timestamps list is
    obtained merging timestamps for matrixes and VME items*/
	void GetAbsTimeStamps(mafTimeStamp *&kframes);
	void GetAbsTimeStamps(TimeVector &kframes);
  
	/**
    Return the list of timestamps for the full sub tree. Timestamps list is
    obtained merging timestamps for matrixes and VME items*/
  void GetTimeStamps(mafTimeStamp *&kframes);
  void GetTimeStamps(TimeVector &kframes);  
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfTimeStamps();
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfLocalTimeStamps();
  
  // To be moved to output data structure
  /** Return true if the number of local time stamps is > 1*/
  virtual int IsAnimated();
  
  /**
    Return Time bounds interval of the only VMEItems stored in this VME. */
  void GetDataTimeBounds(mafTimeStamp tbounds[2]);

  // To be moved to output data structure
  /**
    Get TimeBounds for this VME. TimeBounds interval is defined by the minimum
    and maximum time stamps against the MatrixVector and VMEItems time stamps.
    If only the pose time stamps are required use the mafMatrixVector::GetTimeBounds()
    function. For the time bounds of the VME items only use the mafVMEGeneric::GetItemsTimesList()*/
  virtual void GetLocalTimeBounds(mafTimeStamp tbounds[2]);
  
  // To be moved to output data structure
  /**
    Return the TimeBounds of the whole subtree, i.e recurse the GetLocalTimeBounds()
    function over all the VME in the subtree.*/
  void GetTimeBounds(mafTimeStamp tbounds[2]);
  
  // To be moved to output data structure
  /**
    Extract the 4D bounds for this VME only, i.e. the space bounds along all the time range
    for this VME. The first of these two functions allows to specify the time
    interval for which the time bound is required*/
  void GetLocal4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6]);
  void GetVME4DBounds(double bounds[6]);
  void GetVME4DBounds(mafOBB &bounds);

  // To be moved to output data structure
  /**
    Extract the 4D bounds for all the subtree starting a this VME, i.e. the space bounds 
    along all the time for the VME in the subtree. The first of these two functions allows
    to specify the time interval for which the time bound is required*/
  void Get4DBounds(mafTimeStamp start, mafTimeStamp end, double bounds[6]);
  void Get4DBounds(double bounds[6]);
  void Get4DBounds(mafOBB &bounds);

  // To be moved to output data structure
  /**
    Return the space bound of the subtree for the current time, taking in consideration
    the current data bounds and the current pose matrix*/
  void GetSpaceBounds(double bounds[6]);
  virtual void GetSpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEGenericIterator *iter=NULL);
  
  // To be moved to output data structure
  /**
    Return the space bound of the VME (not the subtree) for the current time, taking in consideration 
    the current data bounds and the current pose matrix*/
  void GetVMESpaceBounds(double bounds[6]);
  virtual void GetVMESpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEGenericIterator *iter=NULL);
  
  // To be moved to output data structure
  /**
    Return the local space bound (in local coordinates) of the VME (not the subtree) for the current time,
    taking in consideration the current data bounds and the current pose matrix*/
  void GetVMELocalSpaceBounds(double bounds[6]);
  virtual void GetVMELocalSpaceBounds(mafOBB &bounds,mafTimeStamp t=-1, mafVMEGenericIterator *iter=NULL);
  
  /** Set the crypting status for the vme. */
  void SetCrypting(int crypting);
  
  /** Get the crypting status of the vme. */
  int GetCrypting();
  
protected:
  mafVMEGeneric();
  virtual ~mafVMEGeneric();

  /** This function is overridden by subclasses to perform custom initialization*/
  virtual int InternalInitialize() {return 0;};

  /** To be redefined by subclasses to define the shutdown actions */
  virtual void InternalShutdown() {};

  //void SetAbsMatrixPipe(mafMatrixPipeDirectCinematic *pipe);

  /**
    Reimplemented as protected to avoid attaching of mafVME parent. 
    This reimplementation sends event when VME is detached from the tree.*/
  virtual int SetParent(mafVME *parent);

  // To be moved to output data structure
  /**
   To be use to override the current data pointer. By default CurrentData
   stores a pointer to the DataPipe output. */
  //void SetCurrentData(vtkDataSet *data);

  //static void OutputDataUpdateCallback(void *arg);
  //static void MatrixUpdateCallback(void *arg);
  //static void AbsMatrixUpdateCallback(void *arg);

  mafMatrixVector *m_MatrixVector;
  mafDataVector *m_DataArray;

  mflDataPipe     *m_DataPipe;
  mafMatrixPipe   *m_MatrixPipe;

  mafTimeStamp m_CurrentTime; ///< the time parameter for generation of the output

  mafMatrixPipeDirectCinematic *m_AbsMatrixPipe;

  //unsigned long MatrixUpdateTag;
  //unsigned long AbsMatrixUpdateTag;
  //unsigned long DataUpdateTag;

  int m_Crypting;             ///< enable flag for this VME

  // to be moved to output data structure
  //mafAutoPointer<mafMatrix>  CurrentMatrix;
  //mafAutoPointer<vtkDataSet> CurrentData;
  
private:
  mafVMEGeneric(const mafVMEGeneric&); // Not implemented
  void operator=(const mafVMEGeneric&); // Not implemented
};

#endif
