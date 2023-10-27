/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEItem
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEItem_h
#define __albaVMEItem_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "albaReferenceCounted.h"
#include "albaTimeStamped.h"
#include "albaEventSender.h"
#include "albaStorable.h"
#include "albaString.h"
#include "albaMTime.h"
#include "albaOBB.h"
#include "albaObserver.h"
#include "albaEventBase.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class albaOBB;
class albaStorageElement;
class albaTagArray;
class vtkDataSet;
class albaVMEItemAsynchObserver;

/** albaVMEItem - store the single dataset stored into a albaDataVector
  albaVMEItem is an object that stores the single time stamped dataset of a
  albaVME. 
  This class associates a Time stamp and a Tagged list to an internally 
  stored dataset. The type of dataset is not defined by base class, and subclasses
  should define a GetData() and SetData() functions. 
  A albaVMEItem item has also a the following member variables:
 
  <B>m_DataType<\B> to store a "textual description of data".
  <B>m_Id<\B> to store a numeric unique Id (for internal use) see albaVMERoot::GetNextItemId)
  <B>m_URL<\B> internally used to store the name of the file where data is stored

  @sa albaVMEGeneric albaDataVector albaTagArray

  @todo
  - Add a link to the VME object
  - Implement DeepCopy and SmartCopy functions
  - build a test
*/
class ALBA_EXPORT albaVMEItem : public albaReferenceCounted, public albaStorable, public albaEventSender, public albaTimeStamped
{
public:
  ALBA_ID_DEC(VME_ITEM_DATA_MODIFIED) ///< event raised by albaVMEItem to advice DataVector a dataset has been modified

  albaAbstractTypeMacro(albaVMEItem,albaReferenceCounted);

  enum VME_ITEM_IO_ERRORS {ALBA_NO_IO=ALBA_USER_RETURN_VALUE+1};

  /** defined to allow DataVector copy */
  static albaObject *NewObject() {return NULL;}
  /** defined to allow DataVector copy */
  albaVMEItem *NewInstance() {return (albaVMEItem *)NewObjectInstance();}

  virtual void Print(std::ostream& os, const int indent=0) const;

  /** Get the TimeStamp of this dataset*/
  albaTimeStamp GetTimeStamp() const {return m_TimeStamp;}
  /** Set the TimeStamp of this dataset */
  void SetTimeStamp (albaTimeStamp t) {m_TimeStamp=t;}

  /** copy data from another dataset */
  virtual void DeepCopy(albaVMEItem *a);
  /** reference another dataset's internal data */ 
  virtual void ShallowCopy(albaVMEItem *a);

  /** copy large data from another dataset and release it (for albaVMEItem is equals to DeepCopy()).*/
  virtual void DeepCopyVmeLarge(albaVMEItem *a){DeepCopy(a);};

  /** release data from memory */
  virtual void ReleaseData()=0;

  albaVMEItem(const albaVMEItem&){};
  void operator=(const albaVMEItem&){};

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
  int ExtractFileFromArchive(albaString &archive_fullname, albaString &item_file);

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
  albaTagArray *GetTagArray();

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
    a unique Id it use the albaVMERoot::GetNextItemId()*/
  void SetId(albaID id) {m_Id=id;Modified();}

  /**
    Compare two datasets. Two datasets are considered equivalent if they store
    the same type of data, have the same TimeStamp and equivalent TagArray.
    Id and URL are not considered for the comparison.
    To force compare the dataset internal data, use SetGlobalCompareDataOn()*/
  virtual bool Equals(albaVMEItem *item);

  /**
    Set/Get the flag for enabling comparison of dataset internal data in the
    Equals() function. This is a global flag impacting all albaVMEItem instances.*/
  static void SetGlobalCompareDataFlag(bool f);
  static bool* GetGlobalCompareDataFlag();
  static void GlobalCompareDataFlagOn() {SetGlobalCompareDataFlag(true);}
  static void GlobalCompareDataFlagOff() {SetGlobalCompareDataFlag(false);}

  /**
  return Bounds for this item*/
  void GetBounds(double bounds[6]);
  void GetBounds(albaOBB &bounds);
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
  
  /** internally used by albaMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(bool flag);

  /**  UpdateBounds for this data. GetBounds automatically call this function...*/
  virtual void UpdateBounds() = 0;

  /** force updating the data in the items (i.e. usaually read it from disk) */
  virtual void UpdateData() = 0;

  /** Read the data file and update the item's data.*/
  virtual int ReadData(albaString &filename, int resolvedURL = ALBA_OK) = 0;

  /** Set the archive file name.
  The Archive filename is needed to restore archived items when data is stored into a single file mode.*/
  void SetArchiveFileName(albaString &archive) {m_ArchiveFileName = archive;};

  /** Get the archive file name.
  The Archive filename is needed to restore archived items when data is stored into a single file mode.*/
  const char *GetArchiveFileName() {return m_ArchiveFileName.GetCStr();};

  /** Serialize the data into the compressed archive.*/
  virtual bool StoreToArchive(wxZipOutputStream &zip) = 0;

  /** Set temp file name for the item.*/
  void SetTempFileName(albaString &tmp) {m_TmpFileName = tmp;};

  /** Return the temp file name used by the item.*/
  const char *GetTempFileName() {return m_TmpFileName.GetCStr();};

  /** Update Item Id  */
  void UpdateItemId();

protected:
  albaVMEItem(); // to be allocated with New()
  ~albaVMEItem(); // to be deleted with Delete()

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** Check that stored file is valid.*/
  virtual int CheckFile(const char *filename) = 0;

  /** Check that stored string is valid.*/
  virtual int CheckFile(const char *input_string, int input_len) = 0;

  /**
    Restore data stored in this object. To be redefined by specialized classes.
    This method is automatically called by GetData(). 
    Return ALBA_OK if data has been read, ALBA_ERROR in case of I/O errors
    and ALBA_NO_IO in case no I/O is really performed (e.g. data already
    present). */
  virtual int InternalRestoreData()=0;
  
  /**
    Store data stored in this object. To be redefined by specialized classes.
    Return ALBA_OK if data has been read, ALBA_ERROR in case of I/O errors
    and ALBA_NO_IO in case no I/O is really performed (e.g. data already
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
  user but only by albaVME and albaVMEItemArray functions.*/
  //void SetVME(albaVME *vme) {m_VME=vme;}

  albaTagArray * m_TagArray;             ///< meta data attributes attached to this dataset      
  bool          m_DataModified;         ///< true when data has been modified from last storing
  // static bool   m_GlobalCompareDataFlag;///< if true Equals will also compare internal data

  bool          m_ReleaseOldFile;       ///< whether to release the old VTK file when data filename changes (set by albaDataVector::InternalStore() ) 

  int           m_Id;         ///< the id assigned to the dataset/file to be stored in the MSF
  bool          m_Crypting;   ///< this flags specify if encryption should be used when saving data

  std::string  m_DecryptedFileString; ///< String containing the decrypted file in memory
  albaString     m_URL;        ///< the URL of the data file for this dataset
  albaTimeStamp  m_TimeStamp;  ///< time stamp of this dataset
  albaString     m_DataType;   ///< the dataset type expressed as a string 
  albaOBB        m_Bounds;     ///< spatial bounds for this dataset
  albaMTime      m_UpdateTime; ///< store modification timestamp for last update
  bool          m_IsLoadingData; ///< Set when item is loading data to prevent setting DataModified to true

  albaString     m_ArchiveFileName; ///< Filename of the archive if single file mode is enabled
  albaString     m_TmpFileName;///< file name used for local cache
  int           m_IOMode;     ///< IO modality to be used for store/restore data
  const char *  m_InputMemory;///< pointer to memory storing data to be read
  unsigned long m_InputMemorySize;  ///< size of memory storing data to be read
  const char *  m_OutputMemory;     ///< pointer to memory storing the data to be written
  unsigned long m_OutputMemorySize; ///< size of the block of memory where data has been stored
  albaVMEItemAsynchObserver *m_DataObserver; ///< observer used to update the item's data when downloaded
  albaString     m_ChecksumMD5; ///< Store the MD5 checksum for the data associated to the item.
};

/** albaVMEItemAsynchObserver - used by the albaVMEItem to synchronize the asynchronous
loading data from remote storages.

@sa albaVMEItemVTK albaVMEItemScalar
*/
class ALBA_EXPORT albaVMEItemAsynchObserver : public albaObserver
{
public:
  ALBA_ID_DEC(VME_ITEM_DATA_DOWNLOADED); ///< event used to update the albaVMEItem data when download is finished.

  albaVMEItemAsynchObserver();
  virtual	~albaVMEItemAsynchObserver();

  virtual void OnEvent(albaEventBase *alba_event);

  /** Set the item to which send the update event when the asynchronous
  message from the storage comes up.*/
  void SetItem(albaVMEItem *item) {m_Item = item;};

  void SetFileName(albaString &filename) {m_Filename = filename;};

protected:
  albaVMEItem *m_Item; ///< Item to update when the loading of the binary data finish
  albaString   m_Filename; ///< Filename downloaded and to be read.
};
#endif
