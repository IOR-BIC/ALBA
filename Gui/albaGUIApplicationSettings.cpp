/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIApplicationSettings
 Authors: Paolo Quadrani - Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUIApplicationSettings.h"

#include "albaDecl.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
albaGUIApplicationSettings::albaGUIApplicationSettings(albaObserver *Listener, const albaString &label):
albaGUISettings(Listener, label)
//----------------------------------------------------------------------------
{
  // Default values for the application.
  m_LogToFile   = 1; //logging enabled for correcting bug #820
  m_LogFolder = (albaGetAppDataDirectory() + "\\Logs").c_str();

  m_WarnUserFlag = true;

	//creating user app directory if does not exist
  if(!wxDirExists(albaGetAppDataDirectory().c_str()))
    wxMkdir(albaGetAppDataDirectory().c_str());

  //creating Logs directory if does not exist
  if(!wxDirExists(m_LogFolder))
    wxMkdir(m_LogFolder.GetCStr());

  InitializeSettings();

  m_EnableLogDirChoices = true;
}
//----------------------------------------------------------------------------
albaGUIApplicationSettings::~albaGUIApplicationSettings()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaGUIApplicationSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
  m_Gui->Label(_("Application General Settings"),true);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_LOG_TO_FILE,_("Log to file *"),&m_LogToFile,1);
  if (m_EnableLogDirChoices)
  {
  	m_Gui->DirOpen(ID_LOG_DIR,_("Log Dir *"),&m_LogFolder);
  }
  m_Gui->Divider(2);
  m_Gui->Bool(ID_WARN_UNDO, _("Warn on not undoable Operations"), &m_WarnUserFlag, 1, _("If checked the use is warned when an operation \nthat not support the undo is executed."));
  EnableItems();
	m_Gui->Divider(2);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Label("");
  m_Gui->Label(_("* Changes will take effect on application \nrestart"),false,true);
  
  m_Gui->Divider(2);
}
//----------------------------------------------------------------------------
void albaGUIApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_DIR,m_LogToFile != 0);
}
//----------------------------------------------------------------------------
void albaGUIApplicationSettings::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  switch(alba_event->GetId())
  {
    case ID_LOG_TO_FILE:
      m_Config->Write("LogToFile",m_LogToFile);
    break;
    case ID_LOG_DIR:
      m_Config->Write("LogFolder",m_LogFolder.GetCStr());
    break;
    case ID_WARN_UNDO:
      m_Config->Write("WarnUser",m_WarnUserFlag);
      albaEventMacro(albaEvent(this,MENU_OPTION_APPLICATION_SETTINGS));
    break;
    default:
      albaEventMacro(*alba_event);
    break; 
  }
  EnableItems();
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void albaGUIApplicationSettings::InitializeSettings()
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
  
  m_Config->Flush();
}
//----------------------------------------------------------------------------
void albaGUIApplicationSettings::SetLogFolder(albaString log_folder)
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
void albaGUIApplicationSettings::SetLogFileStatus(int log_status)
//----------------------------------------------------------------------------
{
  if (m_LogToFile != log_status)
  {
    m_LogToFile = log_status;
    m_Config->Write("LogToFile",m_LogToFile);
    m_Config->Flush();
  }
}