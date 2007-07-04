/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationSettings.cpp,v $
Language:  C++
Date:      $Date: 2007-07-04 12:48:09 $
Version:   $Revision: 1.8 $
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

#include "mmgApplicationSettings.h"
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mmgApplicationSettings::mmgApplicationSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener    = Listener;
  m_Config = new wxConfig(wxEmptyString);
  
  // Default values for the application.
  m_LogToFile   = 0;
  m_VerboseLog  = 0;
  m_LogFolder = wxGetCwd().c_str();

	m_ImageTypeId = 0;

  m_WarnUserFlag = true;
  
  m_AnonymousFalg = true;
//  m_RemoteHostName = "ftp://ftp.wxwindows.org";
//  m_UserName = "anonymous";
//  m_Password = "anonymous@wxwindows.org";
  m_RemoteHostName = "";
  m_UserName = "";
  m_Password = "";
  m_Port = 21;
  m_UseRemoteStorage = 0;
  m_CacheFolder = wxGetCwd().c_str();
  
  m_UseDefaultPasPhrase = 1;
  m_PassPhrase = mafDefaultPassPhrase();

  InitializeApplicationSettings();

  m_Gui = new mmgGui(this);
  m_Gui->Label(_("Application general settings"));
  m_Gui->Bool(ID_USE_DEFAULT_PASSPHRASE,_("use default passphrase"),&m_UseDefaultPasPhrase,1);
  m_Gui->String(ID_PASSPHRASE,_("passphrase"),&m_PassPhrase,"",false,true);
  m_Gui->Divider(2);
  m_Gui->Label(_("Default settings for remote storage"));
  m_Gui->Bool(ID_STORAGE_TYPE,_("remote storage"),&m_UseRemoteStorage,1,_("Check if you want to use remote storage instead the local one."));
  m_Gui->Bool(ID_ANONYMOUS_USER,_("anonymous connection"),&m_AnonymousFalg,1);
  m_Gui->String(ID_HOST_NAME,"host",&m_RemoteHostName);
  m_Gui->Integer(ID_PORT,_("port"),&m_Port,0);
  m_Gui->String(ID_USERNAME,_("user"),&m_UserName);
  //m_Gui->String(ID_PASSWORD,_("pwd"),&m_Password,"",false,true);
  m_Gui->DirOpen(ID_CACHE_FOLDER,_("cache"),&m_CacheFolder,_("set the local cache folder \nin which put downloaded files"));
  m_Gui->Divider(2);
  m_Gui->Bool(ID_LOG_TO_FILE,_("log to file"),&m_LogToFile,1);
  m_Gui->Bool(ID_LOG_VERBOSE,_("log verbose"),&m_VerboseLog,1);
  m_Gui->DirOpen(ID_LOG_DIR,_("log dir"),&m_LogFolder);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_WARN_UNDO, _("warn on undoable"), &m_WarnUserFlag, 1, _("If checked the use is warned when an operation \nthat not support the undo is executed."));
  EnableItems();
  m_Gui->Label(_("changes will take effect when the \napplication restart"),false,true);
  m_Gui->Label("");
	wxString id_array[2] = {_("JPG") , _("BMP")};
	m_Gui->Combo(IMAGE_TYPE_ID,_("image type"), &m_ImageTypeId,2,id_array);
  m_Gui->Divider(2);
}
//----------------------------------------------------------------------------
mmgApplicationSettings::~mmgApplicationSettings()
//----------------------------------------------------------------------------
{
  cppDEL(m_Config);
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_VERBOSE,m_LogToFile != 0);
  m_Gui->Enable(ID_LOG_DIR,m_LogToFile != 0);

  m_Gui->Enable(ID_CACHE_FOLDER,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_ANONYMOUS_USER,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_HOST_NAME,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_PORT,m_UseRemoteStorage != 0);
  m_Gui->Enable(ID_USERNAME,m_UseRemoteStorage != 0 && m_AnonymousFalg == 0);
  m_Gui->Enable(ID_PASSWORD,m_UseRemoteStorage != 0 && m_AnonymousFalg == 0);

  m_Gui->Enable(ID_PASSPHRASE,m_UseDefaultPasPhrase == 0);
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_LOG_TO_FILE:
      m_Config->Write("LogToFile",m_LogToFile);
    break;
    case ID_LOG_VERBOSE:
      m_Config->Write("LogVerbose",m_VerboseLog);
    break;
    case ID_LOG_DIR:
      m_Config->Write("LogFolder",m_LogFolder.GetCStr());
    break;
    case ID_WARN_UNDO:
      m_Config->Write("WarnUser",m_WarnUserFlag);
      mafEventMacro(mafEvent(this,MENU_OPTION_APPLICATION_SETTINGS));
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
    {
      m_Config->Write("User",m_UserName.GetCStr());
    }
    break;
    case ID_PASSWORD:
    break;
    case ID_PASSPHRASE:
    break;
    case ID_USE_DEFAULT_PASSPHRASE:
      m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
      if (m_UseDefaultPasPhrase != 0)
      {
        m_PassPhrase = mafDefaultPassPhrase();
        wxMessageBox(_("Passphrase resetted to default one!"),_("Warning"));
      }
      else
      {
        m_PassPhrase = wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString).c_str();
      }
      EnableItems();
      m_Gui->Update();
    break;
		case IMAGE_TYPE_ID:
		{
			m_Config->Write("ImageType",m_ImageTypeId);
		}
		break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  EnableItems();
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::InitializeApplicationSettings()
//----------------------------------------------------------------------------
{
  wxString string_item;
  long long_item;
  if(m_Config->Read("LogToFile", &long_item))
  {
    m_LogToFile = long_item;
  }
  else
  {
    m_Config->Write("LogToFile",m_LogToFile);
  }
  if(m_Config->Read("LogVerbose", &long_item))
  {
    m_VerboseLog = long_item;
  }
  else
  {
    m_Config->Write("LogVerbose",m_VerboseLog);
  }
  if(m_Config->Read("LogFolder", &string_item))
  {
    m_LogFolder = string_item.c_str();
  }
  else
  {
    m_Config->Write("LogFolder",m_LogFolder.GetCStr());
  }
  if (m_Config->Read("WarnUser", &long_item))
  {
    m_WarnUserFlag = long_item;
  }
  else
  {
    m_Config->Write("WarnUser",m_WarnUserFlag);
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
  if (m_Config->Read("UseDefaultPassphrase", &long_item))
  {
    m_UseDefaultPasPhrase = long_item;
  }
  else
  {
    m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
  }
  if (m_UseDefaultPasPhrase == 0)
  {
    m_PassPhrase = wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString).c_str();
  }
  if(m_Config->Read("User", &string_item))
  {
    m_UserName = string_item.c_str();
  }
  else
  {
    m_Config->Write("User",m_UserName.GetCStr());
  }
  if(m_Config->Read("ImageType", &long_item))
  {
    m_ImageTypeId = long_item;
  }
  else
  {
    m_Config->Write("ImageType", m_ImageTypeId);
  }
  m_Config->Flush();
}
