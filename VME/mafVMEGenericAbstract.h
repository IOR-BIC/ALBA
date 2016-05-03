/*=========================================================================

 Program: MAF2
 Module: mafVMEGenericAbstract
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEGenericAbstract_h
#define __mafVMEGenericAbstract_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafDataVector;
class mafMatrixVector;
class mafVME;
class mafGUI;

/** mafVMEGenericAbstract - a VME featuring an internal array for matrices and data.
  mafVMEGenericAbstract is a node featuring a standard way for managing 4D data, featuring
  an array for pose matrixes (MatrixVector) and an array for internally stored
  data (DataSetVector).
  @sa mafVME mafMatrixVector mafDataVector

  @todo
  - 
  */
class MAF_EXPORT mafVMEGenericAbstract : public mafVME
{
public:
  mafTypeMacro(mafVMEGenericAbstract,mafVME);

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
  virtual int DeepCopy(mafVME *a);

  /**
    Allow to perform a DeepCopy with data very large.
    Release memory of the input mafVME. */
  virtual int DeepCopyVmeLarge(mafVME *a);

  /** 
    perform a copy by simply referencing the copied VME's data array. 
    Beware: This can allow to save memory when doing special tasks, but
    can be very dangerous making one of the VME inconsistent. Some VMEs
    do not support such a function! */  
  virtual int ShallowCopy(mafVME *a);

  /**
    Compare two VME. Two VME are considered equivalent if they have equivalent 
    items, TagArrays, MatrixVectors, Name and Type. */
  virtual bool Equals(mafVME *vme);

  /**
    Reparent this VME into a different place of the same tree
    or into a different tree. If the tree is not the same, the data of
    all items of all sub vme is read into memory and Id is reset to -1, 
    to allow the VMEStorage to write new data as new files on file.
    In case of error during operation return NULL, otherwise return
    this node pointer: this is to be compatible with nodes that during
    reparenting make copy of the VME (mafVMEGenericAbstractRoot)*/
  virtual int ReparentTo(mafVME *parent);

  /**  
    Return the matrix vector associated with this VME. Matrix vector is an array of
    time stamped 4x4 matrices, used to generate the output VME pose matrix. The matrix
    vector is made persistent by saving it in the MSF-XML file (or other kind of storage
    for metadata).
    This array can be NULL for VMEs generating the output matrix procedurally starting from from
    different sources. */
  mafMatrixVector *GetMatrixVector() {return m_MatrixVector;}

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat);

  /** Get the pointer to the array of VMEItem's*/
  mafDataVector *GetDataVector() {return m_DataVector;}

  /** Return the list of time stamps of the VMEItemArray stored in this VME. */
  virtual void GetDataTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Return the list of time stamps of the MatrixVector stored in this VME. */
  void GetMatrixTimeStamps(std::vector<mafTimeStamp> &kframes);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Set the time bounds for the time varying VME based on data and matrix vector.*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]);

  /** 
    return true is this VME has more than one time stamp, either  for
    data or matrices */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();   //SIL. 11-4-2005:

  void OnEvent(mafEventBase *maf_event);


	/** Returns StoreDataVector */
	bool GetStoreDataVector() const { return m_StoreDataVector; }

protected:
  mafVMEGenericAbstract();
  virtual ~mafVMEGenericAbstract();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** Create GUI for the VME */
  virtual mafGUI *CreateGui();

  mafMatrixVector *m_MatrixVector;
  mafDataVector   *m_DataVector;
	bool						 m_StoreDataVector;
private:
  mafVMEGenericAbstract(const mafVMEGenericAbstract&); // Not implemented
  void operator=(const mafVMEGenericAbstract&); // Not implemented
};
#endif
