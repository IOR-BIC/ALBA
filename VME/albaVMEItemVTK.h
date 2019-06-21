/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItemVTK
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEItemVTK_h
#define __albaVMEItemVTK_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEItem.h"
#include "albaStorable.h"
#include "albaString.h"
#include "albaMTime.h"
#include "vtkSmartPointer.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaOBB;
class albaStorageElement;
class albaTagArray;
class vtkDataSet;
class vtkDataReader;
class vtkDataSetWriter;

/** albaVMEItemVTK - store the single VTK dataset stored into a albaDataVector
  albaVMEItemVTK is an object that stores the single time stamped dataset of a
  albaVME. This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A albaVMEItemVTK item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see albaVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa albaVMEGenericVTK albaDataVector albaTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - implement catching of VTK I/O errors
*/
class ALBA_EXPORT albaVMEItemVTK : public albaVMEItem
{
public:  
  albaTypeMacro(albaVMEItemVTK,albaVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** copy data from another dataset */
  virtual void DeepCopy(albaVMEItem *a);

  /** copy large data from another dataset and release it.*/
  virtual void DeepCopyVmeLarge(albaVMEItem *a);

  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(albaVMEItem *a);

  /** 
    return data converted into VTK format. (supported only if ALBA is compiled
    with VTK support) */
  virtual vtkDataSet *GetData();

  /** set the VTK dataset */
  virtual void SetData(vtkDataSet *data);

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(albaVMEItem *a);

  /** Read the data file and update the item's data.*/
  virtual int ReadData(albaString &filename, int resolvedURL = ALBA_OK);

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsDataPresent() {return m_Data!=NULL;}

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

  /** Serialize the data into the compressed archive.*/
  bool StoreToArchive(wxZipOutputStream &zip);

protected:
  albaVMEItemVTK(); // to be allocated with New()
  ~albaVMEItemVTK(); // to be deleted with Delete()

  /** Check that stored file is valid and has the same dimension of that one present in memory.*/
  int CheckFile(const char *filename);

  /** Check that stored string is valid.*/
  int CheckFile(const char *input_string, int input_len);
  
  /** Update the vtk reader to read from memory or file from disk (encrypted or not).*/
  int UpdateReader(vtkDataReader *reader, albaString &filename);

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

  vtkSmartPointer<vtkDataSet> m_Data;       ///< pointer to VTK dataset
  vtkDataReader *             m_DataReader; ///< the reader used to read VTK data
  vtkDataSetWriter *          m_DataWriter; ///< the writer used to write VTK data
  int                         m_IOStatus;   ///< internally used to store the IO status
  
private:
  albaVMEItemVTK(const albaVMEItemVTK&);
  void operator=(const albaVMEItemVTK&);
};

#endif
