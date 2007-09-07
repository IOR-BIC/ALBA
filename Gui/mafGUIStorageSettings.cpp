/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIStorageSettings.cpp,v $
Language:  C++
Date:      $Date: 2007-09-07 15:24:50 $
Version:   $Revision: 1.2 $
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

#include "mafGUIStorageSettings.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mafGUIStorageSettings::mafGUIStorageSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener = Listener;
  m_Gui = NULL;

  m_Config = new wxConfig(wxEmptyString);
  
  m_SingleFileFlag = 1;
  
  m_AnonymousFalg = true;
  m_RemoteHostName = "";
  m_UserName = "";
  m_Password = "";
  m_Port = 21;
  m_UseRemoteStorage = 0;
  m_CacheFolder = wxGetCwd().c_str();
  
  InitializeApplicationSettings();
}
//----------------------------------------------------------------------------
mafGUIStorageSettings::~mafGUIStorageSettings()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Label(_("Storage general settings"));
  m_Gui->Bool(ID_SINGLE_FILE, _("single file mode"), &m_SingleFileFlag,1);
  m_Gui->Divider(2);
  m_Gui->Label(_("Default settings for remote storage"));
  m_Gui->Bool(ID_STORAGE_TYPE,_("remote storage"),&m_UseRemoteStorage,1,_("Check if you want to use remote storage instead the local one."));
  m_Gui->Bool(ID_ANONYMOUS_USER,_("anonymous connection"),&m_AnonymousFalg,1);
  m_Gui->String(ID_HOST_NAME,"host",&m_RemoteHostName);
  m_Gui->Integer(ID_PORT,_("port"),&m_Port,0);
  m_Gui->String(ID_USERNAME,_("user"),&m_UserName);
  m_Gui->DirOpen(ID_CACHE_FOLDER,_("cache"),&m_CacheFolder,_("set the local cache folder \nin which put downloaded files"));
  EnableItems();
  m_Gui->Label(_("changes will take effect when the \napplication restart"),false,true);
  m_Gui->Label("");
}
//----------------------------------------------------------------------------
mmgGui* mafGUIStorageSettings::GetGui()
//----------------------------------------------------------------------------
{
  if (m_Gui == NULL)
  {
    CreateGui();
  }
  assert(m_Gui);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_CACHE_FOLDER,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_ANONYMOUS_USER,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_HOST_NAME,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_PORT,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_USERNAME,m_UseRemoteStorage != 0 && m_AnonymousFalg == 0);
  m_Gui->Enable(ID_PASSWORD,m_UseRemoteStorage != 0 && m_AnonymousFalg == 0);
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_SINGLE_FILE:
      m_Config->Write("SingleFileMode",m_SingleFileFlag);
    break;
    case ID_CACHE_FOLDER:
      m_Config->Write("CacheFolder",m_CacheFolder.GetCStr());
    break;
    case ID_STORAGE_TYPE:
      m_Config->Write("UseRemoteStorage",m_UseRemoteStorage);
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
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  EnableItems();
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::InitializeApplicationSettings()
//----------------------------------------------------------------------------
{
  wxString string_item;
  long long_item;
  if(m_Config->Read("SingleFileMode", &long_item))
  {
    m_SingleFileFlag = long_item;
  }
  else
  {
    m_Config->Write("SingleFileMode", m_SingleFileFlag);
  }
  if(m_Config->Read("UseRemoteStorage", &long_item))
  {
    m_UseRemoteStorage = long_item;
  }
  else
  {
    m_Config->Write("UseRemoteStorage",m_UseRemoteStorage);
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
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::SetSingleFileStatus(int single_file)
//----------------------------------------------------------------------------
{
  if (m_SingleFileFlag != single_file)
  {
    m_SingleFileFlag = single_file;
    m_Config->Write("SingleFileMode", m_SingleFileFlag);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::SetUseRemoteStorage(int use_remote)
//----------------------------------------------------------------------------
{
  if (m_UseRemoteStorage != use_remote)
  {
    m_UseRemoteStorage = use_remote;
    m_Config->Write("UseRemoteStorage",m_UseRemoteStorage);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIStorageSettings::SetCacheFolder(mafString cache_folder)
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
void mafGUIStorageSettings::SetRemoteHostName(mafString host)
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
void mafGUIStorageSettings::SetRemotePort(long port)
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
void mafGUIStorageSettings::SetUserName(mafString user)
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
void mafGUIStorageSettings::SetPassword(mafString pwd)
//----------------------------------------------------------------------------
{
  m_Password = pwd;
}
