/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserverCallback.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-09 06:43:10 $
  Version:   $Revision: 1.3 $
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
