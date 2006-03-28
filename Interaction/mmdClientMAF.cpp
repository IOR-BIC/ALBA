/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmdClientMAF.cpp,v $
Language:  C++
Date:      $Date: 2006-03-28 16:56:32 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmdClientMAF.h"
#include "mmuIdFactory.h"
#include "mmgGui.h"
#include "ClientUnit.h"

#include "mafEventSender.h"

mafCxxTypeMacro(mmdClientMAF)

//------------------------------------------------------------------------------
mmdClientMAF::mmdClientMAF()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_Client = new ClientUnit();
  m_Client->SetListener(this);
  m_ServerHost = wxGetHostName().c_str();
  m_PortNumber = 50000;
}

//------------------------------------------------------------------------------
mmdClientMAF::~mmdClientMAF()
//------------------------------------------------------------------------------
{
  cppDEL(m_Client);
}

//---------------------------------------------------------------------------
void mmdClientMAF::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  m_Gui->Divider(1);

  m_Gui->String(ID_SERVER_HOSTNAME,"server",&m_ServerHost);
  m_Gui->Integer(ID_PORT_NUMBER,"port",&m_PortNumber, 1, 100000);
}

//----------------------------------------------------------------------------
void mmdClientMAF::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetSender() == m_Client) 
  {
    if (maf_event->GetId() == ClientUnit::RemoteMessage_ID) 
    {
      AsyncInvokeEvent(maf_event,MCH_INPUT);
    }
  }
  else if (maf_event->GetSender() == m_Gui) 
  {
    switch(maf_event->GetId()) 
    {
      case ID_SERVER_HOSTNAME:
      case ID_PORT_NUMBER:
    	break;
      default:
        Superclass::OnEvent(maf_event);
    }
  }
  else
    Superclass::OnEvent(maf_event);
}

//------------------------------------------------------------------------------
int mmdClientMAF::InternalInitialize()
//------------------------------------------------------------------------------
{
  if(Superclass::InternalInitialize()!=MAF_OK)
    return MAF_ERROR;

  m_AutoStart = 0;

  wxString hn = m_ServerHost.GetCStr();
  wxIPV4address server_address;
  server_address.Service(m_PortNumber);
  server_address.Hostname(hn);
  if (m_Client->ConnectClient(server_address)) 
  {
    return MAF_OK;
  }
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void mmdClientMAF::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_Client->DisconnectClient();
  return;
}
