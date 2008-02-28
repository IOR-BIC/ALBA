/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsStorage.h,v $
Language:  C++
Date:      $Date: 2008-02-28 11:12:21 $
Version:   $Revision: 1.6 $
Authors:   Paolo Quadrani, Robero Mucci
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafGUISettingsStorage_H__
#define __mafGUISettingsStorage_H__

#include "mafGUISettings.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafGUISettingsStorage :
//----------------------------------------------------------------------------
/** Gui to set storage parameters. 
    Commented example of SRB List Dialog; requires OPENSSL*/
class mafGUISettingsStorage : public mafGUISettings
{
public:
	mafGUISettingsStorage(mafObserver *Listener, const mafString &label = _("Storage"));
	~mafGUISettingsStorage(); 

  enum STORAGE_SETTINGS_WIDGET_ID
  {
    //ID_SINGLE_FILE,
    ID_STORAGE_TYPE = MINID,
    ID_CACHE_FOLDER,
    ID_HOST_NAME,
    ID_PORT,
    ID_USERNAME,
    ID_PASSWORD,
    ID_ANONYMOUS_USER,
    /*ID_SRB_HOST_NAME,
    ID_SRB_SERVER_DN,
    ID_SRB_DOMAIN,
    ID_SRB_AUTH_SCHEME,
    ID_SRB_PORT,
    ID_SRB_USERNAME,
    ID_SRB_PWD,
    ID_SRB_CACHE_FOLDER,*/
  };

  enum STORAGE_TYPE
  {
    LOCAL,
    HTTP,
    //SRB,
  };

  /** Answer to the messages coming from interface. */
  void OnEvent(mafEventBase *maf_event);

  /** Return the status for the single file storing animated VMEs.*/
  //int GetSingleFileStatus() {return m_SingleFileFlag;};


  /** Set the  storage type. */
  void SetStorageType(int storgeType);

  /** Return storage type. */
  int GetStorageType() {return m_RemoteStorageType;};

  /** Set the flag for storing animated VMEs as single file.*/
  //void SetSingleFileStatus(int single_file);

  /** Return true if Remote Storage flag is enabled.*/
  bool UseRemoteStorage() {return m_RemoteStorageType != 0;};

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


  //--------------------SRB--------------------//

 
  /** Set the remote host-name to which connect with SRB storage.*/
 // void SetSRBRemoteHostName(mafString SRBhost);

  /** Return the path for the SRB host name.*/
 // mafString &GetSRBRemoteHostName() {return m_SRBRemoteHostName;};

  /** Set the Server_dn for SRB storage.*/
  //void SetSRBServer_dn(mafString SRBServer_dn);

  /** Return the Server_dn for SRB storage.*/
 // mafString &GetSRBServer_dn() {return m_SRBServer_dn;};

  /** Set the SRB domain.*/
//  void SetSRBDomain(mafString SRBDomain);

  /** Return the SRB domain.*/
 // mafString &GetSRBDomain() {return m_SRBDomain;};

  /** Set the SRB authentication scheme.*/
 // void SetSRBAuth_scheme(mafString SRBAuth_scheme);
  
  /** Return the SRB authentication scheme.*/
 // mafString &GetetSRBAuth_scheme() {return m_SRBAuth_scheme;};

  /** Set the SRB storage port through which connect.*/
 // void SetSRBRemotePort(long SRBport);

  /** Return the port for the SRB connection.*/
 // long GetSRBRemotePort() {return m_SRBPort;};

  /** Set the User name with which connect to the SRB storage.*/
 // void SetSRBUserName(mafString SRBUserName);

  /** Return the username for the SRB connection.*/
 // mafString &GetSRBUserName() {return m_SRBUserName;}

  /** Set the user password with which access the SRB storage.*/
 // void SetSRBPwd(mafString SRBpwd);

  /** Return the password for the SRB connection.*/
 // mafString &GetSRBPwd() {return m_SRBPwd;};

  /** Set the cache folder.*/
//  void SetSRBCacheFolder(mafString SRBcache_folder);

  /** Return the path for the local cache in which download remote VMEs.*/
 // mafString &GetSRBCacheFolder() {return m_SRBCacheFolder;};



protected:
  /** Create the GUI for the setting panel.*/
  void CreateGui();

  /** Initialize the application settings.*/
  void InitializeSettings();

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();
  
  //int m_SingleFileFlag; ///< Flag used to turn On-Off single file mode for storing animated or not items.

  int       m_RemoteStorageType; ///< Flag used to store the using or not of remote storage
  int       m_AnonymousFalg;
  int       m_Port;
  mafString m_CacheFolder;
  mafString m_RemoteHostName;
  mafString m_UserName;
  mafString m_Password;


    //--------------------SRB--------------------//
 /* int       m_SRBPort;
  mafString m_SRBRemoteHostName;
  mafString m_SRBDomain;
  mafString m_SRBServer_dn;
  mafString m_SRBAuth_scheme;
  mafString m_SRBUserName;
  mafString m_SRBPwd;
  mafString m_SRBCacheFolder;*/
};
#endif
