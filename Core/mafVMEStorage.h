/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEStorage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-26 07:18:37 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEStorage_h__
#define __mafVMEStorage_h__

#include "mafXMLStorage.h"
#include "mafObserver.h"
#include "mafEventSender.h"
#include "mmuUtility.h"
#include "mafStorable.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;

/** utility class representing the MSF document. */
class mmuMSFDocument : public mmuUtility, public mafStorable
{
public:
  mmuMSFDocument(mafVMERoot *root=NULL):m_Root(root) {}
  virtual ~mmuMSFDocument() {}
  virtual int InternalStore(mafStorageElement *node);
  virtual int InternalRestore(mafStorageElement *node);
protected:
  mafVMERoot *m_Root;
};

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class mafVMEStorage: public mafXMLStorage, public mafObserver, public mafEventSender
{
public:
  

  mafTypeMacro(mafVMEStorage,mafXMLStorage)

  mafVMEStorage();
  virtual ~mafVMEStorage();
  
  /** return the root node attached to this tree */
  mafVMERoot *GetRoot();

  /** process events coming from tree */
  virtual void OnEvent(mafEventBase *e);

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

  mafVMERoot *m_Root; ///< the VME root node

private:
  
  /** avoid external objects to set the root */
  void SetRoot (mafStorable *root);
};
#endif // _mafVMEStorage_h_
