/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserver.h,v $
  Language:  C++
  Date:      $Date: 2004-12-04 09:27:59 $
  Version:   $Revision: 1.4 $
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
class MAF_EXPORT mafObserver: public mafObject
{
public:
  mafObserver();
  virtual ~mafObserver();

  mafAbstractTypeMacro(mafObserver,mafObject);

  mafObserver(const mafObserver& c) {}
  //void operator=(const mafObserver&) {}

  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e) = 0;
  
protected:

private:
  
};

#endif /* __mafObserver_h */
