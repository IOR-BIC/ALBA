/*=========================================================================

 Program: MAF2
 Module: mafDeviceClientMAF
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafDeviceClientMAF_h
#define __mafDeviceClientMAF_h

#include "mafDevice.h"
#include "ClientUnit.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** Device implementing interface for PLASTIC hub.
  mafDeviceClientMAF is a class providing interface for MAFServer (socket communication).
  @sa mafDevice
*/

class MAF_EXPORT mafDeviceClientMAF : public mafDevice
{
public:
  mafTypeMacro(mafDeviceClientMAF, mafDevice); 

  virtual void OnEvent(mafEventBase *maf_event);

  //----------------------------------------------------------------------------
  //    GUI Constants
  //----------------------------------------------------------------------------
  enum MAF_CLIENT_WIDGET_ID
  {
  	ID_SERVER_HOSTNAME = Superclass::ID_LAST,
    ID_PORT_NUMBER,
    ID_LAST
  };

  /** internal function to create device GUI for settings */
  virtual void CreateGui();

  /** Connect the client to the specified address and return MAF_OK on success, otherwise MAF_ERROR is returned.*/
  int ConnectClient(wxIPV4address &addr);

  /** Disconnect client form remote connection and return MAF_OK on success otherwise MAF_ERROR is returned*/
  int DisconnectClient();

  /** Send the command to the server.*/
  void SendMessageToServer(mafString &cmd);

  /** Return connection status flag.*/
  bool IsConnected() {return m_Connected;};

  /** Return client busy status.*/
  bool IsBusy() {return m_Client->IsBusy();};

protected:
  mafDeviceClientMAF();
  virtual ~mafDeviceClientMAF();

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  ClientUnit *m_Client;
  mafString   m_ServerHost;
  int         m_PortNumber;
  bool        m_Connected;
private:
  mafDeviceClientMAF(const mafDeviceClientMAF&);  // Not implemented.
  void operator=(const mafDeviceClientMAF&);  // Not implemented.
};
#endif
