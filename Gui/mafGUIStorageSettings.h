/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIStorageSettings.h,v $
Language:  C++
Date:      $Date: 2007-09-07 11:32:26 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUIStorageSettings_H__
#define __mafGUIStorageSettings_H__

#include "mafObserver.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mafGUIStorageSettings :
//----------------------------------------------------------------------------
/**
*/
class mafGUIStorageSettings : public mafObserver
{
public:
	mafGUIStorageSettings(mafObserver *Listener);
	~mafGUIStorageSettings(); 

  enum STORAGE_SETTINGS_WIDGET_ID
  {
    ID_SINGLE_FILE = MINID,
    ID_STORAGE_TYPE,
    ID_CACHE_FOLDER,
    ID_HOST_NAME,
    ID_PORT,
    ID_USERNAME,
    ID_PASSWORD,
    ID_ANONYMOUS_USER,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  /** Return the GUI of the setting panel.*/
  mmgGui* GetGui();

  /** Return the status for the single file storing animated VMEs.*/
  int GetSingleFileStatus() {return m_SingleFileFlag;};

  /** Set the flag for storing animated VMEs as single file.*/
  void SetSingleFileStatus(int single_file);

  /** Return true if Remote Storage flag is enabled.*/
  bool UseRemoteStorage() {return m_UseRemoteStorage != 0;};

  /** Set the flag for remote storage usage.*/
  void SetUseRemoteStorage(int use_remote);

  /** Return the path for the local cache in which download remote VMEs.*/
  mafString &GetCacheFolder() {return m_CacheFolder;};

  /** Set the cache folder.*/
  void SetCacheFolder(mafString cache_folder);

  /** Return the path for the remote host name.*/
  mafString &GetRemoteHostName() {return m_RemoteHostName;};

  /** Set the remote host-name to which connect with remote storage.*/
  void SetRemoteHostName(mafString host);

  /** Return the port for the connection.*/
  long GetRemotePort() {return m_Port;};

  /** Set the remote storage port through which connect.*/
  void SetRemotePort(long port);

  /** Return the username for the connection.*/
  mafString &GetUserName() {return m_UserName;};

  /** Set the User name with which connect to the remote storage.*/
  void SetUserName(mafString user);

  /** Return the password for the connection.*/
  mafString &GetPassword() {return m_Password;};

  /** Set the user password with which access the remote storage.*/
  void SetPassword(mafString pwd);

protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeApplicationSettings();

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  mmgGui *m_Gui;

  wxConfig *m_Config;
  
  // Single file variable
  int m_SingleFileFlag; ///< Flag used to turn On-Off single file mode for storing animated or not items.

  // Remote storage variables
  int       m_UseRemoteStorage; ///< Flag used to store the using or not of remote storage
  int       m_AnonymousFalg;
  int       m_Port;
  mafString m_CacheFolder;
  mafString m_RemoteHostName;
  mafString m_UserName;
  mafString m_Password;
  
  mafObserver *m_Listener;
};
#endif
