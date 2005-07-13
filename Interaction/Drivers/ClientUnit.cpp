/////////////////////////////////////////////////////////////////////////////
// Name:        ClientUnit.cpp
//
// project:     MULTIMOD (LAL v2.0)
// Author:      Paolo Quadrani   p.quadrani@cineca.it
// Date:        19/07/2003
/////////////////////////////////////////////////////////////////////////////

#include "mafDefines.h"

#include "ClientUnit.h"
#include "mmuIdFactory.h"
#include "mafEvent.h"


MAF_ID_IMP(ClientUnit::Socket_ID);
MAF_ID_IMP(ClientUnit::RemoteMessage_ID);

BEGIN_EVENT_TABLE(ClientUnit, wxFrame)
  EVT_SOCKET(Socket_ID, ClientUnit::OnSocketEvent)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
ClientUnit::ClientUnit()
//----------------------------------------------------------------------------
{
  // Create the socket
  m_sock = new wxSocketClient();

  // Setup the event handler and subscribe to most events
  m_sock->SetEventHandler(*this, Socket_ID);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(TRUE);

  m_busy = FALSE;
}
//----------------------------------------------------------------------------
ClientUnit::~ClientUnit() 
//----------------------------------------------------------------------------
{
  if(m_sock)
  {
    delete m_sock;
    m_sock = NULL;
  }
}
//----------------------------------------------------------------------------
bool ClientUnit::ConnectClient(wxIPV4address &addr)
//----------------------------------------------------------------------------
{
  wxLogMessage(_("\nTrying to connect (timeout = 10 sec) ...\n"));
  m_sock->Connect(addr, FALSE);
  m_sock->WaitOnConnect(10);

  bool status = m_sock->IsConnected();

  if(status)
    wxLogMessage(_("Succeeded ! Connection established\n"));
  else
  {
    m_sock->Close();
    wxLogMessage(_("Failed ! Unable to connect\n"));
    wxMessageBox(_("Can't connect to the specified host"), _("Alert !"));
  }

  return status;
}
//----------------------------------------------------------------------------
void ClientUnit::SendMessageToServer(wxString cmd)
//----------------------------------------------------------------------------
{
  char *msg1;
  char *msg2;
  size_t len;

  m_busy = TRUE;

  // Here we use ReadMsg and WriteMsg to send messages with
  // a header with size information. Also, the reception is
  // event triggered, so we test input events as well.
  //
  // We need to set no flags here (ReadMsg and WriteMsg are
  // not affected by flags)

  m_sock->SetFlags(wxSOCKET_WAITALL);
  msg1 = (char *)cmd.c_str();
  len  = wxStrlen(msg1) + 1;
  msg2 = new char[len];

  m_sock->WriteMsg(msg1, len);
  wxLogMessage(m_sock->Error() ? _("failed !\n") : _("done\n"));

  delete[] msg2;
  m_busy = FALSE;
}
//----------------------------------------------------------------------------
void ClientUnit::OnSocketEvent(wxSocketEvent& event)
//----------------------------------------------------------------------------
{
  wxString s = _("OnSocketEvent: ");

  wxSocketBase *sock = event.GetSocket();

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT: 
      s.Append(_("wxSOCKET_INPUT\n")); 
      ReadMessageFromServer(sock);
    break;
    case wxSOCKET_LOST       : s.Append(_("wxSOCKET_LOST\n")); break;
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  wxLogMessage(s);
}
//----------------------------------------------------------------------------
void ClientUnit::ReadMessageFromServer(wxSocketBase *sock)
//----------------------------------------------------------------------------
{
#define MAX_MSG_SIZE 10000

  m_busy = TRUE;
  char *msg = new char[MAX_MSG_SIZE];
  wxUint32 len;

  // We don't need to set flags because ReadMsg and WriteMsg
  // are not affected by them anyway.

  // Read the message
  len = sock->ReadMsg(msg, MAX_MSG_SIZE).LastCount();

  //wxString s = msg;
  mafEvent e(this,RemoteMessage_ID,msg);
  InvokeEvent(e);

  delete[] msg;
  m_busy = FALSE;

#undef MAX_MSG_SIZE
}

