/*=========================================================================

 Program: MAF2
 Module: mafEventBase
 Authors: Marco Petrone, Crimi Gianluigi
 
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


#include "mafEventBase.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventBase)
//------------------------------------------------------------------------------

// bool mafEventBase::m_LogVerbose = false;

//------------------------------------------------------------------------------
mafEventBase::mafEventBase(void *sender, mafID id, void *data, mafID channel):
m_Sender(sender),m_Data(data),m_Id(id),m_Channel(channel)
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
mafEventBase::~mafEventBase()
//------------------------------------------------------------------------------
{
}

//------------------------------------------------------------------------------
void mafEventBase::DeepCopy(const mafEventBase *maf_event)
//------------------------------------------------------------------------------
{
  m_Sender    = maf_event->m_Sender;
  m_Data      = maf_event->m_Data;
  m_Id        = maf_event->m_Id;
}

//------------------------------------------------------------------------------
mafEventBase::mafEventBase(const mafEventBase& c)
//------------------------------------------------------------------------------
{
  DeepCopy(&c);
}
//------------------------------------------------------------------------------
void mafEventBase::SetSender(void *sender)
//------------------------------------------------------------------------------
{
  m_Sender = sender;
}
//------------------------------------------------------------------------------
void *mafEventBase::GetSender()
//------------------------------------------------------------------------------
{
  return m_Sender;
}

//------------------------------------------------------------------------------
void mafEventBase::SetData(void *calldata)
//------------------------------------------------------------------------------
{
  m_Data = calldata;
}

//------------------------------------------------------------------------------
void *mafEventBase::GetData()
//------------------------------------------------------------------------------
{
  return m_Data;
}

//------------------------------------------------------------------------------
void mafEventBase::SetId(mafID id)
//------------------------------------------------------------------------------
{
  m_Id = id;
}
//------------------------------------------------------------------------------
mafID mafEventBase::GetId()
//------------------------------------------------------------------------------
{
  return m_Id;
}


//------------------------------------------------------------------------------
mafID mafEventBase::GetChannel()
//------------------------------------------------------------------------------
{
  return m_Channel;
}
  
//------------------------------------------------------------------------------
void mafEventBase::SetChannel(mafID channel)
//------------------------------------------------------------------------------
{
  m_Channel=channel;
}
