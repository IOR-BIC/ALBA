/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserver.h,v $
  Language:  C++
  Date:      $Date: 2004-11-04 12:18:53 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafObserver_h
#define __mafObserver_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafEventBase;

//------------------------------------------------------------------------------
// mafObserver
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafObserver is a class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to listen to events rised by subjects.
  An observer must be registered to a subject to create the communication channel 
  between the two. An observer can "observe" many subjects.
  @sa mafEventSource mafEventListener mafObserverCallback
*/
class mafObserver: public mafObject
{
public:
  mafObserver();
  virtual ~mafObserver();

  mafTypeMacro(mafObserver,mafObject);

  mafObserver(const mafObserver& c) {}
  //void operator=(const mafObserver&) {}

  /** process the events sent by subjects */
  virtual void ProcessEvent(mafEventBase *e);
  
protected:

private:
  
};

#endif /* __mafObserver_h */
