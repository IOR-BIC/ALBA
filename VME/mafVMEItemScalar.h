/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-08 14:07:45 $
  Version:   $Revision: 1.1 $
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
*/
class MAF_EXPORT mafVMEItemScalar : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemScalar,mafVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** copy data from another dataset */
  virtual void DeepCopy(mafVMEItem *a);

  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(mafVMEItem *a);

  /** 
    return data scalar. (supported only if MAF is compiled
    with ITK support which include VNL library) */
  virtual vnl_matrix<double> &GetData();

  /** set the scalar data */
  virtual void SetData(vnl_matrix<double> &data);

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *a);

  /**
  Return true if data is not empty. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsDataPresent() {return !m_Data.empty();}
  
  /**  UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds();

  /** Internally used to update data, automatically called by GetData() */
  virtual void UpdateData();

  /** return "vtk" file extension */
  virtual const char * GetDataFileExtension();

  /** destroy VTK reader and unregister VTK dataset */
  virtual void ReleaseData();

  /** return pointer to memory to be used as input. This must be set with SetInputMemory(). */
  virtual void GetOutputMemory(const char *&out_str, int &size);

  /** 
    Release memory where data has been written. This should
    be used in MEMORY mode where data is written into memory */
  virtual void ReleaseOutputMemory();

protected:
  mafVMEItemScalar(); // to be allocated with New()
  ~mafVMEItemScalar(); // to be deleted with Delete()

  /**
    Restore data stored in this object. This function asks the storage
    for the filename corresponding to the URL.
    This method is automatically called by GetData().*/
  virtual int InternalRestoreData();
  /**
    Store data stored in this object. This function asks the storage object
    for a tmp filename for saving and then call ... to store the tmp file
    into the URL. This method is called by Store().*/
  virtual int InternalStoreData(const char *url);

  vnl_matrix<double>  m_Data;       ///< pointer to VTK dataset
  mafString           m_DataString; 
  int                 m_IOStatus;   ///< internally used to store the IO status
  double              m_ScalarBouns[2];
  
private:
  mafVMEItemScalar(const mafVMEItemScalar&);
  void operator=(const mafVMEItemScalar&);
};
#endif
