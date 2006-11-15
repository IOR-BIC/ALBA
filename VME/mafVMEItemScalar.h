/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-11-15 14:37:25 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEItemScalar_h
#define __mafVMEItemScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEItem.h"
#include "mafStorable.h"
#include "mafString.h"
#include "mafMTime.h"

#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;

/** mafVMEItemScalar - store time-varying scalar information into a vnl_vector
  mafVMEItemScalar is an object that stores the single time stamped scalar array of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored scalar array. 
  A mafVMEItemScalar item has also a the following member variables:

  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEScalar mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - Complete the implementation of reading crypted files.
*/
class MAF_EXPORT mafVMEItemScalar : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemScalar,mafVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Copy data from another scalar item*/
  virtual void DeepCopy(mafVMEItem *a);

  /** Reference another scalar item's internal data*/ 
  virtual void ShallowCopy(mafVMEItem *a);

  /** Return data scalar. (Supported only if MAF is compiled
    with ITK support which include VNL library) */
  virtual vnl_matrix<double> &GetData();

  /** Set the scalar data */
  virtual void SetData(vnl_matrix<double> &data);

  /** Compare two scalar items. Two scalar items are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the scalar items internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *a);

  /** Return true if scalar data is not empty. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsDataPresent() {return !m_Data.empty();}
  
  /** UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds();

  /** Internally used to update data, automatically called by GetData() */
  virtual void UpdateData();

  /** Return "sca" file extension */
  virtual const char * GetDataFileExtension();

  /** Clear scalar data.*/
  virtual void ReleaseData();

  /** Return pointer to memory to be used as input. This must be set with SetInputMemory(). */
  virtual void GetOutputMemory(const char *&out_str, int &size);

  /** Release memory where data has been written. This should
    be used in MEMORY mode where data is written into memory */
  virtual void ReleaseOutputMemory();

protected:
  mafVMEItemScalar(); // to be allocated with New()
  ~mafVMEItemScalar(); // to be deleted with Delete()

  /** Restore data stored in this object. This function asks the storage
    for the filename corresponding to the URL.
    This method is automatically called by GetData().*/
  virtual int InternalRestoreData();
  
  /** Store scalar item's data. This function asks the storage object
    for a tmp filename for saving and then call ... to store the tmp file
    into the URL. This method is called by Store().*/
  virtual int InternalStoreData(const char *url);

  vnl_matrix<double>  m_Data;       ///< pointer to scalar data
  mafString           m_DataString; 
  int                 m_IOStatus;   ///< internally used to store the IO status
  double              m_ScalarBouns[2];
  
private:
  mafVMEItemScalar(const mafVMEItemScalar&);
  void operator=(const mafVMEItemScalar&);
};
#endif
