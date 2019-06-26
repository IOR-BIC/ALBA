/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventBase
 Authors: Marco Petrone, Crimi Gianluigi
 
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


#include "albaEventBase.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaEventBase)
//------------------------------------------------------------------------------

// bool albaEventBase::m_LogVerbose = false;

//------------------------------------------------------------------------------
albaEventBase::albaEventBase(void *sender, albaID id, void *data, albaID channel):
m_Sender(sender),m_Data(data),m_Id(id),m_Channel(channel)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
albaEventBase::~albaEventBase()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void albaEventBase::DeepCopy(const albaEventBase *alba_event)
//------------------------------------------------------------------------------
{
  m_Sender    = alba_event->m_Sender;
  m_Data      = alba_event->m_Data;
  m_Id        = alba_event->m_Id;
}

//------------------------------------------------------------------------------
albaEventBase::albaEventBase(const albaEventBase& c)
//------------------------------------------------------------------------------
{
  DeepCopy(&c);
}
//------------------------------------------------------------------------------
void albaEventBase::SetSender(void *sender)
//------------------------------------------------------------------------------
{
  m_Sender = sender;
}
//------------------------------------------------------------------------------
void *albaEventBase::GetSender()
//------------------------------------------------------------------------------
{
  return m_Sender;
}

//------------------------------------------------------------------------------
void albaEventBase::SetData(void *calldata)
//------------------------------------------------------------------------------
{
  m_Data = calldata;
}

//------------------------------------------------------------------------------
void *albaEventBase::GetData()
//------------------------------------------------------------------------------
{
  return m_Data;
}

//------------------------------------------------------------------------------
void albaEventBase::SetId(albaID id)
//------------------------------------------------------------------------------
{
  m_Id = id;
}
//------------------------------------------------------------------------------
albaID albaEventBase::GetId()
//------------------------------------------------------------------------------
{
  return m_Id;
}


//------------------------------------------------------------------------------
albaID albaEventBase::GetChannel()
//------------------------------------------------------------------------------
{
  return m_Channel;
}
  
//------------------------------------------------------------------------------
void albaEventBase::SetChannel(albaID channel)
//------------------------------------------------------------------------------
{
  m_Channel=channel;
}
