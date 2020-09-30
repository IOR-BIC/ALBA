/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemScalarMatrix
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEItemScalarMatrix_h
#define __albaVMEItemScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEItem.h"
#include "albaStorable.h"
#include "albaString.h"
#include "albaMTime.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix.txx>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaOBB;
class albaStorageElement;
class albaTagArray;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

/** albaVMEItemScalarMatrix - store time-varying scalar information into a vnl_vector
  albaVMEItemScalarMatrix is an object that stores the single time stamped scalar array of a
  albaVME. This class associates a Time stamp and a Tagged list to an internally 
  stored scalar array. 
  A albaVMEItemScalarMatrix item has also a the following member variables:

  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see albaVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa albaVMEScalarMatrix albaTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - Complete the implementation of reading crypted files.
*/
class ALBA_EXPORT albaVMEItemScalarMatrix : public albaVMEItem
{
public:  
  albaTypeMacro(albaVMEItemScalarMatrix,albaVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Copy data from another scalar item*/
  virtual void DeepCopy(albaVMEItem *a);

  /** Reference another scalar item's internal data*/ 
  virtual void ShallowCopy(albaVMEItem *a);

  /** Return data scalar. (Supported only if ALBA is compiled
    with ITK support which include VNL library) */
  virtual vnl_matrix<double> &GetData();

  /** Set the scalar data */
  virtual void SetData(vnl_matrix<double> &data);

  /** Compare two scalar items. Two scalar items are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the scalar items internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(albaVMEItem *a);

  /** Read the data file and update the item's data.*/
  virtual int ReadData(albaString &filename, int resolvedURL = ALBA_OK);

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
  albaVMEItemScalarMatrix(); // to be allocated with New()
  ~albaVMEItemScalarMatrix(); // to be deleted with Delete()

  /** Check that stored file is valid and has the same dimension of that one present in memory.*/
  virtual int CheckFile(const char *filename) {return ALBA_OK;};

  /** Check that stored string is valid.*/
  virtual int CheckFile(const char *input_string, int input_len) {return ALBA_OK;};

  /** Update the vnl reader to read from memory or file from disk (encrypted or not).*/
  int UpdateReader(albaString &filename);

  /** Restore data stored in this object. This function asks the storage
    for the filename corresponding to the URL.
    This method is automatically called by GetData().*/
  virtual int InternalRestoreData();
  
  /** Store scalar item's data. This function asks the storage object
    for a tmp filename for saving and then call ... to store the tmp file
    into the URL. This method is called by Store().*/
  virtual int InternalStoreData(const char *url);

  vnl_matrix<double>  m_Data;       ///< pointer to scalar data
  albaString           m_DataString; 
  int                 m_IOStatus;   ///< internally used to store the IO status
  double              m_ScalarBouns[2];
  
private:
  albaVMEItemScalarMatrix(const albaVMEItemScalarMatrix&);
  void operator=(const albaVMEItemScalarMatrix&);
};
#endif
