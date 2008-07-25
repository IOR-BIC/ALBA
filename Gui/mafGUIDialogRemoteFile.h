/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIDialogRemoteFile.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafGUIDialogRemoteFile_H__
#define __mafGUIDialogRemoteFile_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include <curl/curl.h>
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafGUI;

//----------------------------------------------------------------------------
// mafGUIDialogRemoteFile :
//----------------------------------------------------------------------------
/**
This component allow to make remote file browsing.
*/
class mafGUIDialogRemoteFile : public mafGUIDialog
{
public:
	mafGUIDialogRemoteFile(mafObserver *listener = NULL,const wxString &title = _("Remote file browsing"), long style = mafRESIZABLE);
	~mafGUIDialogRemoteFile(); 
	void OnEvent(mafEventBase *maf_event);
	void SetListener(mafObserver *listener) {m_Listener = listener;};

  /** Return the filename of the choose (remote or local) file*/
  mafString GetFile() {return m_RemoteFilename;};

  /** Return the host-name if the user changed the default one.*/
  mafString GetHost() {return m_Host;};

  /** Return the user-name for remote connection.*/
  mafString GetUser() {return m_User;};

  /** Return the password for remote connection.*/
  mafString GetPassword() {return m_Pwd;};

  /** Return the server port for remote connection.*/
  int GetPort() {return m_Port;};

protected:
	/** Create the GUI for the FlyTo animation */
	void CreateGui();

  /** FTP browsing of remote filesystem.*/
  void RemoteFileBrowsing();

  /** Enable/Disable dialog widgets according to the protocol choose for the file.*/
  void EnableWidgets();

  mafGUI *m_Gui;
  mafGUI *m_GuiList;

  CURL        *m_Curl;
  CURLcode     m_Result;

  mafString    m_Host;
  mafString    m_User;
  mafString    m_Pwd;
  mafString    m_RemotePath;
  mafString    m_RemoteFilename;
  int          m_Port;
  wxListBox   *m_RemoteFilesList;
	mafObserver	*m_Listener;
};
#endif
