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
  m_NumWizard++;
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
      
      wxString wizardPath = "";
      //divide path in sub token
      wxStringTokenizer path_tkz(wizard->GetMenuPath(), "/");
      while ( path_tkz.HasMoreTokens() )
      {
        //search inside the menu for the token
        wizardPath = path_tkz.GetNextToken();
        int item = wizardMenu->FindItem(_(wizardPath));
        if (item != wxNOT_FOUND)
        {
          //if the submenu is found select it
          wxMenuItem *menu_item = wizardMenu->FindItem(item);
          if (menu_item)
            sub_menu = menu_item->GetSubMenu();
        }
        else
        {
          //else create a new submenu
          if (sub_menu)
          {
            wizardMenu = sub_menu;
          }
          sub_menu = new wxMenu;
          wizardMenu->Append(submenu_id++,_(wizardPath),sub_menu);
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
  
  //notify logic about wizard start
  //Used for other menu and toolbar disabling
  Notify(WIZARD_RUN_STARTING); 

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

  //notify to to logic
  Notify(WIZARD_RUN_TERMINATED);
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
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case WIZARD_RUN_TERMINATED:
     {
        WizzardStop();
     }
    break;
    case WIZARD_INFORMATION_BOX_SHOW_SET:
      {
        //update the wizard settings
        m_Settings->SetShowInformationBoxes(e->GetBool());
      }
    break;
    case WIZARD_INFORMATION_BOX_SHOW_GET:
      {
        //get the wizard settings and update the event to inform the caller
        e->SetBool(m_Settings->GetShowInformationBoxes());
      }
    break;
    case WIZARD_RUN_OP:
      { 
        //call the handler for operations
        OnRunOp(e);
      }
    break;
    case WIZARD_SWITCH:
      {
        int i;
        wxString wizardName=e->GetString()->GetCStr();
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
    break;
    default:
      //All event form wizard (like vme select/show, view request, ecc...)
      //will be forwarded up to logic
      e->SetSender(this);
      albaEventMacro(*e);
      break;
    } // end switch case
  }
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
void albaWizardManager::Notify( int msg )
//----------------------------------------------------------------------------
{
  //notify operation to logic
  albaEventMacro(albaEvent(this,msg));  
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
void albaWizardManager::OnRunOp(albaEvent *e)
//----------------------------------------------------------------------------
{
  //Special operation for the wizard
  albaString opString=*e->GetString();
  if (opString=="PAUSE")
  {
    //pause op
    albaEventMacro(albaEvent(this,WIZARD_PAUSE,m_WaitOp));
  }
  else if (opString=="SAVE")
  {
    //Save msf
    albaEventMacro(albaEvent(this,MENU_FILE_SAVE));
  }
  else if (opString=="SAVE_AS")
  {
    //save msf with name
    albaEventMacro(albaEvent(this,MENU_FILE_SAVEAS));
  }
  else if (opString=="OPEN")
  {
    //Open MSF
    albaEventMacro(albaEvent(this,MENU_FILE_OPEN));
  }
  else if (opString=="DELETE")
  {
    //Delete current VME
    albaEventMacro(albaEvent(this,WIZARD_OP_DELETE));
  }
  else if (opString=="NEW")
  {
    albaEventMacro(albaEvent(this,WIZARD_OP_NEW));
  }
  else if (opString=="RELOAD")
  {
    albaEventMacro(albaEvent(this,WIZARD_RELOAD_MSF));
  }
  else if (opString=="SNAPSHOT")
  {
    albaEventMacro(albaEvent(this,MENU_FILE_SNAPSHOT,true));
  }
  else
  {
    //Run the standard operations
    albaEventMacro(*e);
  }
}

