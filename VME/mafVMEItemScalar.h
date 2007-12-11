/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemScalar.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:23:37 $
  Version:   $Revision: 1.7 $
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

#define MAF_SCALAR_MIN -1.0e+299
#define MAF_SCALAR_MAX 1.0e+299

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;

/** mafVMEItemScalar - store time-varying scalar information into a double data stored into the XML part of the msf.
  mafVMEItemScalar is an object that stores the single time stamped scalar array of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored scalar array. 
  A mafVMEItemScalar item has also a the following member variables:

  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEScalar mafTagArray
*/
class MAF_EXPORT mafVMEItemScalar : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemScalar, mafVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Copy data from another scalar item*/
  virtual void DeepCopy(mafVMEItem *a);

  /** Reference another scalar item's internal data*/ 
  virtual void ShallowCopy(mafVMEItem *a);

  /** Return scalar data.*/
  virtual double GetData();

  /** Set the scalar data */
  virtual void SetData(double data);

  /** Compare two scalar items. Two scalar items are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the scalar items internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *a);

  /** Read the data file and update the item's data.*/
  virtual int ReadData(mafString &filename, int resolvedURL = MAF_OK);

  /** Return true if scalar data is different from MAF_SCALAR_MIN. 
  Currently this doesn't ensure data is the same on the file. 
  IsDataModified() can be used to know if data has been changed with respect to file.*/
  virtual bool IsDataPresent() {return !mafEquals(m_Data, MAF_SCALAR_MIN);}
  
  /** UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds();

  /** Internally used to update data, automatically called by GetData() */
  virtual void UpdateData();

  /** Return "sca" file extension */
  virtual const char *GetDataFileExtension();

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
  mafVMEItemScalar(); // to be allocated with New()
  ~mafVMEItemScalar(); // to be deleted with Delete()

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** Scalar data is stored into the XML part of the MSF, so it is restored during the MSF file load.*/
  virtual int InternalRestoreData();
  
  /** Scalar data is stored into the XML part of the MSF, so it is stored during the MSF file store.*/
  virtual int InternalStoreData(const char *url);

  double     m_Data;       ///< scalar data
  mafString  m_DataString; 
  int        m_IOStatus;   ///< internally used to store the IO status
  double     m_ScalarBouns[2];

private:
  mafVMEItemScalar(const mafVMEItemScalar&);
  void operator=(const mafVMEItemScalar&);
};
#endif
