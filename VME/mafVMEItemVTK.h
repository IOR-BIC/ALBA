/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEItemVTK.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 14:06:51 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEItemVTK_h
#define __mafVMEItemVTK_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEItem.h"
#include "mafStorable.h"
#include "mafString.h"
#include "mafMTime.h"
#include "vtkSmartPointer.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;
class vtkDataSet;
class vtkDataReader;
class vtkDataSetWriter;

/** mafVMEItemVTK - store the single VTK dataset stored into a mafDataVector
  mafVMEItemVTK is an object that stores the single time stamped dataset of a
  mafVME. This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A mafVMEItemVTK item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEGenericVTK mafDataVector mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
  - implement read from TmpFile
  - read from memory
  - implement catching of VTK I/O errors
*/
class MAF_EXPORT mafVMEItemVTK : public mafVMEItem
{
public:  
  mafTypeMacro(mafVMEItemVTK,mafVMEItem);

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** copy data from another dataset */
  virtual void DeepCopy(mafVMEItem *a);
  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(mafVMEItem *a);

  /** 
    return data converted into VTK format. (supported only if MAF is compiled
    with VTK support) */
  virtual vtkDataSet *GetData();

  /** set the VTK dataset */
  virtual void SetData(vtkDataSet *data);

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *a);

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

protected:
  mafVMEItemVTK(); // to be allocated with New()
  ~mafVMEItemVTK(); // to be deleted with Delete()

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
  mafVMEItemVTK(const mafVMEItemVTK&);
  void operator=(const mafVMEItemVTK&);
};

#endif
