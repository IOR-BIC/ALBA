/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafDeviceClientMAF.cpp,v $
Language:  C++
Date:      $Date: 2009-05-25 14:48:12 $
Version:   $Revision: 1.1.2.1 $
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

#include "mafDeviceClientMAF.h"
#include "mmuIdFactory.h"
#include "mafGUI.h"

#include "mafEventSender.h"

mafCxxTypeMacro(mafDeviceClientMAF)

//------------------------------------------------------------------------------
mafDeviceClientMAF::mafDeviceClientMAF()
//------------------------------------------------------------------------------
{
  SetThreaded(0);
  m_Client = new ClientUnit();
  m_Client->SetListener(this);
  m_ServerHost = wxGetHostName().c_str();
  m_PortNumber = 50000;
  m_Connected  = false;
}

//------------------------------------------------------------------------------
mafDeviceClientMAF::~mafDeviceClientMAF()
//------------------------------------------------------------------------------
{
  cppDEL(m_Client);
}

//---------------------------------------------------------------------------
void mafDeviceClientMAF::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  //m_Gui->Divider(1);

  m_Gui->String(ID_SERVER_HOSTNAME,"server",&m_ServerHost);
  m_Gui->Integer(ID_PORT_NUMBER,"port",&m_PortNumber, 1, 100000);

	m_Gui->Divider();
}

//----------------------------------------------------------------------------
void mafDeviceClientMAF::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetSender() == m_Client) 
  {
    if (maf_event->GetId() == ClientUnit::RemoteMessage_ID) 
    {
      InvokeEvent(maf_event,MCH_INPUT);
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
int mafDeviceClientMAF::InternalInitialize()
//------------------------------------------------------------------------------
{
  if(Superclass::InternalInitialize()!=MAF_OK)
    return MAF_ERROR;

  m_AutoStart = 0;

  wxString hn = m_ServerHost.GetCStr();
  wxIPV4address server_address;
  server_address.Service(m_PortNumber);
  server_address.Hostname(hn);
  m_Connected = (ConnectClient(server_address) == MAF_OK);
  mafEvent e(this,COLLABORATE_ENABLE,m_Connected);
  InvokeEvent(&e);
  return m_Connected ? MAF_OK : MAF_ERROR;
}
//------------------------------------------------------------------------------
void mafDeviceClientMAF::InternalShutdown()
//------------------------------------------------------------------------------
{
  m_Connected = !(DisconnectClient() == MAF_OK);
  mafEvent e(this,COLLABORATE_ENABLE,m_Connected);
  InvokeEvent(&e);

  Superclass::InternalShutdown();
}
//------------------------------------------------------------------------------
int mafDeviceClientMAF::ConnectClient(wxIPV4address &addr)
//------------------------------------------------------------------------------
{
  if (m_Client->ConnectClient(addr)) 
  {
    return MAF_OK;
  }
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
int mafDeviceClientMAF::DisconnectClient()
//------------------------------------------------------------------------------
{
  if (m_Client->DisconnectClient()) 
  {
    return MAF_OK;
  }
  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void mafDeviceClientMAF::SendMessageToServer(mafString &cmd)
//------------------------------------------------------------------------------
{
  if (m_Connected)
  {
    m_Client->SendMessageToServer(cmd);
  }
}
