/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEStorage
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEStorage_h__
#define __albaVMEStorage_h__

#include "albaXMLStorage.h"
#include "albaObserver.h"
#include "albaEventSender.h"
#include "albaUtility.h"
#include "albaStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMERoot;

/** utility class representing the MSF document. */
class mmuMSFDocument : public albaUtility, public albaStorable
{
public:
  mmuMSFDocument(albaVMERoot *root=NULL):m_Root(root) {}
  virtual ~mmuMSFDocument() {}
  virtual int InternalStore(albaStorageElement *node);
  virtual int InternalRestore(albaStorageElement *node);
protected:
  albaVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa albaXMLStorage
  @todo
    - 
*/  
class ALBA_EXPORT albaVMEStorage: public albaXMLStorage, public albaObserver, public albaEventSender
{
public:
  ALBA_ID_DEC(MSF_FILENAME_CHANGED);
  albaTypeMacro(albaVMEStorage,albaXMLStorage)

  albaVMEStorage();
  virtual ~albaVMEStorage();
  
  /** return the root node attached to this tree */
  albaVMERoot *GetRoot();

  /** 
    Set the root to this Storage. This is usually not necessary
    since storage creates the root on its own */
  void SetRoot (albaVMERoot *root);

  virtual void SetURL(const char *name);

  /** process events coming from tree */
  virtual void OnEvent(albaEventBase *e);

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

  albaVMERoot *m_Root; ///< the VME root node
};
#endif // _albaVMEStorage_h_
