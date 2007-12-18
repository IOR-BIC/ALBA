/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUISettingsStorage.cpp,v $
Language:  C++
Date:      $Date: 2007-12-18 10:57:54 $
Version:   $Revision: 1.5 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGUISettingsStorage.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mafGUISettingsStorage::mafGUISettingsStorage(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  //m_SingleFileFlag = 1;
  
  m_AnonymousFalg = true;
  m_RemoteHostName = "";
  m_UserName = "";
  m_Password = "";
  m_Port = 21;
  m_RemoteStorageType = 0;
  m_CacheFolder = wxGetCwd().c_str();


  m_SRBRemoteHostName = "";
  m_SRBDomain = "";
  m_SRBServer_dn = "";
  m_SRBAuth_scheme = "";
  m_SRBUserName = "";
  m_SRBPwd = "";
  m_SRBPort = 12544;
  m_SRBCacheFolder = wxGetCwd().c_str();

  InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUISettingsStorage::~mafGUISettingsStorage()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::CreateGui()
//----------------------------------------------------------------------------
{
  wxString remoteStorageArray[3] = {"Local", "HTTP", "SRB"};
  m_Gui = new mmgGui(this);
  m_Gui->Label(_("Storage general settings"));
  //m_Gui->Bool(ID_SINGLE_FILE, _("single file mode"), &m_SingleFileFlag,1);
  //m_Gui->Divider(2);
  m_Gui->Radio(ID_STORAGE_TYPE,_("Storage type"),&m_RemoteStorageType, 3, remoteStorageArray,1,_("Choose the remote storage you want to use."));
  m_Gui->Label(_("Default settings for HTTP storage"));
  m_Gui->Bool(ID_ANONYMOUS_USER,_("anonymous connection"),&m_AnonymousFalg,1);
  m_Gui->String(ID_HOST_NAME,"host",&m_RemoteHostName);
  m_Gui->Integer(ID_PORT,_("port"),&m_Port,0);
  m_Gui->String(ID_USERNAME,_("user"),&m_UserName);
  m_Gui->DirOpen(ID_CACHE_FOLDER,_("cache"),&m_CacheFolder,_("set the local cache folder \nin which put downloaded files"));


    //--------------------SRB--------------------//
  m_Gui->Divider(2);
  m_Gui->Label(_("Default settings for SRB storage"));
  m_Gui->String(ID_SRB_HOST_NAME,"host",&m_SRBRemoteHostName);
  m_Gui->String(ID_SRB_SERVER_DN,"server_dn",&m_SRBServer_dn);
  m_Gui->String(ID_SRB_DOMAIN,"domain",&m_SRBDomain);
  m_Gui->String(ID_SRB_AUTH_SCHEME,"auth",&m_SRBAuth_scheme);
  m_Gui->Integer(ID_SRB_PORT,_("port"),&m_SRBPort,0);
  m_Gui->String(ID_SRB_USERNAME,_("user"),&m_SRBUserName);
  m_Gui->String(ID_SRB_PWD,_("pwd"),&m_SRBPwd,"",false,true);
  m_Gui->DirOpen(ID_SRB_CACHE_FOLDER,_("cache"),&m_SRBCacheFolder,_("set the local cache folder \nin which put downloaded files"));

  EnableItems();
  m_Gui->Label(_("changes will take effect when the \napplication restart"),false,true);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_CACHE_FOLDER,m_RemoteStorageType == HTTP);
  m_Gui->Enable(ID_ANONYMOUS_USER,m_RemoteStorageType == HTTP);
  m_Gui->Enable(ID_HOST_NAME,m_RemoteStorageType == HTTP);
  m_Gui->Enable(ID_PORT,m_RemoteStorageType == HTTP);
  m_Gui->Enable(ID_USERNAME,m_RemoteStorageType == HTTP && m_AnonymousFalg == 0);
  m_Gui->Enable(ID_PASSWORD,m_RemoteStorageType == HTTP && m_AnonymousFalg == 0);


  //--------------------SRB--------------------//
  m_Gui->Enable(ID_SRB_HOST_NAME,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_SERVER_DN,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_DOMAIN,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_AUTH_SCHEME,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_PORT,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_USERNAME,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_PWD,m_RemoteStorageType == SRB);
  m_Gui->Enable(ID_SRB_CACHE_FOLDER,m_RemoteStorageType == SRB);
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    /*case ID_SINGLE_FILE:
      m_Config->Write("SingleFileMode",m_SingleFileFlag);
    break;*/
    case ID_CACHE_FOLDER:
      m_Config->Write("CacheFolder",m_CacheFolder.GetCStr());
    break;
    case ID_STORAGE_TYPE:
      m_Config->Write("RemoteStorageType",m_RemoteStorageType);
    break;
    case ID_ANONYMOUS_USER:
      m_Config->Write("AnonymousConnection",m_AnonymousFalg);
    break;
    case ID_HOST_NAME:
      m_Config->Write("RemoteHost",m_RemoteHostName.GetCStr());
    break;
    case ID_PORT:
      m_Config->Write("PortConnection",m_Port);
    break;
    case ID_USERNAME:
      m_Config->Write("User",m_UserName.GetCStr());
    break;
    case ID_PASSWORD:
    break;

      //--------------------SRB--------------------//
    case ID_SRB_HOST_NAME:
      m_Config->Write("SRBRemoteHostName",m_SRBRemoteHostName.GetCStr());
      break;
    case ID_SRB_SERVER_DN:
      m_Config->Write("SRBServer_dn",m_SRBServer_dn.GetCStr());
      break;
    case ID_SRB_DOMAIN:
      m_Config->Write("SRBDomain",m_SRBDomain.GetCStr());
      break;
    case ID_SRB_AUTH_SCHEME:
      m_Config->Write("SRBAuth_scheme",m_SRBAuth_scheme.GetCStr());
      break;
    case ID_SRB_PORT:
      m_Config->Write("SRBPort",m_SRBPort);
      break;
    case ID_SRB_USERNAME:
      m_Config->Write("SRBUserName",m_SRBUserName.GetCStr());
      break;
    case ID_SRB_PWD:
       m_Config->Write("SRBPwd",m_SRBPwd.GetCStr());
      break;
    case ID_SRB_CACHE_FOLDER:
      m_Config->Write("SRBCacheFolder",m_SRBCacheFolder.GetCStr());
      break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  EnableItems();
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::InitializeSettings()
//----------------------------------------------------------------------------
{
  wxString string_item;
  long long_item;
  /*if(m_Config->Read("SingleFileMode", &long_item))
  {
    m_SingleFileFlag = long_item;
  }
  else
  {
    m_Config->Write("SingleFileMode", m_SingleFileFlag);
  }*/
  if(m_Config->Read("RemoteStorageType", &long_item))
  {
    m_RemoteStorageType = long_item;
  }
  else
  {
    m_Config->Write("RemoteStorageType",m_RemoteStorageType);
  }
  if(m_Config->Read("PortConnection", &long_item))
  {
    m_Port = long_item;
  }
  else
  {
    m_Config->Write("PortConnection",m_Port);
  }
  if(m_Config->Read("AnonymousConnection", &long_item))
  {
    m_AnonymousFalg = long_item;
  }
  else
  {
    m_Config->Write("AnonymousConnection",m_AnonymousFalg);
  }
  if(m_Config->Read("CacheFolder", &string_item))
  {
    m_CacheFolder = string_item.c_str();
  }
  else
  {
    m_Config->Write("CacheFolder",m_CacheFolder.GetCStr());
  }
  if(m_Config->Read("RemoteHost", &string_item))
  {
    m_RemoteHostName = string_item.c_str();
  }
  else
  {
    m_Config->Write("RemoteHost",m_RemoteHostName.GetCStr());
  }
  if(m_Config->Read("User", &string_item))
  {
    m_UserName = string_item.c_str();
  }
  else
  {
    m_Config->Write("User",m_UserName.GetCStr());
  }


    //--------------------SRB--------------------//
  if(m_Config->Read("SRBRemoteHostName", &string_item))
  {
    m_SRBRemoteHostName = string_item;
  }
  else
  {
    m_Config->Write("SRBRemoteHostName", m_SRBRemoteHostName);
  }
  if(m_Config->Read("SRBServer_dn", &string_item))
  {
    m_SRBServer_dn = string_item;
  }
  else
  {
    m_Config->Write("SRBServer_dn", m_SRBServer_dn);
  }
  if(m_Config->Read("SRBDomain", &string_item))
  {
    m_SRBDomain = string_item;
  }
  else
  {
    m_Config->Write("SRBDomain", m_SRBDomain);
  }
  if(m_Config->Read("SRBAuth_scheme", &string_item))
  {
    m_SRBAuth_scheme = string_item;
  }
  else
  {
    m_Config->Write("SRBAuth_scheme", m_SRBAuth_scheme);
  }
  if(m_Config->Read("SRBPort", &long_item))
  {
    m_SRBPort = long_item;
  }
  else
  {
    m_Config->Write("SRBPort", m_SRBPort);
  }
  if(m_Config->Read("SRBUserName", &string_item))
  {
    m_SRBUserName = string_item;
  }
  else
  {
    m_Config->Write("SRBUserName", m_SRBUserName);
  }
  if(m_Config->Read("SRBPwd", &string_item))
  {
    m_SRBPwd = string_item;
  }
  else
  {
    m_Config->Write("SRBPwd", m_SRBPwd);
  }
  if(m_Config->Read("SRBCacheFolder", &string_item))
  {
    m_SRBCacheFolder = string_item.c_str();
  }
  else
  {
    m_Config->Write("SRBCacheFolder",m_SRBCacheFolder.GetCStr());
  }
  m_Config->Flush();
}
/*//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSingleFileStatus(int single_file)
//----------------------------------------------------------------------------
{
  if (m_SingleFileFlag != single_file)
  {
    m_SingleFileFlag = single_file;
    m_Config->Write("SingleFileMode", m_SingleFileFlag);
    m_Config->Flush();
  }
}*/
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetUseRemoteStorage(int use_remote)
//----------------------------------------------------------------------------
{
  if (m_RemoteStorageType != use_remote)
  {
    m_RemoteStorageType = use_remote;
    m_Config->Write("RemoteStorageType",m_RemoteStorageType);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetStorageType(int storgeType)
//----------------------------------------------------------------------------
{
  if (m_RemoteStorageType != storgeType)
  {
    m_RemoteStorageType = storgeType;
    m_Config->Write("RemoteStorageType",m_RemoteStorageType);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetCacheFolder(mafString cache_folder)
//----------------------------------------------------------------------------
{
  if (m_CacheFolder != cache_folder)
  {
    m_CacheFolder = cache_folder;
    m_Config->Write("CacheFolder",m_CacheFolder.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetRemoteHostName(mafString host)
//----------------------------------------------------------------------------
{
  if (m_RemoteHostName != host)
  {
    m_RemoteHostName = host;
    m_Config->Write("RemoteHost",m_RemoteHostName.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetRemotePort(long port)
//----------------------------------------------------------------------------
{
  if (m_Port != port)
  {
    m_Port = port;
    m_Config->Write("PortConnection",m_Port);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetUserName(mafString user)
//----------------------------------------------------------------------------
{
  if (m_UserName != user)
  {
    m_UserName = user;
    m_Config->Write("User",m_UserName.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetPassword(mafString pwd)
//----------------------------------------------------------------------------
{
  m_Password = pwd;
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBServer_dn(mafString SRBServer_dn)
//----------------------------------------------------------------------------
{
  if (m_SRBServer_dn != SRBServer_dn)
  {
    m_SRBServer_dn = SRBServer_dn;
    m_Config->Write("SRBServer_dn",m_SRBServer_dn);
    m_Config->Flush();
  }
}

//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBDomain(mafString SRBDomain)
//----------------------------------------------------------------------------
{
  if (m_SRBDomain != SRBDomain)
  {
    m_SRBDomain = SRBDomain;
    m_Config->Write("SRBDomain",m_SRBServer_dn);
    m_Config->Flush();
  }
}

//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBAuth_scheme(mafString SRBAuth_scheme)
//----------------------------------------------------------------------------
{
  if (m_SRBAuth_scheme != SRBAuth_scheme)
  {
    m_SRBAuth_scheme = SRBAuth_scheme;
    m_Config->Write("SRBAuth_scheme",m_SRBAuth_scheme);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBCacheFolder(mafString SRBcache_folder)
//----------------------------------------------------------------------------
{
  if (m_SRBCacheFolder != SRBcache_folder)
  {
    m_SRBCacheFolder = SRBcache_folder;
    m_Config->Write("SRBCacheFolder",m_SRBCacheFolder.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBRemoteHostName(mafString SRBhost)
//----------------------------------------------------------------------------
{
  if (m_SRBRemoteHostName != SRBhost)
  {
    m_SRBRemoteHostName = SRBhost;
    m_Config->Write("SRBRemoteHost",m_SRBRemoteHostName.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBRemotePort(long SRBport)
//----------------------------------------------------------------------------
{
  if (m_SRBPort != SRBport)
  {
    m_SRBPort = SRBport;
    m_Config->Write("SRBPort",m_SRBPort);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBUserName(mafString SRBUserName)
//----------------------------------------------------------------------------
{
  if (m_SRBUserName != SRBUserName)
  {
    m_SRBUserName = SRBUserName;
    m_Config->Write("SRBUserName",m_SRBUserName.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUISettingsStorage::SetSRBPwd(mafString SRBpwd)
//----------------------------------------------------------------------------
{
  m_SRBPwd = SRBpwd;
}