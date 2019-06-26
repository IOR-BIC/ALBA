/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventIO
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaEventIO_h
#define __albaEventIO_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaEventBase.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class albaStorage;
class albaVME;
class albaRoot;
//------------------------------------------------------------------------------
// albaEventIO
//------------------------------------------------------------------------------
/** Implementation of event used to exchange info about tree I/O.
  This event is thought to allow exchanging information regarding
  I/O of the tree.

  @sa albaEventBase albaVME albaVMEItem albaStorage albaSubject albaObserver
*/
class ALBA_EXPORT albaEventIO: public albaEventBase
{
public:
  albaEventIO(void *sender=NULL,albaID id=ID_NO_EVENT,albaID item_id=-1,void *data=NULL, albaID channel=MCH_UP);
  virtual ~albaEventIO();

  albaTypeMacro(albaEventIO,albaEventBase);
 
  /** copy constructor, this makes a copy of the event */
  virtual void DeepCopy(const albaEventIO *c);

  /** set item Id stored into this event */
  void SetItemId(albaID id);

  /** return item Id of this event */
  albaID GetItemId();

  /** set the storage reference */
  void SetStorage(albaStorage *storage);
  albaStorage *GetStorage();

  void SetRoot(albaVME *root);
  
  /** return the pointer to the tree root node */
  albaVME *GetRoot();

protected:
  albaID           m_ItemId;
  albaStorage*     m_Storage;
  albaVME*        m_Root;
};

#endif /* __albaEventIO_h */
