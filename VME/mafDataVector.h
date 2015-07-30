/*=========================================================================

 Program: MAF2
 Module: mafDataVector
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafDataVector_h
#define __mafDataVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmuIdFactory.h"
#include "mafTimeMap.h"
#include "mafStorable.h"
#include "mafTimeStamped.h"
#include "mafEventSender.h"
#include "mafVMEItem.h"

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_MAP(MAF_EXPORT,mafTimeStamp, mafAutoPointer<mafVMEItem>);
#endif

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------


/** a dynamic associative sorted array of datasets indexed by their "timestamp".
  This class is though to store generic VMEItems (i.e. generic data), and to 
  provide basic interface for 
  @todo
  - implement Store/Restore
  - implement deletion of single files in "SingleFileMode"
  - reimplement IsDataModified() mechanism
*/
class MAF_EXPORT mafDataVector : public mafTimeMap<mafVMEItem>, public mafObserver, public mafStorable, public mafEventSender
{
public:

  mafDataVector();  
  virtual ~mafDataVector();

  mafTypeMacro(mafDataVector,mafTimeMap<mafVMEItem>);

  typedef mafTimeMap<mafVMEItem>::TimeMap::iterator Iterator;
  typedef mafTimeMap<mafVMEItem>::TimeMap DataMap;
  typedef mafTimeMap<mafVMEItem>::mmuTimePair DataPair;

  /** copy another vector referencing the data */
  void ShallowCopy(mafDataVector *a);

  /** 
    Set the single file mode to force the vector to store data
    as a single file. */
  void SetSingleFileMode(bool mode);
  bool GetSingleFileMode() {return m_SingleFileMode;}

  mafString GetArchiveName() {return m_ArchiveName;}

   /**
    Insert an item to the vector trying to append it, anyway the array
    is kept sorted. */
  virtual void AppendItem(mafVMEItem *m);

   /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  virtual void PrependItem(mafVMEItem *m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(mafVMEItem *m);

  void OnEvent(mafEventBase *maf_event);

  /** return ID used to store this vector into a file when in SingleFileMode */
  mafID GetVectorID();

  /** update ID used to store this vector into a file when in SingleFileMode */
  void UpdateVectorId();
  
  /** redefined to set the DataModified flag */
  virtual void Modified() {m_DataModified = true; Superclass::Modified();}

  /** return true if any data has been inserted or removed */
  bool IsDataModified() {return m_DataModified;}

  /** return true if data has been saved encrypted */
  bool GetCrypting();
  
  /** internally used by mafMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(bool flag);

  /**
  Allow to perform a DeepCopy with data very large.
  Release memory of the input mafNode. */
  void DeepCopyVmeLarge(mafDataVector *o);

  static mafID GetSingleFileDataId();

protected:
 
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mafString m_ArchiveName;///< Name of the archive if the items are stored in single file mode
  bool  m_SingleFileMode; ///< flag for storing Items as a single file
  mafID m_VectorID;       ///< an Id used to identify the single file when
  bool  m_DataModified;   ///< flag set when a new item is inserted/removed
  bool  m_Crypting;       ///< this flags specify if encryption should be used when saving data
  mafString m_LastBaseURL;///< used to store the base URL of the last storing, to know when need to write again
  bool  m_JustRestored;   ///< flag set when data has just been restored (to be used by InternalStore)
};

#endif
