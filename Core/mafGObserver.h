/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGObserver.h,v $
  Language:  C++
  Date:      $Date: 2004-11-04 20:59:28 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGObserver_h
#define __mafGObserver_h

#include "mafObject.h"

//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafEventBase;

//------------------------------------------------------------------------------
// mafGObserver
//------------------------------------------------------------------------------
/** Implementation of the Subject/Observer design pattern.
  mafGObserver is a class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to listen to events rised by subjects.
  An observer must be registered to a subject to create the communication channel 
  between the two. An observer can "observe" many subjects.
  @sa mafGEventSource mafEventListener mafGObserverCallback
*/
class mafGObserver: public mafObject
{
public:
  mafGObserver();
  virtual ~mafGObserver();

  mafTypeMacro(mafGObserver,mafObject);

  mafGObserver(const mafGObserver& c) {}
  //void operator=(const mafGObserver&) {}

  /** process the events sent by subjects */
  virtual void ProcessEvent(mafEventBase *e);
  
protected:

private:
  
};

#endif /* __mafGObserver_h */
