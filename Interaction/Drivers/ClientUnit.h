/*=========================================================================

 Program: MAF2
 Module: ClientUnit
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

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

  /** Close the connection with the server.*/
  bool DisconnectClient();

  /** 
  Send the message to the server.*/
  void SendMessageToServer(mafString &cmd);

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
