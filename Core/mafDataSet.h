/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataSet.h,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:32:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDataSet_h
#define __mafDataSet_h

#include "mafReferenceCounted.h"
#include "mafStorable.h"
#include "mafString.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mmaTagArray;
class vtkDataSet;

/** mafDataSet - store the single dataset stored into a mafTimeMap
  mafDataSet is an object that stores the single time stamped dataset of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A mafDataSet item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVME mafVMERoot mafMSFStorage mafTimeMap mmaTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
*/
class MAF_EXPORT mafDataSet : public mafReferenceCounted, public mafStorable
{
public:  
  mafAbstractTypeMacro(mafDataSet,mafReferenceCounted);

  void Print(std::ostream& os, const int indent);

  /** Get the TimeStamp of this dataset*/
  mafTimeStamp GetTimeStamp() {return m_TimeStamp;}
  /** Set the TimeStamp of this dataset */
  void SetTimeStamp (mafTimeStamp t) {m_TimeStamp=t;}

  /** copy dat from another dataset */
  virtual void DeepCopy(mafDataSet *a);
  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(mafDataSet *a);

  mafDataSet(const mafDataSet&);
  void operator=(const mafDataSet&);

  /**
    return true if the data stored in this object has been
    externally modified (i.e with SetData()).*/
  bool IsDataModified() {return m_ModifiedData;};

  /** return the URL where this data is stored */
  const char *GetURL() {return m_URL;};

  /**
  Return the array of Tags for this object*/
  mmaTagArray *GetTagArray();

  /**
    Return the type of data stored in this object. The name returned
    is usually the class string name.*/
  const char *GetDataType() {return m_DataType.GetCStr();};

  /**
    return the VME owning pointed this object.
    BEWARE: At present this pointer is not registered,
    thus never remove an Item from a VME directly, but only
    by means of the VME APIs.*/
  mafVME *GetVME() {return m_VME;}

  /**
    Restore data stored in this object. This function propagates the
    update event in the VME object linked to this (if present) and
    back up in the tree up to the storage object attached to the root (if present).
    This method is automatically called by GetData().*/
  virtual void RestoreData()=0;

  /** 
    return data converted into VTK format. (supported only if MAF is compiled
    with VTK support) */
  virtual vtkDataSet *GetVTKData() {return NULL;}

  /**
  Return the Id of this Item. This Id is unique within the tree and
  is used internally for referencing the dataset of this item. The MaxItemId
  can be retrieved from the root of the tree.*/
  int GetId() {return m_Id;}

  /**
    Set the Id of this dataset. (this is typically set by the when storing the 
    dataset and is used to select a file name for the data to be saved. To obtain
    a unique Id it use the mafVMERoot::GetNextItemId()*/
  void SetId(mafID id) {m_Id=id;Modified();}

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafDataSet *item);

  /**
    Set/Get the flag for enabling comparison of dataset internal data in the
    Equals() function. This is a global flag impacting all mafDataSet instances.*/
  static void SetGlobalCompareDataFlag(bool f) {m_GlobalCompareDataFlag=f;}
  static int GetGlobalCompareDataFlag() {return m_GlobalCompareDataFlag;}
  static void GlobalCompareDataFlagOn() {SetGlobalCompareDataFlag(true);}
  static void GlobalCompareDataFlagOff() {SetGlobalCompareDataFlag(false);}

  /**
  return Bounds for this item*/
  void GetBounds(double bounds[6]);
  void GetBounds(mafOBB &bounds);
  double *GetBounds();

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModiedied() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsUpdated();
  // Description:
  // return time the data has been updated
  unsigned long GetUpdateTime() {return m_UpdateTime.GetMTime();}
  
  /** return true if data has been saved encrypted */
  bool GetCrypting();
  
  /** internally used by mafMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(bool flag);

  /**  UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds()=0;

  /** increment update modification time */
  void Modified() {m_MTime.Modified();}

  /** return modification time */
  unsigned long GetMTime() {return m_MTime.GetMTime();}

protected:
  mafDataSet();
  ~mafDataSet();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);


  /** internally used to set the URL where this data is stored */
  void SetURL(const char *name) {m_URL=name;};

  /** Set the data type expressed as string name. */
  void SetDataType(const char *name) {m_DataType=name;};

  /**
    Internally used to specify if data stored in this object has been
    modified with respect to saved data.*/
  void SetModifiedData(bool flag);
  bool GetModifiedData() ;

  /**
  Set the VME to which this item owns. At present the VME object is
  not referenced!!! Also this function should never be called by the
  user but only by mflVME and mafDataSetArray functions.*/
  void SetVME(mafVME *vme) {m_VME=vme;}

  mmaTagArray   *m_TagArray;             ///< meta data attributes attached to this dataset      
  bool          m_ModifiedData;         ///< true when data has been mofied from last storing
  static bool   m_GlobalCompareDataFlag;///< if true Equals will also compare internal data

  int           m_Id;         ///< the id assigned to the dataset/file to be stored in the MSF
  bool          m_Crypting;   ///< this flags specify if crypting should be used when saving data

  mafString     m_URL;        ///< the URL of the data file for this dataset
  mafTimeStamp  m_TimeStamp;  ///< time stamp of this dataset
  mafString     m_DataType;   ///< the dataset type expressed as a string 
  mafVME        *m_VME;       ///< the VME this dataset is part of
  mafOBB        *m_Bounds;     ///< spatial bounds for this dataset
  mafMTime      m_UpdateTime; ///< store modification timestamp for last update
  mafMTime      m_MTime;      ///< Last modification time

};

#endif
