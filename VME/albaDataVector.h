/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataVector
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDataVector_h
#define __albaDataVector_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mmuIdFactory.h"
#include "albaTimeMap.h"
#include "albaStorable.h"
#include "albaTimeStamped.h"
#include "albaEventSender.h"
#include "albaVMEItem.h"

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,albaTimeStamp, albaAutoPointer<albaVMEItem>);
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
class ALBA_EXPORT albaDataVector : public albaTimeMap<albaVMEItem>, public albaObserver, public albaStorable, public albaEventSender
{
public:

  albaDataVector();  
  virtual ~albaDataVector();

  albaTypeMacro(albaDataVector,albaTimeMap<albaVMEItem>);

  typedef albaTimeMap<albaVMEItem>::TimeMap::iterator Iterator;
  typedef albaTimeMap<albaVMEItem>::TimeMap DataMap;
  typedef albaTimeMap<albaVMEItem>::mmuTimePair DataPair;

  /** copy another vector referencing the data */
  void ShallowCopy(albaDataVector *a);

  /** 
    Set the single file mode to force the vector to store data
    as a single file. */
  void SetSingleFileMode(bool mode);
  bool GetSingleFileMode() {return m_SingleFileMode;}

  albaString GetArchiveName() {return m_ArchiveName;}

   /**
    Insert an item to the vector trying to append it, anyway the array
    is kept sorted. */
  virtual void AppendItem(albaVMEItem *m);

   /**
    Insert an item to the vector trying to prepend it, anyway the array is kept sorted.
    Item's timestamp must be >=0 */
  virtual void PrependItem(albaVMEItem *m);

  /**
    Set the item for a specified time. If no item with the same time exist
    the item is inserted in the vector. If an item with the same time exist, it's simply
    substituted with the new one. The item is always references and not copied.
    This function also automatically call the UpdateData() member function.*/
  virtual void InsertItem(albaVMEItem *m);

  void OnEvent(albaEventBase *alba_event);

  /** return ID used to store this vector into a file when in SingleFileMode */
  albaID GetVectorID();

  /** update ID used to store this vector into a file when in SingleFileMode */
  void UpdateVectorId();
  
  /** redefined to set the DataModified flag */
  virtual void Modified() {m_DataModified = true; Superclass::Modified();}

  /** return true if any data has been inserted or removed */
  bool IsDataModified() {return m_DataModified;}

  /** return true if data has been saved encrypted */
  bool GetCrypting();
  
  /** internally used by albaMSFWriter/Reader to remember this is a encrypted data */
  void SetCrypting(bool flag);

  /**
  Allow to perform a DeepCopy with data very large.
  Release memory of the input albaVME. */
  void DeepCopyVmeLarge(albaDataVector *o);

  static albaID GetSingleFileDataId();

protected:
 
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  albaString m_ArchiveName;///< Name of the archive if the items are stored in single file mode
  bool  m_SingleFileMode; ///< flag for storing Items as a single file
  albaID m_VectorID;       ///< an Id used to identify the single file when
  bool  m_DataModified;   ///< flag set when a new item is inserted/removed
  bool  m_Crypting;       ///< this flags specify if encryption should be used when saving data
  albaString m_LastBaseURL;///< used to store the base URL of the last storing, to know when need to write again
  bool  m_JustRestored;   ///< flag set when data has just been restored (to be used by InternalStore)
};

#endif
