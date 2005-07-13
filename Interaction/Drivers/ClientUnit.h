/////////////////////////////////////////////////////////////////////////////
// Name:        ClientUnit.h
//
// Project:     MULTIMOD (LAL v2.0)
// Author:      Paolo Quadrani   p.quadrani@cineca.it
// Date:        19/07/2003
/////////////////////////////////////////////////////////////////////////////

#ifndef __ClientUnit_H__
#define __ClientUnit_H__

#include "mafEventSender.h"

#include <wx/socket.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ClientUnit :
//----------------------------------------------------------------------------
/**
*/
class ClientUnit : public wxFrame , public mafEventSender
{
public:
	ClientUnit();
	~ClientUnit(); 

  MAF_ID_DEC(Socket_ID);
  MAF_ID_DEC(RemoteMessage_ID);

  /** 
  Return the client handler.*/
  wxSocketClient *GetConnectionHandler() {return m_sock;};

  /** 
  Connect the client to the server at the given IP address.*/
  bool ConnectClient(wxIPV4address &addr);

  /** 
  Send the message to the server.*/
  void SendMessageToServer(wxString cmd);

  /** 
  Function called when the server sends a message.*/
  void OnSocketEvent(wxSocketEvent& event);

  bool IsBusy() {return m_busy;};

protected:
  void ReadMessageFromServer(wxSocketBase *sock);

  wxSocketClient   *m_sock;
  bool              m_busy;

  DECLARE_EVENT_TABLE()
};
#endif
