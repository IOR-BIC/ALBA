/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventBase.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-17 11:48:53 $
  Version:   $Revision: 1.5.22.1 $
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


#include "mafEventBase.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafEventBase)
//------------------------------------------------------------------------------

bool mafEventBase::m_LogVerbose = false;

//------------------------------------------------------------------------------
mafEventBase::mafEventBase(void *sender, mafID id, void *data, mafID channel):
m_Sender(sender),m_Source(NULL),m_Data(data),m_Id(id),m_Channel(channel),m_SkipFlag(false)
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
  m_Source    = maf_event->m_Source;
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
void mafEventBase::SetSource(mafEventSource *src)
//------------------------------------------------------------------------------
{
  m_Source=src;
}

//------------------------------------------------------------------------------
mafEventSource *mafEventBase::GetSource()
//------------------------------------------------------------------------------
{
  return m_Source;
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
  if (m_Id != UPDATE_UI && m_LogVerbose)
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
