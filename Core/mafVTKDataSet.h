/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVTKDataSet.h,v $
  Language:  C++
  Date:      $Date: 2005-03-02 00:31:47 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDataSet_h
#define __mafDataSet_h

#include "mafReferenceCounted.h"
#include "mafString.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;

/** mafDataSet - store the single item of a mafTimeMap
  mafDataSet is an object that stores the single time sample of a
  mflVME. This class associates a Time stamp and a Tagged list to a dataset.
  An item has also a the following member variables:
 
  <B>DataType<\B> to store a "textual description of data". By default its the vtkDataSet Type
  <B>Id<\B> to store a numeric unique Id (for internal use) see mflVMERoot::GetNextItemId)
  <B>FileName<\B> intrenally used to store the name of the VTK where data is stored

  @sa mflVME mflVMERoot mflVMEStorage vtkTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
*/
class MAF_EXPORT mafDataSet : public mafReferenceCounted
{
public:
  friend class mflVME;
  friend class mafDataSetArray;
  
  static mafDataSet *New();
  vtkTypeMacro(mafDataSet,mafReferenceCounted);

  void PrintSelf(ostream& os, vtkIndent indent);

  /**
  Set the internally stored Data. When data is set by means of this function 
  the flag ModifiedData is set. This flag is used to make persisent data,
  for example only modified data is saved on disk.*/
  void SetData(vtkDataSet *data);

  /**
  Get data stored into this object. Notice that data is automatically updated.*/
  vtkDataSet *GetData();

  /**
  Set the data type string name. */
  void SetDataType(const char *name) {this->DataType=name;};
  void SetDataType(mflString name) {this->DataType=name;};

  /**
  Get/Set the TimeStamp of this Data*/
  mflTimeStamp GetTimeStamp() {return this->TimeStamp;}
  void SetTimeStamp (mflTimeStamp t) {this->TimeStamp=t;}

  void DeepCopy(mafDataSet *a);
  void ShallowCopy(mafDataSet *a);

  /**
  return true (!=0) if the data stored in this object has been
  modified externally (i.e with SetData()).*/
  bool IsDataModified() {return this->ModifiedData!=0;};

  void SetFileName(mflString name) {this->FileName=name;};
  const char *GetFileName() {return this->FileName.GetCStr();};

  /**
  Used by MultimodSource to tell this object when data has been generated*/
  // .SECTION TODO
  // To be updated
  void DataHasBeenGenerated();

  /**
  TimeStamp of the last time this object has been updated by a source object*/
  vtkTimeStamp UpdateTime;

  /**
  Return the array of Tags for this object*/
  vtkTagArray *GetTagArray();

  /**
  Set/Get the a block of memory associated with this object*/
  void *GetClientData() {return this->ClientData;};
  void SetClientData(void *data) { \
    if (this->AutoDeleteClientData && this->ClientData) \
    { \
		  delete this->ClientData; \
    } \
	  this->ClientData=data; \
  }
  
  /**
  If this flag is true, automatically "delete" the memory 
  associated with this object, specified with SetClientData()*/
  void SetAutoDeleteClientData(int v) {this->AutoDeleteClientData=v;};
  int GetAutoDeleteClientData() {return this->AutoDeleteClientData;};
  vtkBooleanMacro(AutoDeleteClientData,int);

  /**
  Return the type of data stored in this object. The name returned
  is usually the class string name.*/
  const char *GetDataType() {return this->DataType.GetCStr();};
  void GetDataType(mflString &name) {name=this->DataType;};

  /**
  return the VME owning pointed this object.
  BEWARE: At present this pointer is not registered,
  thus never remove an Item from a VME directly, but only
  by means of the VME APIs.*/
  mflVME *GetVME() {return this->VME;}

  /**
  Update data stored in this object. This function propagates the
  update event in the VME object linked to this (if present) and
  back up in the tree up to the source object (if present).
  This method is automatically called by GetData()!*/
  void UpdateData();

  /**
  Return the Id of this Item. This Id is unique within the tree and
  is used internally for referencing the dataset of this item. The MaxItemId
  can be retrieved from the root of the tree.*/
  int GetId() {return this->Id;}

  /**
  Set the Id of this item. (this is typically set by the mflVMESource 
  and is used to select a file name for the data to be saved. To obtain
  a unique Id use the mflVMERoot::GetNextItemId()*/
  void SetId(int id) {this->Id=id;this->Modified();}

  /**
  Compare two VMEItems. Two VMEItems are considered equivalent if they store
  the same kind of dataset, have the same time stamp and equivalent TagArray.
  Id and filename are not considered for comparison.
  For also compare the dataset information, use SetGlobalCompareDataOn()*/
  bool Equals(mafDataSet *item);

  /**
  Set/Get the flag for enabling comparison of dataset information in the
  Equals() function. This is a global flag impacting all VMEItems instances.*/
  static void SetGlobalCompareDataFlag(int f) {GlobalCompareDataFlag=f;}
  static int GetGlobalCompareDataFlag() {return GlobalCompareDataFlag;}
  static void GlobalCompareDataFlagOn() {SetGlobalCompareDataFlag(1);}
  static void GlobalCompareDataFlagOff() {SetGlobalCompareDataFlag(0);}

  /**
  return Bounds for this item*/
  void GetBounds(double bounds[6]);
  void GetBounds(mafOBB &bounds);
  double *GetBounds();

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModiedied() can be used to know if data has been changed with respect
  to file.*/
  bool IsUpdated() {return (this->Data!=NULL);}

  // Description:
  // return time the data has been updated
  unsigned long GetUpdateTime() {return this->UpdateTime.GetMTime();}
  
  /**
  This function shouldn't be used directly by other classes but MSFWriter. It is currently
  used to reset the ModifiedData flag when data is written on this. It is going to 
  be removed in a future major refactoring of the MSFWriter.*/
  vtkBooleanMacro(ModifiedData,int);

  /** return true if data has been saved encrypted */
  int GetCrypting();
  /** internally used by mflMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(int flag);

protected:
  mafDataSet();
  ~mafDataSet();

  /**
  Flag specifying if data stored in this object has been
  modified.*/
  vtkSetMacro(ModifiedData,int);
  vtkGetMacro(ModifiedData,int);

  /**
  UpdateBounds for this data. GetBounds automatically call this function...*/
  void UpdateBounds();

  /**
  Set the VME to which this item owns. At present the VME object is
  not referenced!!! Also this function should never be called by the
  user but only by mflVME and mafDataSetArray functions.*/
  void SetVME(mflVME *vme) {this->VME=vme;}

  vtkTagArray *TagArray;
  void *ClientData;

  int AutoDeleteClientData;
  int ModifiedData;
  static int GlobalCompareDataFlag;

  int Id;
  int Crypting;

  mafString m_FileName;
  vtkDataSet *Data;
  mflTimeStamp TimeStamp;
  mflString DataType;
  mflVME *VME;

  mafOBB *m_Bounds;

private:
  mafDataSet(const mafDataSet&);  // Not implemented.
  void operator=(const mafDataSet&);  // Not implemented.
};

#endif
