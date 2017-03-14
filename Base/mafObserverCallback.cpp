/*=========================================================================

 Program: MAF2
 Module: mafObserverCallback
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include "mafObserverCallback.h"
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
    m_Callback(e->GetSender(), e->GetId(), this->GetClientData());
  }
}

//------------------------------------------------------------------------------
void mafObserverCallback::SetCallback(void (*f)(void *sender, mafID eid, void *clientdata))
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
