/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserver.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:27:43 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
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
/** Interface implementing the Observer of the Subject/Observer design pattern.
  mafObserver is an abstract class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to provide an interface for listening to events
  issued by subjects.
  An observer must be registered to a subject to create the communication channel 
  between the two. An observer can "observe" many subjects at the same time.
  @sa mafEventSource mafEventListener mafObserverCallback
*/
class MAF_EXPORT mafObserver
{
public:
  mafObserver() {}
  virtual ~mafObserver() {}

  /** process the events sent by subjects */
  virtual void OnEvent(mafEventBase *e) = 0;
};

#endif /* __mafObserver_h */
