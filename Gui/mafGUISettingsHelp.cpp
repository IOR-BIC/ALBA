/*=========================================================================

 Program: MAF2
 Module: mafGUISettingsHelp
 Authors: Stefano Perticoni
 
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

#include "mafGUISettingsHelp.h"
#include "mafCrypt.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include <iostream>
#include <fstream>

mafGUISettingsHelp::mafGUISettingsHelp(mafObserver *Listener, const mafString &label):
mafGUISettings(Listener, label)
{
  m_BuildHelpGui = false;
  m_HelpFileName = mafGetApplicationDirectory().c_str();
  m_HelpFileName.Append("\\Help\\Help.txt");   
  InitializeSettings();
}

mafGUISettingsHelp::~mafGUISettingsHelp()
{
}

void mafGUISettingsHelp::CreateGui()
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_("Help Settings"));
  m_Gui->Label(_(""));
  m_Gui->Label(_("Help file name"));
  m_Gui->String(ID_HELP_FILE_NAME ,_(""),&m_HelpFileName);
  m_Gui->Label(_(""));
  m_Gui->Label(_("build the help gui"));
  m_Gui->Bool(ID_BUILD_HELP_GUI ,_(""),&m_BuildHelpGui,1);
  m_Gui->Divider(2);
  EnableItems(true);
}

void mafGUISettingsHelp::EnableItems( bool enable )
{
  m_Gui->Enable(ID_BUILD_HELP_GUI, enable);
  m_Gui->Enable(ID_HELP_FILE_NAME, enable);
}

void mafGUISettingsHelp::OnEvent(mafEventBase *maf_event)
{
  switch(maf_event->GetId())
  {
    case ID_BUILD_HELP_GUI:
    {
		m_Config->Write("m_BuildHelpGui",m_BuildHelpGui);
    }
    break;

	case ID_HELP_FILE_NAME:
	{
		m_Config->Write("m_HelpFileName",m_HelpFileName);
	}
	break;

	default:
      mafEventMacro(*maf_event);
    break; 
  }
  
  m_Config->Flush();
}

void mafGUISettingsHelp::InitializeSettings()
{
  int intItem;

  if(m_Config->Read("m_BuildHelpGui", &intItem))
  {
    m_BuildHelpGui = intItem;
  }
  else
  {
    m_Config->Write("m_BuildHelpGui",m_BuildHelpGui);
  }

  wxString stringItem;

  if(m_Config->Read("m_HelpFileName", &stringItem))
  {
	  m_HelpFileName = stringItem;
  }
  else
  {
	  m_Config->Write("m_HelpFileName",m_HelpFileName);
  }
  
  m_Config->Flush();
}

void mafGUISettingsHelp::OpenHelpPage( wxString entity )
{
	std::string helpFileName = m_HelpFileName;
	assert(wxFileExists(helpFileName.c_str()));
	
	std::ifstream csvFile(helpFileName.c_str());
	std::string line;
	while(std::getline(csvFile,line))
	{
		std::stringstream  lineStream(line);
		
		std::string currentLineEntityName;
		std::getline(lineStream,currentLineEntityName,',');
		wxString trimmedCurrentLineEntityName = wxString(currentLineEntityName.c_str()).Trim().Trim(false);
		
		std::string currentLineHelpLink;
		std::getline(lineStream,currentLineHelpLink,',');
		wxString trimmedCurrentLineHelpLink = wxString(currentLineHelpLink.c_str()).Trim().Trim(false);

		entity.Replace("\t","");

		if (strcmp(entity.c_str() , trimmedCurrentLineEntityName.c_str()) == 0)
		{
			wxLaunchDefaultBrowser(trimmedCurrentLineHelpLink);
			return;
		}
	}
	
	wxMessageBox("No help available for this entry");
	
}
