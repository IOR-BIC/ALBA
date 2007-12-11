/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalarMatrix.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:22:56 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEItemScalarMatrix_h
#define __mafVMEItemScalarMatrix_h
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

/** mafVMEItemScalarMatrix - store time-varying scalar information into a vnl_vector
  mafVMEItemScalarMatrix is an object that stores the single time stamped scalar array of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored scalar array. 
  A mafVMEItemScalarMatrix item has also a the following member variables:

  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEScalarMatrix mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - Complete the implementation of reading crypted files.
*/
class MAF_EXPORT mafVMEItemScalarMatrix : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemScalarMatrix,mafVMEItem);

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

  /** Read the data file and update the item's data.*/
  virtual int ReadData(mafString &filename, int resolvedURL = MAF_OK);

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

  /** Serialize the data into the compressed archive.*/
  bool StoreToArchive(wxZipOutputStream &zip);

protected:
  mafVMEItemScalarMatrix(); // to be allocated with New()
  ~mafVMEItemScalarMatrix(); // to be deleted with Delete()

  /** Update the vnl reader to read from memory or file from disk (encrypted or not).*/
  int UpdateReader(mafString &filename);

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
  mafVMEItemScalarMatrix(const mafVMEItemScalarMatrix&);
  void operator=(const mafVMEItemScalarMatrix&);
};
#endif
