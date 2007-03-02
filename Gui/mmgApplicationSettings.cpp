/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mmgApplicationSettings.cpp,v $
Language:  C++
Date:      $Date: 2007-03-02 12:10:02 $
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

#include "mmgApplicationSettings.h"
#include <wx/config.h>
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mmgGui.h"

//----------------------------------------------------------------------------
mmgApplicationSettings::mmgApplicationSettings(mafObserver *Listener)
//----------------------------------------------------------------------------
{
	m_Listener    = Listener;
  
  // Default values for the application.
  m_LogToFile   = 0;
  m_VerboseLog  = 0;
  m_LogFolder = wxGetCwd().c_str();

	m_ImageTypeId = 0;
	InitializeImageType();
  
  m_AnonymousFalg = true;
  m_RemoteHostName = "ftp://ftp.wxwindows.org";
  m_UserName = "anonymous";
  m_Password = "anonymous@wxwindows.org";
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
  m_Gui->String(ID_PASSWORD,_("pwd"),&m_Password,"",false,true);
  m_Gui->DirOpen(ID_CACHE_FOLDER,_("cache"),&m_CacheFolder,_("set the local cache folder \nin which put downloaded files"));
  m_Gui->Divider(2);
  m_Gui->Bool(ID_LOG_TO_FILE,_("log to file"),&m_LogToFile,1);
  m_Gui->Bool(ID_LOG_VERBOSE,_("log verbose"),&m_VerboseLog,1);
  m_Gui->DirOpen(ID_LOD_DIR,_("log dir"),&m_LogFolder);
  m_Gui->Divider(2);
  EnableItems();
  m_Gui->Label(_("changes will take effect when the \napplication restart"),false,true);
  m_Gui->Label("");
	wxString id_array[2] = {_("JPG") , _("BMP")};
	m_Gui->Combo(IMAGE_TYPE_ID,_("image type"), &m_ImageTypeId,2,id_array);
}
//----------------------------------------------------------------------------
mmgApplicationSettings::~mmgApplicationSettings()
//----------------------------------------------------------------------------
{
  m_Gui = NULL; // gui is destroyed by the dialog.
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_VERBOSE,m_LogToFile != 0);
  m_Gui->Enable(ID_LOD_DIR,m_LogToFile != 0);

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
  wxConfig *config = new wxConfig(wxEmptyString);

  switch(maf_event->GetId())
  {
    case ID_LOG_TO_FILE:
      config->Write("LogToFile",m_LogToFile);
    break;
    case ID_LOG_VERBOSE:
      config->Write("LogVerbose",m_VerboseLog);
    break;
    case ID_LOD_DIR:
      config->Write("LogFolder",m_LogFolder.GetCStr());
    break;
    case ID_CACHE_FOLDER:
      config->Write("CacheFolder",m_CacheFolder.GetCStr());
    break;
    case ID_STORAGE_TYPE:
      config->Write("UseRemoteStorage",m_UseRemoteStorage);
    break;
    case ID_ANONYMOUS_USER:
      config->Write("AnonymousConnection",m_AnonymousFalg);
    break;
    case ID_HOST_NAME:
      config->Write("RemoteHost",m_RemoteHostName.GetCStr());
    break;
    case ID_PORT:
      config->Write("PortConnection",m_Port);
    break;
    case ID_USERNAME:
    {
      std::string cry_user;
      cry_user.clear();
      mafEncryptFromMemory(m_UserName.GetCStr(),m_UserName.Length(),cry_user,m_PassPhrase);
      config->Write("User",cry_user.c_str());
    }
    break;
    case ID_PASSWORD:
    {
      std::string cry_pwd;
      cry_pwd.clear();
      mafEncryptFromMemory(m_Password.GetCStr(),m_Password.Length(),cry_pwd,m_PassPhrase);
      config->Write("Password",cry_pwd.c_str());
    }
    break;
    case ID_PASSPHRASE:
    break;
    case ID_USE_DEFAULT_PASSPHRASE:
      config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
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
      OnEvent(&mafEvent(this,ID_USERNAME));
      OnEvent(&mafEvent(this,ID_PASSWORD));
    break;
		case IMAGE_TYPE_ID:
			{
				config->Write("ImageType",wxString::Format("%d",m_ImageTypeId));
			}
			break;
    default:
      mafEventMacro(*maf_event);
    break; 
  }
  EnableItems();

  cppDEL(config);
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::InitializeApplicationSettings()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  std::string crypted_user, crypted_pwd;
  std::string decrypted_user, decrypted_pwd;
  crypted_user.clear();
  crypted_pwd.clear();
  decrypted_user.clear();
  decrypted_pwd.clear();
  wxString string_item;
  long long_item;
  if(config->Read("LogToFile", &long_item))
  {
    m_LogToFile = long_item;
  }
  else
  {
    config->Write("LogToFile",m_LogToFile);
  }
  if(config->Read("LogVerbose", &long_item))
  {
    m_VerboseLog = long_item;
  }
  else
  {
    config->Write("LogVerbose",m_VerboseLog);
  }
  if(config->Read("LogFolder", &string_item))
  {
    m_LogFolder = string_item.c_str();
  }
  else
  {
    config->Write("LogFolder",m_LogFolder.GetCStr());
  }
  if(config->Read("UseRemoteStorage", &long_item))
  {
    m_UseRemoteStorage = long_item;
  }
  else
  {
    config->Write("UseRemoteStorage",m_UseRemoteStorage);
  }
  if(config->Read("PortConnection", &long_item))
  {
    m_Port = long_item;
  }
  else
  {
    config->Write("PortConnection",m_Port);
  }
  if(config->Read("AnonymousConnection", &long_item))
  {
    m_AnonymousFalg = long_item;
  }
  else
  {
    config->Write("AnonymousConnection",m_AnonymousFalg);
  }
  if(config->Read("CacheFolder", &string_item))
  {
    m_CacheFolder = string_item.c_str();
  }
  else
  {
    config->Write("CacheFolder",m_CacheFolder.GetCStr());
  }
  if(config->Read("RemoteHost", &string_item))
  {
    m_RemoteHostName = string_item.c_str();
  }
  else
  {
    config->Write("RemoteHost",m_RemoteHostName.GetCStr());
  }
  if (config->Read("UseDefaultPassphrase", &long_item))
  {
    m_UseDefaultPasPhrase = long_item;
  }
  else
  {
    config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
  }
  if (m_UseDefaultPasPhrase == 0)
  {
    m_PassPhrase = wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString).c_str();
  }
  if(config->Read("User", &string_item))
  {
    mafDecryptInMemory(string_item.c_str(),decrypted_user,m_PassPhrase);
    m_UserName = decrypted_user.c_str();
  }
  else
  {
    mafEncryptFromMemory(m_UserName.GetCStr(),m_UserName.Length(),crypted_user,m_PassPhrase);
    config->Write("User",crypted_user.c_str());
  }
  if(config->Read("Password", &string_item))
  {
    mafDecryptInMemory(string_item.c_str(),decrypted_pwd,m_PassPhrase);
    m_Password = decrypted_pwd.c_str();
  }
  else
  {
    mafEncryptFromMemory(m_Password.GetCStr(),m_Password.Length(),crypted_pwd,m_PassPhrase);
    config->Write("Password",crypted_pwd.c_str());
  }
  
  cppDEL(config);
}
//----------------------------------------------------------------------------
void mmgApplicationSettings::InitializeImageType()
//----------------------------------------------------------------------------
{
	wxConfig *config = new wxConfig(wxEmptyString);
	wxString type;

	if(config->Read(L"ImageType", &type))
	{
		m_ImageTypeId = atoi(mafString(type));
	}
	cppDEL(config);
}