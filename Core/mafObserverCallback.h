/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserverCallback.h,v $
  Language:  C++
  Date:      $Date: 2004-11-09 06:43:10 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafObserverCallback_h
#define __mafObserverCallback_h

#include "mafObserver.h"

//------------------------------------------------------------------------------
// mafObserverCallback
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafObserverCallback is a class implementing an "observer" that launches a callback
  function. It's useful to attach as observers objects that can not inherit from mafObserver.
  When an event is rised the callback function is called passing as argument some event content 
  plus event source Data and this class ClientData, that can be set to any value. Usually ClientData
  is used to store the (self) pointer of the class to whom the callback function belongs.
  @sa mafEventSource mafObserver
*/
class mafObserverCallback: public mafObserver
{
public:
  mafObserverCallback();
  virtual ~mafObserverCallback();
  mafObserverCallback(const mafObserverCallback& c) {}

  mafTypeMacro(mafObserverCallback,mafObserver);

  /** Set function callback to be called by this observer */
  void SetCallback(void (*f)(void *sender, mafID eid, 
                             void *clientdata, void *calldata))
    {m_Callback = f;}
   
  /** set client data to be passed to callback function */
  void SetClientData(void *cd) {this->m_ClientData = cd;}
    
  /** return client data passed to callback function */  
  void* GetClientData() {return this->m_ClientData;}
       
  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e);

protected:
  void (*m_Callback)(void *, mafID, void *, void *);  
  void *m_ClientData;
};

#endif /* __mafObserverCallback_h */
