/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKDataSet.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 10:16:33 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVTKDataSet_h
#define __mafVTKDataSet_h

#include "mafVMEItem.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** mafVTKDataSet - store a VTK dataset marked with a mafTimeMap.

*/
class MAF_EXPORT mafVTKDataSet : public mafVMEItem
{
public:
  mafTypeMacro(mafVTKDataSet,mafVMEItem);

  virtual void Print(std::ostream& os, const int indent);

  /** 
    return data converted into VTK format. (supported only if MAF is compiled
    with VTK support) */
  virtual vtkDataSet *GetVTKData();

  /**
  Set the internally stored Data. When data is set by means of this function 
  the flag ModifiedData is set. This flag is used to make persisent data,
  for example only modified data is saved on disk.*/
  void SetData(vtkDataSet *data);

  /**
  Get data stored into this object. Notice that data is automatically updated.*/
  vtkDataSet *GetData();

   /** copy dat from another dataset */
  virtual void DeepCopy(mafVMEItem *a);
  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(mafVMEItem *a);

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *item);

  /**
    Restore data stored in this object. This function propagates the
    update event in the VME object linked to this (if present) and
    back up in the tree up to the storage object attached to the root (if present).
    This method is automatically called by GetData().*/
  virtual void RestoreData()=0;

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsDataPresent()=0;
  
protected:
  mafVTKDataSet();
  ~mafVTKDataSet();
  
  //virtual int InternalStore(mafStorageElement *parent);
  //virtual int InternalRestore(mafStorageElement *node);

  virtual int InternalRestoreData();
  virtual int InternalStoreData();

  vtkDataSet *m_VTKData;
};

#endif
