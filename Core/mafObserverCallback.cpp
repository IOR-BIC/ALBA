/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserverCallback.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-04 12:18:54 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafObserverCallback.h"
#include "mafEventSource.h"
#include "mafEventBase.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafObserverCallback)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafObserverCallback::mafObserverCallback()
//------------------------------------------------------------------------------
{
  Callback    = NULL;
  ClientData  = NULL;
}

//------------------------------------------------------------------------------
mafObserverCallback::~mafObserverCallback()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafObserverCallback::ProcessEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (Callback)
  {
    Callback(e->GetSender(), e->GetId(), this->GetClientData(), e->GetSource()->GetData());
  }
}
