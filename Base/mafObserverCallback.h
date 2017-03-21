/*=========================================================================

 Program: MAF2
 Module: mafObserverCallback
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafObserverCallback_h
#define __mafObserverCallback_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafObserver.h"

//------------------------------------------------------------------------------
// mafObserverCallback
//------------------------------------------------------------------------------
/** Concrete implementation of Observer calling a callback function.
  mafObserverCallback is a class implementing an "observer" that launches a callback
  function. It's useful for attaching as observers objects that do not inherit from mafObserver.
  When an event is rise the callback function is called passing as argument some event content 
  plus event source Data and this class ClientData, that can be set to any value. Usually ClientData
  is used to store the (self) pointer of the class to whom the callback function belongs.
  @sa mafEventBroadcaster mafObserver
  @todo 
  - create a test
*/
class MAF_EXPORT mafObserverCallback:public mafObject, public mafObserver
{
public:
  mafObserverCallback();
  virtual ~mafObserverCallback();

  mafTypeMacro(mafObserverCallback,mafObject);

  /** Set function callback to be called by this observer */
  void SetCallback(void (*f)(void *sender, mafID eid, void *clientdata));
   
  /** set client data to be passed to callback function */
  void SetClientData(void *cd);
    
  /** return client data passed to callback function */  
  void* GetClientData();
       
  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e);

protected:
  void (*m_Callback)(void *, mafID, void *);  
  void *m_ClientData;
};

#endif /* __mafObserverCallback_h */
