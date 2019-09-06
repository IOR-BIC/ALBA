/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEGenericAbstract
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEGenericAbstract_h
#define __albaVMEGenericAbstract_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDataVector;
class albaMatrixVector;
class albaVME;
class albaGUI;

/** albaVMEGenericAbstract - a VME featuring an internal array for matrices and data.
  albaVMEGenericAbstract is a node featuring a standard way for managing 4D data, featuring
  an array for pose matrixes (MatrixVector) and an array for internally stored
  data (DataSetVector).
  @sa albaVME albaMatrixVector albaDataVector

  @todo
  - 
  */
class ALBA_EXPORT albaVMEGenericAbstract : public albaVME
{
public:
  albaTypeMacro(albaVMEGenericAbstract,albaVME);

  enum VME_GUI_ID
  {
    ID_VME_CRYPTING = Superclass::ID_LAST,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /**
    Copy the contents of another VME into this one. Notice that subtrees
    are not copied, i.e. copy is not recursive!
    Concrete class should reimplement this function to verify admitted
    conversion. */
  virtual int DeepCopy(albaVME *a);

  /**
    Allow to perform a DeepCopy with data very large.
    Release memory of the input albaVME. */
  virtual int DeepCopyVmeLarge(albaVME *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(albaVME *a);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(albaVME *vme);

  /**
    Reparent this VME into a different place of the same tree
    or into a different tree. If the tree is not the same, the data of
    all items of all sub vme is read into memory and Id is reset to -1, 
    to allow the VMEStorage to write new data as new files on file.
    In case of error during operation return NULL, otherwise return
    this node pointer: this is to be compatible with nodes that during
    reparenting make copy of the VME (albaVMEGenericAbstractRoot)*/
  virtual int ReparentTo(albaVME *parent);

  /**  
    Return the matrix vector associated with this VME. Matrix vector is an array of
    time stamped 4x4 matrices, used to generate the output VME pose matrix. The matrix
    vector is made persistent by saving it in the MSF-XML file (or other kind of storage
    for metadata).
    This array can be NULL for VMEs generating the output matrix procedurally starting from from
    different sources. */
  albaMatrixVector *GetMatrixVector() {return m_MatrixVector;}

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const albaMatrix &mat);

  /** Get the pointer to the array of VMEItem's*/
  albaDataVector *GetDataVector() {return m_DataVector;}

  /** Return the list of time stamps of the VMEItemArray stored in this VME. */
  virtual void GetDataTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** Return the list of time stamps of the MatrixVector stored in this VME. */
  void GetMatrixTimeStamps(std::vector<albaTimeStamp> &kframes);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** Set the time bounds for the time varying VME based on data and matrix vector.*/
  void GetLocalTimeBounds(albaTimeStamp tbounds[2]);

  /** 
    return true is this VME has more than one time stamp, either  for
    data or matrices */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:

	/** Precess events coming from other objects */
  virtual void OnEvent(albaEventBase *alba_event);


	/** Returns StoreDataVector */
	bool GetStoreDataVector() const { return m_StoreDataVector; }

protected:
  albaVMEGenericAbstract();
  virtual ~albaVMEGenericAbstract();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** Create GUI for the VME */
  virtual albaGUI *CreateGui();

  albaMatrixVector *m_MatrixVector;
  albaDataVector   *m_DataVector;
	bool						 m_StoreDataVector;
private:
  albaVMEGenericAbstract(const albaVMEGenericAbstract&); // Not implemented
  void operator=(const albaVMEGenericAbstract&); // Not implemented
};
#endif
