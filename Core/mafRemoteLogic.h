/*=========================================================================

 Program: MAF2
 Module: mafRemoteLogic
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafRemoteLogic_H__
#define __mafRemoteLogic_H__

#include <wx/socket.h>

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafDeviceButtonsPadMouseRemote;
class mafViewManager;
class mafOpManager;
class mafDeviceClientMAF;

//----------------------------------------------------------------------------
// mafRemoteLogic :
//----------------------------------------------------------------------------
/** This class is used to manage remote messages to be sent to the server or received from it and allow
to reproduce locally remote messages coming from server.
*/
class MAF_EXPORT mafRemoteLogic : public mafObserver
{
public:
	mafRemoteLogic(mafObserver *Listener, mafViewManager *view_manager, mafOpManager *operation_manager);
	~mafRemoteLogic(); 

  /** Used to receive events from the Client Unit.*/
  void OnEvent(mafEventBase *event);

  /** Pack/unpack the message to be sent/received to/from the server. */
  void RemoteMessage(mafString &cmd, bool to_server = true);

  /** Connect the client to the server. */
  bool Connect(wxIPV4address &addr);

  /** Check is client is connected and return true if it is connected.*/
  bool IsSocketConnected();

  /** Disconnect the client from the server. */
  void Disconnect();

  void SetRemoteMouse(mafDeviceButtonsPadMouseRemote *rMouse);

  /** Prepare VME_SELECTED message to be sent to remote clients.*/
  void VmeSelected(mafNode *vme);

  /** Prepare VME_SHOW message to be sent to remote clients.*/
  void VmeShow(mafNode *vme, bool show_vme);

  void SetClientUnit(mafDeviceClientMAF *clientUnit);

  /** In collaborative modality this is used to synchronize the local application with remote ones.*/
  void SynchronizeApplication();

protected:
  mafObserver     *m_Listener;          ///< MAF local listener
  mafViewManager  *m_ViewManager;       ///< Object listened
  mafOpManager    *m_OperationManager;  ///< Object listened
  mafDeviceClientMAF    *m_ClientUnit;        ///< Used to connect the application to a remote server
  mafDeviceButtonsPadMouseRemote  *m_RemoteMouse;

  mafString        m_RemoteMsg;   ///< Command string sent to the server
  mafString        m_CommandSeparator;
};
#endif
