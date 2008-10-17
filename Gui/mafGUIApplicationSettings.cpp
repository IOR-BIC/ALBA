/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafGUIApplicationSettings.cpp,v $
Language:  C++
Date:      $Date: 2008-10-17 11:51:53 $
Version:   $Revision: 1.1.2.1 $
Authors:   Paolo Quadrani - Daniele Giunchi
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

#include "mafGUIApplicationSettings.h"
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mafGUI.h"

//----------------------------------------------------------------------------
mafGUIApplicationSettings::mafGUIApplicationSettings(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  // Default values for the application.
  m_LogToFile   = 1; //logging enabled for correcting bug #820
  m_VerboseLog  = 0;
  m_LogFolder = wxGetCwd().c_str();

	m_ImageTypeId = 1;

  m_WarnUserFlag = true;
  
  m_UseDefaultPasPhrase = 1;
  m_PassPhrase = mafDefaultPassPhrase();

  InitializeSettings();
}
//----------------------------------------------------------------------------
mafGUIApplicationSettings::~mafGUIApplicationSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_("Application general settings"));
  m_Gui->Bool(ID_USE_DEFAULT_PASSPHRASE,_("use default passphrase"),&m_UseDefaultPasPhrase,1);
  m_Gui->String(ID_PASSPHRASE,_("passphrase"),&m_PassPhrase,"",false,true);
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
void mafGUIApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_VERBOSE,m_LogToFile != 0);
  m_Gui->Enable(ID_LOG_DIR,m_LogToFile != 0);
  m_Gui->Enable(ID_PASSPHRASE,m_UseDefaultPasPhrase == 0);
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case ID_LOG_TO_FILE:
      m_Config->Write("LogToFile",m_LogToFile);
    break;
    case ID_LOG_VERBOSE:
      m_Config->Write("LogVerbose",m_VerboseLog);
      mafEventBase::SetLogVerbose(m_VerboseLog != 0);
    break;
    case ID_LOG_DIR:
      m_Config->Write("LogFolder",m_LogFolder.GetCStr());
    break;
    case ID_WARN_UNDO:
      m_Config->Write("WarnUser",m_WarnUserFlag);
      mafEventMacro(mafEvent(this,MENU_OPTION_APPLICATION_SETTINGS));
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
void mafGUIApplicationSettings::InitializeSettings()
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
    mafEventBase::SetLogVerbose(m_VerboseLog != 0);
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
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogFolder(mafString log_folder)
//----------------------------------------------------------------------------
{
  if (m_LogFolder != log_folder)
  {
    m_LogFolder = log_folder;
    m_Config->Write("LogFolder",m_LogFolder.GetCStr());
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogFileStatus(int log_status)
//----------------------------------------------------------------------------
{
  if (m_LogToFile != log_status)
  {
    m_LogToFile = log_status;
    m_Config->Write("LogToFile",m_LogToFile);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetLogVerboseStatus(int log_verbose)
//----------------------------------------------------------------------------
{
  if (m_VerboseLog != log_verbose)
  {
    m_VerboseLog = log_verbose;
    m_Config->Write("LogVerbose",m_VerboseLog);
    m_Config->Flush();
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetUseDefaultPassPhrase(int use_default, mafString passphrase)
//----------------------------------------------------------------------------
{
  if (m_UseDefaultPasPhrase != use_default)
  {
    m_UseDefaultPasPhrase = use_default;
    m_Config->Write("UseDefaultPassphrase",m_UseDefaultPasPhrase);
    m_Config->Flush();
    if (m_UseDefaultPasPhrase != 0)
    {
      m_PassPhrase = mafDefaultPassPhrase();
    }
    else
    {
      if (passphrase.IsEmpty())
      {
        m_PassPhrase = wxGetPasswordFromUser(_("Insert passphrase"),_("Passphrase"),wxEmptyString).c_str();
      }
      else
      {
        m_PassPhrase = passphrase;
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationSettings::SetPassPhrase(mafString pass_phrase)
//----------------------------------------------------------------------------
{
  m_PassPhrase = pass_phrase;
}
