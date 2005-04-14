/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVME.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:09:00 $
  Version:   $Revision: 1.13 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVME_h
#define __mafVME_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafNode.h"
#include "mafVMEOutput.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafMatrix;
class mafTransform;

class mafDataPipe;
class mafMatrixPipe;

class mafAbsMatrixPipe;

/** mafVME - a tree node implementation with an output with a pose matrix and a VTK dataset.
  mafVME is a node for sciViz purposes. It features a procedural core generating an output 
  data structure storing a pose matrix and a VTK dataset. The mafVME generates also an absolute
  pose matrix, a matrix representing the pose of the VME in the world reference system.
  @sa mafNode mafVMEIterator

  @todo
  - implement the GetMTime() function: it's used by pipeline to trigger the update
  - Change the SetParent to update the AbsMatrixPipe connections
  - fix the VME_TIME_SET issuing and propagation
  - implement Update() function
  */
class MAF_EXPORT mafVME : public mafNode
{
public:
  mafAbstractTypeMacro(mafVME,mafNode);

  //typedef std::vector<mafTimeStamp> std::vector<mafTimeStamp>;

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** 
    return the parent VME Node. Notice that a VME can only reparented 
    under another VME, not to other kind of nodes! */
  mafVME *GetParent() const;

  /**
    This function set the parent for this Node. It has been redefined to update 
    AbsMatrixPipe input frame. */
  virtual int SetParent(mafNode *parent);

  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(mafNode *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(mafVME *a);

  /** Compare two VMEs. The specific comparison must be implemented by subclasses. */
  virtual bool Equals(mafVME *vme);

  /**
   Set the time for this VME (not for the whole tree). Normaly time 
   of the tree is set by sending an event with id VME_TIME_SET */
  virtual void SetCurrentTime(mafTimeStamp t);
  mafTimeStamp GetCurrentTime() {return m_CurrentTime;}
  
  /**
    Set/Get CurrentTime for this VME and all subtree. Normaly time 
    of the tree is set by sending an event with id VME_TIME_SET*/
  void SetTreeTime(mafTimeStamp t);

  /**
    Set the Pose matrix of the VME. This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  virtual void SetMatrix(const mafMatrix &mat)=0;

  /** set the pose matrix for a specific time stamp (ignore the matrix internal timestamp!) */
  void SetPose(const mafMatrix &mat,mafTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t);

  /** 
    Set the pose for this VME This function modifies pose matrix of the VME.
    The pose is modified for the given timestamp, if VME supports 4D pose (e.g. 
    MatrixVector of VME-Generic) the output matrix is interpolated among set
    key matrices.*/
  void SetPose(double xyz[3],double rxyz[3], mafTimeStamp t);

  /** apply a matrix to the VME pose matrix */
  void ApplyMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t=-1);
  
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double x,double y,double z,double rx,double ry,double rz, mafTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsPose(double xyz[3],double rxyz[3], mafTimeStamp t=-1);
  /** Set the global pose of this VME for the given time "t". This function usually modifies the MatrixVector. */
  void SetAbsMatrix(const mafMatrix &matrix,mafTimeStamp t);
  /** Set the global pose of this VME for the current time. This function usually modifies the MatrixVector. */
  void SetAbsMatrix(const mafMatrix &matrix);

  /** apply a matrix to the VME abs pose matrix */
  void ApplyAbsMatrix(const mafMatrix &matrix,int premultiply,mafTimeStamp t=-1);
 
  /**
    return true if the VME can be reparented under the specified node */
  virtual bool CanReparentTo(mafNode *parent);
  
  // to be revised
	/** Set auxiliary reference system and its name*/
	//int SetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int SetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  // to be revises
	/** Get auxiliary reference system from its name*/
  //int GetAuxiliaryRefSys(mflTransform *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  //int GetAuxiliaryRefSys(vtkMatrix4x4 *AuxRefSys, const char *RefSysName, int type = MFL_LOCAL_FRAME_TAG);
  
  /** return the matrix pipe object, i.e. the source of the output matrix. */
  mafMatrixPipe *GetMatrixPipe() {return m_MatrixPipe;}
  
  /** return the matrix pipe used for computing the AbsMatrix.*/
  mafAbsMatrixPipe *GetAbsMatrixPipe() {return m_AbsMatrixPipe;}
  
  /** return the data pipe object, i.e. the source of the output dataset. */
  mafDataPipe *GetDataPipe() {return m_DataPipe;}
  
  /**
    this function makes the current data pointer to point the right output
    data, usually the DataPipe output data but subclasses can redefine this
    function. By default if no data pipe is defined, current data is set to
    NULL.*/
  virtual void Update();
  
  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)=0;
  
	/**
    Return the list of timestamps considering all parents timestamps. Timestamps list is
    obtained merging timestamps for matrixes and VME items*/
	virtual void GetAbsTimeStamps(std::vector<mafTimeStamp> &kframes);
  
	/**
    Return the list of timestamps for the full sub tree. Timestamps list is
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetTimeStamps(std::vector<mafTimeStamp> &kframes);  
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfTimeStamps();
  
  /** Return the number of time stamps in the whole tree*/
  int GetNumberOfLocalTimeStamps();
  
  /** Return true if the number of local time stamps is > 1*/
  virtual bool IsAnimated();
  
  /** Set the crypting status for the vme. */
  void SetCrypting(int crypting);
  
  /** Get the crypting status of the vme. */
  int GetCrypting();

  /** return a pointer to the output data structure */
  virtual mafVMEOutput *GetOutput() {return m_Output;}

  /** process events coming from other components */
  void OnEvent(mafEventBase *e);
  
protected:
  mafVME(); // to be allocated with New()
  virtual ~mafVME(); // to be deleted with Delete()

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** used to initialize the AbsMatrixPipe */
  virtual int mafVME::InternalInitialize();

  /** called to prepare the update of output */
  virtual void InternalPreUpdate() {}

  /** update the output data structure */
  virtual void InternalUpdate() {}

  /** 
    Set the output and connect it to the VME. This is automatically called
    by GetOutput() of specific VME's */
  void SetOutput(mafVMEOutput *output);
  
  /** Set/Get the data pipe object, i.e. the source of the output dataset. */
  virtual int SetDataPipe(mafDataPipe *dpipe);

  /** Set the matrix pipe object, i.e. the source of the output matrix. */
  int SetMatrixPipe(mafMatrixPipe *pipe);

  /** Set the abs matrix pipe object, i.e. the source of the output abs matrix. */
  void SetAbsMatrixPipe(mafAbsMatrixPipe *pipe);

  mafAutoPointer<mafDataPipe>       m_DataPipe;
  mafAutoPointer<mafMatrixPipe>     m_MatrixPipe;
  mafAutoPointer<mafAbsMatrixPipe>  m_AbsMatrixPipe;

  mafVMEOutput  *m_Output;      ///< the data structure storing the output of this VME
  mafTimeStamp  m_CurrentTime;  ///< the time parameter for generation of the output
  int           m_Crypting;     ///< enable flag for this VME
  
private:
  mafVME(const mafVME&); // Not implemented
  void operator=(const mafVME&); // Not implemented
  virtual bool Equals(mafNode *node); // not accessible from other classes
};

#endif
