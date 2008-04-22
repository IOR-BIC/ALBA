/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmdClientMAF.h,v $
Language:  C++
Date:      $Date: 2008-04-22 16:58:11 $
Version:   $Revision: 1.3 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmdClientMAF_h
#define __mmdClientMAF_h

#include "mafDevice.h"
#include "ClientUnit.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------


/** Device implementing interface for PLASTIC hub.
  mmdClientMAF is a class providing interface for MAFServer (socket communication).
  @sa mafDevice
*/

class mmdClientMAF : public mafDevice
{
public:
  mafTypeMacro(mmdClientMAF, mafDevice); 

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
  mmdClientMAF();
  virtual ~mmdClientMAF();

  virtual int InternalInitialize();
  virtual void InternalShutdown();

  ClientUnit *m_Client;
  mafString   m_ServerHost;
  int         m_PortNumber;
  bool        m_Connected;
private:
  mmdClientMAF(const mmdClientMAF&);  // Not implemented.
  void operator=(const mmdClientMAF&);  // Not implemented.
};
#endif
