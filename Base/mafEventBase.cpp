/*=========================================================================

 Program: MAF2
 Module: mafEventBase
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


#include "mafEventBase.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventBase)
//------------------------------------------------------------------------------

// bool mafEventBase::m_LogVerbose = false;

//------------------------------------------------------------------------------
mafEventBase::mafEventBase(void *sender, mafID id, void *data, mafID channel):
m_Sender(sender),m_Data(data),m_Id(id),m_Channel(channel),m_SkipFlag(false)
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
  m_SkipFlag  = maf_event->m_SkipFlag;
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
  bool *verbose = GetLogVerbose();
  if (m_Id != UPDATE_UI && (*verbose) )
  {
    mafString sender_type = "Received Event. Sender: ";
    try
    {
      mafObject *obj = (mafObject *)m_Sender;
      sender_type << typeid(*obj).name();
    }
    catch (...)
    {
      sender_type = "not mafObject (";
      sender_type << (long)m_Sender;
      sender_type << ")";
    }
    mafString id_name = mafIdString(m_Id).c_str();
    mafString msg = sender_type + "  ID: " + id_name;
    mafLogMessage(msg);
  }
  return m_Id;
}

//------------------------------------------------------------------------------
bool mafEventBase::GetSkipFlag()
//------------------------------------------------------------------------------
{
  return m_SkipFlag;
}

//------------------------------------------------------------------------------
void mafEventBase::SetSkipFlag(bool flag)
//------------------------------------------------------------------------------
{
  m_SkipFlag=flag;
}

//------------------------------------------------------------------------------
void mafEventBase::SkipNext()
//------------------------------------------------------------------------------
{
  m_SkipFlag=true;
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
//------------------------------------------------------------------------------
void mafEventBase::SetLogVerbose(bool verbose /* = true */)
//------------------------------------------------------------------------------
{
  bool *help = GetLogVerbose();

  *help = verbose;
}
//------------------------------------------------------------------------------
bool* mafEventBase::GetLogVerbose()
//------------------------------------------------------------------------------
{
  static bool logVerbose = false;
  return &logVerbose;
}
