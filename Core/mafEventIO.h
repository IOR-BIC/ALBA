/*=========================================================================

 Program: MAF2
 Module: mafEventIO
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafEventIO_h
#define __mafEventIO_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafEventBase.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafStorage;
class mafNode;
class mafRoot;
//------------------------------------------------------------------------------
// mafEventIO
//------------------------------------------------------------------------------
/** Implementation of event used to exchange info about tree I/O.
  This event is thought to allow exchanging information regarding
  I/O of the tree.

  @sa mafEventBase mafVME mafVMEItem mafStorage mafSubject mafObserver
*/
class MAF_EXPORT mafEventIO: public mafEventBase
{
public:
  mafEventIO(void *sender=NULL,mafID id=ID_NO_EVENT,mafID item_id=-1,void *data=NULL, mafID channel=MCH_UP);
  virtual ~mafEventIO();

  mafTypeMacro(mafEventIO,mafEventBase);
 
  /** copy constructor, this makes a copy of the event */
  virtual void DeepCopy(const mafEventIO *c);

  /** set item Id stored into this event */
  void SetItemId(mafID id);

  /** return item Id of this event */
  mafID GetItemId();

  /** set the storage reference */
  void SetStorage(mafStorage *storage);
  mafStorage *GetStorage();

  void SetRoot(mafNode *root);
  
  /** return the pointer to the tree root node */
  mafNode *GetRoot();

protected:
  mafID           m_ItemId;
  mafStorage*     m_Storage;
  mafNode*        m_Root;
};

#endif /* __mafEventIO_h */
