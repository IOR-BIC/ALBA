/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafRemoteLogic.h,v $
Language:  C++
Date:      $Date: 2006-06-03 10:57:37 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafRemoteLogic_H__
#define __mafRemoteLogic_H__

#include <wx/socket.h>

#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmdRemoteMouse;
class mafViewManager;
class mafOpManager;
class mmdClientMAF;

//----------------------------------------------------------------------------
// mafRemoteLogic :
//----------------------------------------------------------------------------
/**
*/
class mafRemoteLogic : public mafObserver
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

  void SetRemoteMouse(mmdRemoteMouse *rMouse);

  void VmeSelected(mafNode *vme);

  void SetClientUnit(mmdClientMAF *clientUnit);

  /** In collaborative modality this is used to synchronize the local application with remote ones.*/
  void SynchronizeApplication();

protected:
  mafObserver     *m_Listener;          ///< MAF local listener
  mafViewManager  *m_ViewManager;       ///< Object listened
  mafOpManager    *m_OperationManager;  ///< Object listened
  mmdClientMAF    *m_ClientUnit;        ///< Used to connect the application to a remote server
  mmdRemoteMouse  *m_RemoteMouse;

  mafString        m_RemoteMsg;   ///< Command string sent to the server
  mafString        m_CommandSeparator;
};
#endif
