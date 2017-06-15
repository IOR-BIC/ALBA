/*=========================================================================

 Program: MAF2
 Module: mafVMEItem
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEItem_h
#define __mafVMEItem_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafReferenceCounted.h"
#include "mafTimeStamped.h"
#include "mafEventSender.h"
#include "mafStorable.h"
#include "mafString.h"
#include "mafMTime.h"
#include "mafOBB.h"
#include "mafObserver.h"
#include "mafEventBase.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class mafOBB;
class mafStorageElement;
class mafTagArray;
class vtkDataSet;
class mafVMEItemAsynchObserver;

/** mafVMEItem - store the single dataset stored into a mafDataVector
  mafVMEItem is an object that stores the single time stamped dataset of a
  mafVME. 
  This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A mafVMEItem item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see mafVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa mafVMEGeneric mafDataVector mafTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
*/
class MAF_EXPORT mafVMEItem : public mafReferenceCounted, public mafStorable, public mafEventSender, public mafTimeStamped
{
public:
  MAF_ID_DEC(VME_ITEM_DATA_MODIFIED) ///< event raised by mafVMEItem to advice DataVector a dataset has been modified

  mafAbstractTypeMacro(mafVMEItem,mafReferenceCounted);

  enum VME_ITEM_IO_ERRORS {MAF_NO_IO=MAF_USER_RETURN_VALUE+1};

  /** defined to allow DataVector copy */
  static mafObject *NewObject() {return NULL;}
  /** defined to allow DataVector copy */
  mafVMEItem *NewInstance() {return (mafVMEItem *)NewObjectInstance();}

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Get the TimeStamp of this dataset*/
  mafTimeStamp GetTimeStamp() const {return m_TimeStamp;}
  /** Set the TimeStamp of this dataset */
  void SetTimeStamp (mafTimeStamp t) {m_TimeStamp=t;}

  /** copy data from another dataset */
  virtual void DeepCopy(mafVMEItem *a);
  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(mafVMEItem *a);

  /** release data from memory */
  virtual void ReleaseData()=0;

  mafVMEItem(const mafVMEItem&){};
  void operator=(const mafVMEItem&){};

  /**
    return true if the data stored in this object has been
    externally modified (i.e with SetData()).*/
  bool IsDataModified() {return m_DataModified;};

  /** return the URL where this data is stored */
  const char *GetURL() {return m_URL;};

  /** used to set an external URL where this data is stored */
  virtual void SetURL(const char *name);

  /** if set to true this flags make the old data files to be removed */
  void ReleaseOldFileOn() {m_ReleaseOldFile = true;}
  
  /** if set to false this flags avoid old data files to be removed, as is when a SaveAs is performed */
  void ReleaseOldFileOff() {m_ReleaseOldFile = false;}

  /** 
    Return the extension to be used for data file storing data of this item.
    Returned string does not include the initial dot character (e.g. "vtk" not ".vtk") */
  virtual const char * GetDataFileExtension() = 0;

  /** 
    Store data to the storage. Write modality depends on item
    settings (@sa SetIOMode() ). */
  virtual int StoreData(const char *url);

  /** Restore data to storage. . */
  virtual int RestoreData();

  enum VME_ITEM_WRITE_MODALITIY {DEFAULT,TMP_FILE,MEMORY};

  /** 
    Set the modality for writing the data: 
    - DEFAULT to the URL
    - TMP_FILE to a local files specified with SetTmpFile()
    - MEMORY store into memory
  */
  virtual void SetIOMode(const int mode) {m_IOMode = mode;}
  int GetIOMode(){return m_IOMode;};

  void SetIOModeToDefault() {SetIOMode(DEFAULT);}
  void SetIOModeToTmpFile() {SetIOMode(TMP_FILE);}
  void SetIOModeToMemory() {SetIOMode(MEMORY);}

  /** Extract the item filename from the archive and copy the string into the m_InputMemory*/
  int ExtractFileFromArchive(mafString &archive_fullname, mafString &item_file);

  /** 
    Set the memory pointer from where the data should be read. Also memory
    size should be provided. */
  virtual void SetInputMemory(const char *int_str,unsigned long size);

  /** return pointer to memory to be used as input. This must be set with SetInputMemory(). */
  const char * GetInputMemory() {return m_InputMemory;}
  unsigned long GetInputMemorySize() {return m_InputMemorySize;}

  /** 
    Release memory where data has been written. This should
    be used in MEMORY mode where data is written into memory */
  virtual void ReleaseOutputMemory()=0;

  /** 
    Return the pointer to the data written into memory. Also memory 
    size is returned.*/
  virtual void GetOutputMemory(const char *&out_str, int &size)=0;

  /**
  Return the array of Tags for this object*/
  mafTagArray *GetTagArray();

  /**
    Return the type of data stored in this object. The name returned
    is usually the class string name.*/
  const char *GetDataType() {return m_DataType.GetCStr();};

  /** Set the data type expressed as string name. */
  void SetDataType(const char *name) {m_DataType=name;};

  /**
  Return the Id of this Item. This Id is unique within the tree and
  is used internally for referencing the dataset of this item. The MaxItemId
  can be retrieved from the root of the tree.*/
  int GetId() {return m_Id;}

  /**
    Set the Id of this dataset. (this is typically set by the datavector when storing the 
    dataset and is used to select a file name for the data to be saved. To obtain
    a unique Id it use the mafVMERoot::GetNextItemId()*/
  void SetId(mafID id) {m_Id=id;Modified();}

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(mafVMEItem *item);

  /**
    Set/Get the flag for enabling comparison of dataset internal data in the
    Equals() function. This is a global flag impacting all mafVMEItem instances.*/
  static void SetGlobalCompareDataFlag(bool f);
  static bool* GetGlobalCompareDataFlag();
  static void GlobalCompareDataFlagOn() {SetGlobalCompareDataFlag(true);}
  static void GlobalCompareDataFlagOff() {SetGlobalCompareDataFlag(false);}

  /**
  return Bounds for this item*/
  void GetBounds(double bounds[6]);
  void GetBounds(mafOBB &bounds);
  double *GetBounds();

  /**
  Return true if data is !=NULL. Currently this doesn't ensure data is the same on 
  the file. IsDataModified() can be used to know if data has been changed with respect
  to file.*/
  virtual bool IsDataPresent()=0;

  /** Return true if the data is not present on disk and has been downloaded.*/
  virtual bool IsWaitingData() {return m_IsLoadingData;};
  
  /** return time the data has been updated*/
  unsigned long GetUpdateTime() {return m_UpdateTime.GetMTime();}
  
  /** return true if data has been saved encrypted */
  bool GetCrypting();
  
  /** internally used by mafMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(bool flag);

  /**  UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds() = 0;

  /** force updating the data in the items (i.e. usaually read it from disk) */
  virtual void UpdateData() = 0;

  /** Read the data file and update the item's data.*/
  virtual int ReadData(mafString &filename, int resolvedURL = MAF_OK) = 0;

  /** Set the archive file name.
  The Archive filename is needed to restore archived items when data is stored into a single file mode.*/
  void SetArchiveFileName(mafString &archive) {m_ArchiveFileName = archive;};

  /** Get the archive file name.
  The Archive filename is needed to restore archived items when data is stored into a single file mode.*/
  const char *GetArchiveFileName() {return m_ArchiveFileName.GetCStr();};

  /** Serialize the data into the compressed archive.*/
  virtual bool StoreToArchive(wxZipOutputStream &zip) = 0;

  /** Set temp file name for the item.*/
  void SetTempFileName(mafString &tmp) {m_TmpFileName = tmp;};

  /** Return the temp file name used by the item.*/
  const char *GetTempFileName() {return m_TmpFileName.GetCStr();};

  /** Update Item Id  */
  void UpdateItemId();

protected:
  mafVMEItem(); // to be allocated with New()
  ~mafVMEItem(); // to be deleted with Delete()

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** Check that stored file is valid.*/
  virtual int CheckFile(const char *filename) = 0;

  /** Check that stored string is valid.*/
  virtual int CheckFile(const char *input_string, int input_len) = 0;

  /**
    Restore data stored in this object. To be redefined by specialized classes.
    This method is automatically called by GetData(). 
    Return MAF_OK if data has been read, MAF_ERROR in case of I/O errors
    and MAF_NO_IO in case no I/O is really performed (e.g. data already
    present). */
  virtual int InternalRestoreData()=0;
  
  /**
    Store data stored in this object. To be redefined by specialized classes.
    Return MAF_OK if data has been read, MAF_ERROR in case of I/O errors
    and MAF_NO_IO in case no I/O is really performed (e.g. data already
    present on storage). */
  virtual int InternalStoreData(const char *url)=0; 

  /**
    Internally used to specify if data stored in this object has been
    modified with respect to saved data.*/
  void SetDataModified(bool flag);
  bool GetDataModified() {return m_DataModified;}

  /**
  Set the VME to which this item owns. At present the VME object is
  not referenced!!! Also this function should never be called by the
  user but only by mafVME and mafVMEItemArray functions.*/
  //void SetVME(mafVME *vme) {m_VME=vme;}

  mafTagArray * m_TagArray;             ///< meta data attributes attached to this dataset      
  bool          m_DataModified;         ///< true when data has been modified from last storing
  // static bool   m_GlobalCompareDataFlag;///< if true Equals will also compare internal data

  bool          m_ReleaseOldFile;       ///< whether to release the old VTK file when data filename changes (set by mafDataVector::InternalStore() ) 

  int           m_Id;         ///< the id assigned to the dataset/file to be stored in the MSF
  bool          m_Crypting;   ///< this flags specify if encryption should be used when saving data

  std::string   m_DecryptedFileString; ///< String containing the decrypted file in memory
  mafString     m_URL;        ///< the URL of the data file for this dataset
  mafTimeStamp  m_TimeStamp;  ///< time stamp of this dataset
  mafString     m_DataType;   ///< the dataset type expressed as a string 
  mafOBB        m_Bounds;     ///< spatial bounds for this dataset
  mafMTime      m_UpdateTime; ///< store modification timestamp for last update
  bool          m_IsLoadingData; ///< Set when item is loading data to prevent setting DataModified to true

  mafString     m_ArchiveFileName; ///< Filename of the archive if single file mode is enabled
  mafString     m_TmpFileName;///< file name used for local cache
  int           m_IOMode;     ///< IO modality to be used for store/restore data
  const char *  m_InputMemory;///< pointer to memory storing data to be read
  unsigned long m_InputMemorySize;  ///< size of memory storing data to be read
  const char *  m_OutputMemory;     ///< pointer to memory storing the data to be written
  unsigned long m_OutputMemorySize; ///< size of the block of memory where data has been stored
  mafVMEItemAsynchObserver *m_DataObserver; ///< observer used to update the item's data when downloaded
  mafString     m_ChecksumMD5; ///< Store the MD5 checksum for the data associated to the item.
};

/** mafVMEItemAsynchObserver - used by the mafVMEItem to synchronize the asynchronous
loading data from remote storages.

@sa mafVMEItemVTK mafVMEItemScalar
*/
class MAF_EXPORT mafVMEItemAsynchObserver : public mafObserver
{
public:
  MAF_ID_DEC(VME_ITEM_DATA_DOWNLOADED); ///< event used to update the mafVMEItem data when download is finished.

  mafVMEItemAsynchObserver();
  virtual	~mafVMEItemAsynchObserver();

  virtual void OnEvent(mafEventBase *maf_event);

  /** Set the item to which send the update event when the asynchronous
  message from the storage comes up.*/
  void SetItem(mafVMEItem *item) {m_Item = item;};

  void SetFileName(mafString &filename) {m_Filename = filename;};

protected:
  mafVMEItem *m_Item; ///< Item to update when the loading of the binary data finish
  mafString   m_Filename; ///< Filename downloaded and to be read.
};
#endif
