/*=========================================================================

 Program: MAF2
 Module: mafObserver
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafObserver_h
#define __mafObserver_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafEvent.h"

//------------------------------------------------------------------------------
// mafObserver
//------------------------------------------------------------------------------
/** Interface implementing the Observer of the Subject/Observer design pattern.
  mafObserver is an abstract class implementing the "observer" in the Subject/Observer 
  design pattern. Objective of this object is to provide an interface for listening to events
  issued by subjects.
  An observer must be registered to a subject to create the communication channel 
  between the two. An observer can "observe" many subjects at the same time.
  @sa mafEventBroadcaster mafObserverCallback
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
