/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaObserverCallback
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "albaObserverCallback.h"
#include "albaEventBase.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaObserverCallback)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaObserverCallback::albaObserverCallback()
//------------------------------------------------------------------------------
{
  m_Callback    = NULL;
  m_ClientData  = NULL;
}

//------------------------------------------------------------------------------
albaObserverCallback::~albaObserverCallback()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaObserverCallback::OnEvent(albaEventBase *e)
//------------------------------------------------------------------------------
{
  if (m_Callback)
  {
    m_Callback(e->GetSender(), e->GetId(), this->GetClientData());
  }
}

//------------------------------------------------------------------------------
void albaObserverCallback::SetCallback(void (*f)(void *sender, albaID eid, void *clientdata))
//------------------------------------------------------------------------------
{
  m_Callback = f;
}

//------------------------------------------------------------------------------
void albaObserverCallback::SetClientData(void *cd)
//------------------------------------------------------------------------------
{
  this->m_ClientData = cd;
}

//------------------------------------------------------------------------------
void* albaObserverCallback::GetClientData()
//------------------------------------------------------------------------------
{
  return this->m_ClientData;
}
