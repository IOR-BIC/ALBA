/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventIO.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 18:10:11 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
  mafEventIO(const mafEventIO& c);

  /** set item Id stored into this event */
  void SetItemId(mafID id);

  /** return item Id of this event */
  mafID GetItemId();

  /** set the storage reference */
  void SetStorage(mafStorage *storage);
  mafStorage *GetStorage();

  void SetRoot(mafNode *root);
  mafNode *GetRoot();

protected:
  mafID           m_ItemId;
  mafStorage*     m_Storage;
  mafRoot*        m_Root;
};

#endif /* __mafEventIO_h */
