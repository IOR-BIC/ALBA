/*=========================================================================

 Program: MAF2
 Module: mafGUIApplicationSettings
 Authors: Paolo Quadrani - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
  m_LogFolder = (mafGetAppDataDirectory() + "\\Logs").c_str();

  m_WarnUserFlag = true;

	//creating user app directory if does not exist
  if(!wxDirExists(mafGetAppDataDirectory().c_str()))
    wxMkdir(mafGetAppDataDirectory().c_str());

  //creating Logs directory if does not exist
  if(!wxDirExists(m_LogFolder))
    wxMkdir(m_LogFolder.GetCStr());

  InitializeSettings();

  m_EnableLogDirChoices = true;
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
void mafGUIApplicationSettings::EnableItems()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_LOG_DIR,m_LogToFile != 0);
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
    case ID_LOG_DIR:
      m_Config->Write("LogFolder",m_LogFolder.GetCStr());
    break;
    case ID_WARN_UNDO:
      m_Config->Write("WarnUser",m_WarnUserFlag);
      mafEventMacro(mafEvent(this,MENU_OPTION_APPLICATION_SETTINGS));
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
