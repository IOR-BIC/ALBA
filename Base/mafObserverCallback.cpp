/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserverCallback.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-04 10:42:14 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
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
  m_Callback    = NULL;
  m_ClientData  = NULL;
}

//------------------------------------------------------------------------------
mafObserverCallback::~mafObserverCallback()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafObserverCallback::OnEvent(mafEventBase *e)
//------------------------------------------------------------------------------
{
  if (m_Callback)
  {
    m_Callback(e->GetSender(), e->GetId(), this->GetClientData(), e->GetSource()->GetData());
  }
}

//------------------------------------------------------------------------------
void mafObserverCallback::SetCallback(void (*f)(void *sender, mafID eid, void *clientdata, void *calldata))
//------------------------------------------------------------------------------
{
  m_Callback = f;
}

//------------------------------------------------------------------------------
void mafObserverCallback::SetClientData(void *cd)
//------------------------------------------------------------------------------
{
  this->m_ClientData = cd;
}

//------------------------------------------------------------------------------
void* mafObserverCallback::GetClientData()
//------------------------------------------------------------------------------
{
  return this->m_ClientData;
}
