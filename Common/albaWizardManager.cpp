/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardManager
 Authors: Gianluigi Crimi
 
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

#include "albaDecl.h"
#include "albaWizardManager.h"
#include "albaWizardSettings.h"
#include "albaWizard.h"
#include "albaWizardWaitOp.h"
#include "albaGUISettingsDialog.h"
#include "albaServiceLocator.h"
#include "albaAbsLogicManager.h"
#include <wx/tokenzr.h>

//----------------------------------------------------------------------------
albaWizardManager::albaWizardManager()
//----------------------------------------------------------------------------
{
  //Setting default values
  m_Warn           = true;
  m_ToolBar = NULL;
  m_MenuBar = NULL;
  m_Listener = NULL;
  m_Selected = NULL;
  m_RunningWizard =NULL;
  m_ProgressGauge = NULL;
  m_ProgressLabel = NULL;
  m_Settings=new albaWizardSettings(this);
  m_NumWizard = 0;
  m_WizardList.clear();
  m_WaitOp = new albaWizardWaitOp;
}

//----------------------------------------------------------------------------
albaWizardManager::~albaWizardManager()
//----------------------------------------------------------------------------
{
  //deleting all plugged wizards
  for (int i=0;i<m_WizardList.size();i++)
    delete m_WizardList[i];

  //Clearing wizard list
  m_WizardList.clear();

  delete m_WaitOp;
  delete m_Settings;
}


//----------------------------------------------------------------------------
void albaWizardManager::FillSettingDialog(albaGUISettingsDialog *settingDialog)
//----------------------------------------------------------------------------
{
  //Fill the settings menu with wizard specific settings
  if (m_Settings != NULL)
    settingDialog->AddPage(m_Settings->GetGui(), m_Settings->GetLabel());
}

//----------------------------------------------------------------------------
void albaWizardManager::WizardAdd( albaWizard *wizard, wxString menuPath /*= ""*/ )
//----------------------------------------------------------------------------
{
  //Add a new wizard
  m_WizardList.push_back(wizard);
  wizard->SetMenuPath(menuPath);
  //Setting id starting on WIZARD_START
  wizard->SetId(m_NumWizard+WIZARD_START);
  //The listener for the wizard is the wizard manager
  wizard->SetListener(this);
  wizard->m_Manager=this;
  m_NumWizard++;
}

//----------------------------------------------------------------------------
// Return the direct submenu of menu with the given label, NULL if not present.
// wxMenu::FindItem(label) is not used because it also searches inside the submenus and can return
// a wizard item with the same label of the submenu
static wxMenu *FindSubMenu(wxMenu *menu, wxString label)
{
	wxMenuItemList &items = menu->GetMenuItems();
	for (wxMenuItemList::iterator it = items.begin(); it != items.end(); ++it)
		if ((*it)->IsSubMenu() && (*it)->GetItemLabelText() == label)
			return (*it)->GetSubMenu();
	return NULL;
}

//----------------------------------------------------------------------------
void albaWizardManager::FillMenu( wxMenu* wizardMenu )
//----------------------------------------------------------------------------
{
  int submenu_id = 1;
  //Filling the menu using the path for menu and submenu
  for(int i=0; i<m_NumWizard; i++)
  {
    albaWizard *wizard = m_WizardList[i];
    int GGTEST=wizard->GetId();
    if (wizard->GetMenuPath() != "")
    {
      wxMenu *sub_menu = NULL;
      // Local copy: the parameter must stay the root menu for the next wizards
      wxMenu *path_menu = wizardMenu;

      wxString wizardPath = "";
      //divide path in sub token
      wxStringTokenizer path_tkz(wizard->GetMenuPath(), "/");
      while ( path_tkz.HasMoreTokens() )
      {
        //search the token among the submenus of the previous level, create it if missing
        wizardPath = path_tkz.GetNextToken();
        if (sub_menu)
          path_menu = sub_menu;
        sub_menu = FindSubMenu(path_menu, _(wizardPath));
        if (sub_menu == NULL)
        {
          sub_menu = new wxMenu;
          path_menu->Append(submenu_id++,_(wizardPath),sub_menu);
        }
      }

      //append entry to the menu
      if(sub_menu)
        sub_menu->Append(wizard->GetId(), _(wizard->GetLabel()), _(wizard->GetLabel()));
      else
        albaLogMessage(_("error in FillMenu"));
    }
    else
    {
      //append entry to the menu
      wizardMenu->Append(wizard->GetId(), _(wizard->GetLabel()), _(wizard->GetLabel()));
    }
  }

}

//----------------------------------------------------------------------------
void albaWizardManager::WizardRun( int wizardId )
//----------------------------------------------------------------------------
{
  //Run a wizard starting from is menuid
  int index = wizardId - WIZARD_START;
  if(index >=0 && index <m_NumWizard) 
    WizardRun(m_WizardList[index]);
}

//----------------------------------------------------------------------------
void albaWizardManager::WizardRun( albaWizard *wizard, void *wizard_param /*= NULL*/ )
//----------------------------------------------------------------------------
{
  
  //Disabling wizard menu on wizard start
  EnableWizardMenus(false);

  m_RunningWizard=wizard;
  if (wizard_param != NULL)
  {
    m_RunningWizard->SetParameters(wizard_param);
  }
  
  //tell the logic about wizard start: menus, toolbar and operations are locked
  if (albaServiceLocator::GetLogicManager())
    albaServiceLocator::GetLogicManager()->WizardRunStarting(); 

  //set selected VME and execute the wizard
  m_RunningWizard->SetSelectedVME(m_Selected);
  m_RunningWizard->Execute();
}

//----------------------------------------------------------------------------
void albaWizardManager::VmeSelected( albaVME* node )
//----------------------------------------------------------------------------
{
  //update selected vme
  m_Selected=node;
  if (m_RunningWizard)
  {
    //if there is a running wizard we select the vme on it
    m_RunningWizard->SetSelectedVME(node);
  }
  else
  {
    //else we enable/disable menu depending on wizard accept function
    EnableWizardMenus();
  }
}

//----------------------------------------------------------------------------
void albaWizardManager::VmeModified(albaVME* node)
{
	if (!m_RunningWizard && node == m_Selected)
	{
		//else we enable/disable menu depending on wizard accept function
		EnableWizardMenus();
	}
}

//----------------------------------------------------------------------------
void albaWizardManager::WizzardStop()
//----------------------------------------------------------------------------
{
  //Stopping wizard execution
  m_RunningWizard=NULL;

  EnableWizardMenus();

  //tell the logic about wizard end
  if (albaServiceLocator::GetLogicManager())
    albaServiceLocator::GetLogicManager()->WizardRunTerminated();
}

//----------------------------------------------------------------------------
void albaWizardManager::EnableWizardMenus( bool CanEnable /*= true*/ )
//----------------------------------------------------------------------------
{
  if(m_MenuBar)
  {
    for(int i=0; i<m_NumWizard; i++)
    {
      //enabling/disabling menu depending on wizard accept function
      //and can enable parameter, when can enable is false all menu will be disabled
      albaWizard *wizard = m_WizardList[i];
      if(m_MenuBar->FindItem(wizard->GetId()))
        m_MenuBar->Enable(wizard->GetId(),CanEnable && wizard->Accept(m_Selected)); 
    }
  }
  //Disabling toolbar
  if(m_ToolBar) EnableToolbar(CanEnable);
}

//----------------------------------------------------------------------------
void albaWizardManager::OnEvent( albaEventBase *alba_event )
//----------------------------------------------------------------------------
{
  //All events from the wizards are forwarded up to logic
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    e->SetSender(this);
    albaEventMacro(*e);
  }
}

//----------------------------------------------------------------------------
void albaWizardManager::WizardSwitch(wxString wizardName)
//----------------------------------------------------------------------------
{
  int i;
  for(i=0;i<m_NumWizard;i++)
    if (wizardName==m_WizardList[i]->GetName())
    {
      WizardRun(WIZARD_START+i);
      break;
    }
  //If we not found the required wizard we stop the execution of the current
  if (i==m_NumWizard)
     WizzardStop();
}

//----------------------------------------------------------------------------
bool albaWizardManager::GetShowInformationBoxes()
//----------------------------------------------------------------------------
{
  return m_Settings->GetShowInformationBoxes() != 0;
}

//----------------------------------------------------------------------------
void albaWizardManager::SetShowInformationBoxes(bool show)
//----------------------------------------------------------------------------
{
  m_Settings->SetShowInformationBoxes(show);
}

//----------------------------------------------------------------------------
void albaWizardManager::SetProgressGauge(wxGauge *gauge, wxStaticText *label)
//----------------------------------------------------------------------------
{
  m_ProgressGauge = gauge;
  m_ProgressLabel = label;
}

//----------------------------------------------------------------------------
void albaWizardManager::ShowProgress(bool show)
//----------------------------------------------------------------------------
{
  if (m_ProgressGauge == NULL)
    return;

  if (!show)
    m_ProgressGauge->SetValue(0);
  m_ProgressGauge->Enable(show);
  if (m_ProgressLabel)
    m_ProgressLabel->Enable(show);
}

//----------------------------------------------------------------------------
void albaWizardManager::SetProgress(int progress)
//----------------------------------------------------------------------------
{
  if (m_ProgressGauge)
    m_ProgressGauge->SetValue(progress);
}

//----------------------------------------------------------------------------
void albaWizardManager::WizardContinue(int opSuccess)
//----------------------------------------------------------------------------
{
  //Called from logic on operation done to continue the execution 
  //of the wizard, opSuccess is false if user abort the operation
  if (m_RunningWizard)
    m_RunningWizard->ContinueExecution(opSuccess);
  else 
    albaLogMessage("Error no wizard running");
}


//----------------------------------------------------------------------------
void albaWizardManager::EnableToolbar( bool CanEnable /*= true*/ )
//----------------------------------------------------------------------------
{
  //Enabling toolbar function 
  //this function will be updated on wizard-toolbar creation
}

//----------------------------------------------------------------------------
albaString albaWizardManager::GetDescription()
//----------------------------------------------------------------------------
{
  //return the description of the wizard 
  //used in the main window titlebar
  if (m_RunningWizard)
    return m_RunningWizard->GetDescriptionTitle();
  else 
    return albaString("No running wizard");

}

//----------------------------------------------------------------------------
void albaWizardManager::RunOp(const char *op)
//----------------------------------------------------------------------------
{
  albaAbsLogicManager *logic = albaServiceLocator::GetLogicManager();
  if (logic == NULL)
    return;

  //Special operations for the wizard: the file operations end at once and the wizard continues with their result,
  //the others are run by the logic and the wizard continues when they end
  albaString opString=op;
  if (opString=="PAUSE")
    logic->WizardRunOp(m_WaitOp);
  else if (opString=="SAVE")
    WizardContinue(logic->FileSave() != ALBA_ERROR);
  else if (opString=="SAVE_AS")
    WizardContinue(logic->FileSaveAs() != ALBA_ERROR);
  else if (opString=="OPEN")
    WizardContinue(logic->FileOpen() != ALBA_ERROR);
  else if (opString=="DELETE")
    logic->WizardRunOp(OP_DELETE);
  else if (opString=="NEW")
    WizardContinue(logic->FileNew() != ALBA_ERROR);
  else if (opString=="RELOAD")
    WizardContinue(logic->FileReload() != ALBA_ERROR);
  else if (opString=="SNAPSHOT")
    WizardContinue(logic->CreateSnapshot() != ALBA_ERROR);
  else
    logic->WizardRunOp(opString.GetCStr());
}

