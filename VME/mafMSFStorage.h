/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFStorage.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:16:06 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafMSFStorage_h__
#define __mafMSFStorage_h__

#include "mafXMLStorage.h"
#include "mafObserver.h"
#include "mafEventSender.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMERoot;

/** A storage class for MSF local files.
  This is a concrete implementation of storage object for storing MSF local files.
  @sa mafXMLStorage
  @todo
    - 
*/  
class mafMSFStorage: public mafXMLStorage, public mafObserver, public mafEventSender
{
public:
  mafTypeMacro(mafMSFStorage,mafXMLStorage)

  mafMSFStorage();
  virtual ~mafMSFStorage();
  
  /** return the root object restored */
  mafVMERoot *GetRoot();

  /** process events coming from tree */
  virtual void OnEvent(mafEventBase *e);

protected:  
  /** Do not allow changing the file type from external objects. */
  void SetFileType(const char *filetype) {Superclass::SetFileType(filetype);}
  
  /** Do not allow changing the file version from external objects. */
  void SetVersion(const char *version) {Superclass::SetVersion(version);}

  mafVMERoot *m_VMERoot; ///< the VME root node

private:
  
  /** avoid external objects to set the root */
  void SetRoot (mafStorable *root);
};
#endif // _mafMSFStorage_h_
