/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDialogRemoteFile.h,v $
  Language:  C++
  Date:      $Date: 2006-10-17 14:55:51 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmgDialogRemoteFile_H__
#define __mmgDialogRemoteFile_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include <curl/curl.h>
#include "mmgDialog.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mmgDialogRemoteFile :
//----------------------------------------------------------------------------
/**
This component allow to make remote file browsing.
*/
class mmgDialogRemoteFile : public mmgDialog
{
public:
	mmgDialogRemoteFile(mafObserver *listener = NULL,const wxString &title = _("Remote file browsing"), long style = mafRESIZABLE);
	~mmgDialogRemoteFile(); 
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

  mmgGui *m_Gui;
  mmgGui *m_GuiList;

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
