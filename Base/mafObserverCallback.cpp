/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafObserverCallback.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-26 07:16:02 $
  Version:   $Revision: 1.3 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



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
