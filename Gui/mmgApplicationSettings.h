/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationSettings.h,v $
Language:  C++
Date:      $Date: 2007-05-23 14:16:13 $
Version:   $Revision: 1.5 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmgApplicationSettings_H__
#define __mmgApplicationSettings_H__

#include "mafObserver.h"
#include <wx/config.h>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmgGui;

//----------------------------------------------------------------------------
// mmgApplicationSettings :
//----------------------------------------------------------------------------
/**
*/
class mmgApplicationSettings : public mafObserver
{
public:
	mmgApplicationSettings(mafObserver *Listener);
	~mmgApplicationSettings(); 

  enum APP_SETTINGS_WIDGET_ID
  {
    ID_LOG_TO_FILE = MINID,
    ID_LOG_VERBOSE,
    ID_LOD_DIR,
    ID_STORAGE_TYPE,
    ID_CACHE_FOLDER,
    ID_HOST_NAME,
    ID_PORT,
    ID_USERNAME,
    ID_PASSWORD,
    ID_ANONYMOUS_USER,
    ID_USE_DEFAULT_PASSPHRASE,
    ID_PASSPHRASE,
		IMAGE_TYPE_ID,
  };

	enum IMAGE_TYPE_LIST
	{
		JPG = 0,
		BMP,
	};

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  void SetListener(mafObserver *Listener) {m_Listener = Listener;};

  mmgGui* GetGui() {return m_Gui;};

  /** Return true if Log To File is enabled.*/
  bool GetLogToFileStatus() {return m_LogToFile != 0;};

  void SetLogFileStatus(int log_status) {m_LogToFile = log_status;};

  /** Return true if Verbose Log is enabled.*/
  bool GetLogVerboseStatus() {return m_VerboseLog != 0;};
  
  void SetLogVerboseStatus(int log_verbose) {m_VerboseLog = log_verbose;};

  /** Return the folder in which store the log files.*/
  mafString &GetLogFolder() {return m_LogFolder;};

  void SetLogFolder(mafString log_folder) {m_LogFolder = log_folder;};

  /** Return true if Remote Storage flag is enabled.*/
  bool UseRemoteStorage() {return m_UseRemoteStorage != 0;};

  void SetUseRemoteStorage(int use_remote) {m_UseRemoteStorage = use_remote;};

  /** Return the path for the local cache in which download remote VMEs.*/
  mafString &GetCacheFolder() {return m_CacheFolder;};

  void SetCacheFolder(mafString cache_folder) {m_CacheFolder = cache_folder;};

  /** Return the path for the remote host name.*/
  mafString &GetRemoteHostName() {return m_RemoteHostName;};

  void SetRemoteHostName(mafString host) {m_RemoteHostName = host;};

  /** Return the port for the connection.*/
  long GetRemotePort() {return m_Port;};

  void SetRemotePort(long port) {m_Port = port;};

  /** Return the username for the connection.*/
  mafString &GetUserName() {return m_UserName;};

  void SetUserName(mafString user) {m_UserName = user;};

  /** Return the password for the connection.*/
  mafString &GetPassword() {return m_Password;};

  void SetPassword(mafString pwd) {m_Password = pwd;};

  /** Return the status of the default pass phrase usage.*/
  bool UseDefaultPassPhrase() {return m_UseDefaultPasPhrase != 0;};

  void SetUseDefaultPassPhrase(int use_default) {m_UseDefaultPasPhrase = use_default;};

  /** Return the Pass phrase used to encrypt/decrypt files.*/
  mafString &GetPassPhrase() {return m_PassPhrase;};

  void SetPassPhrase(mafString pass_phrase) {m_PassPhrase = pass_phrase;};

	/**select image type during saving of the views*/
	int GetImageTypeId(){return m_ImageTypeId;};

protected:
  /** Initialize the application settings.*/
  void InitializeApplicationSettings();

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  mmgGui *m_Gui;

  wxConfig *m_Config;
  
  // Log variables
  int     m_LogToFile;
  int     m_VerboseLog;
  mafString m_LogFolder;

  // Remote storage variables
  int       m_UseRemoteStorage;
  int       m_AnonymousFalg;
  int       m_Port;
  mafString m_CacheFolder;
  mafString m_RemoteHostName;
  mafString m_UserName;
  mafString m_Password;
  
  // Encryption variables
  int       m_UseDefaultPasPhrase;
  mafString m_PassPhrase;

  mafObserver *m_Listener;

	int          m_ImageTypeId;
};
#endif
