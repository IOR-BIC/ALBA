/*=========================================================================

 Program: MAF2Medical
 Module: medWizardManager
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medWizardManager.h"
#include "medWizard.h"
#include <wx/tokenzr.h>

//----------------------------------------------------------------------------
medWizardManager::medWizardManager()
//----------------------------------------------------------------------------
{
  //Setting default values
  m_Warn           = true;
  m_ToolBar = NULL;
  m_MenuBar = NULL;
  m_Listener = NULL;
  m_Selected = NULL;
  m_RunningWizard =NULL;
  m_NumWizard = 0;
  m_WizardList.clear();
}

//----------------------------------------------------------------------------
medWizardManager::~medWizardManager()
//----------------------------------------------------------------------------
{
  //deleting all plugged wizards
  for (int i=0;i<m_WizardList.size();i++)
    delete m_WizardList[i];

  //Clearing wizard list
  m_WizardList.clear();
}

//----------------------------------------------------------------------------
void medWizardManager::WizardAdd( medWizard *wizard, wxString menuPath /*= ""*/ )
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
void medWizardManager::FillMenu( wxMenu* wizardMenu )
//----------------------------------------------------------------------------
{
  int submenu_id = 1;
  //Filling the menu using the path for menu and submenu
  for(int i=0; i<m_NumWizard; i++)
  {
    medWizard *wizard = m_WizardList[i];
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
        mafLogMessage(_("error in FillMenu"));
    }
    else
    {
      //append entry to the menu
      wizardMenu->Append(wizard->GetId(), _(wizard->GetLabel()), _(wizard->GetLabel()));
    }
  }

}

//----------------------------------------------------------------------------
void medWizardManager::WizardRun( int wizardId )
//----------------------------------------------------------------------------
{
  //Run a wizard starting from is menuid
  int index = wizardId - WIZARD_START;
  if(index >=0 && index <m_NumWizard) 
    WizardRun(m_WizardList[index]);
}

//----------------------------------------------------------------------------
void medWizardManager::WizardRun( medWizard *wizard, void *wizard_param /*= NULL*/ )
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
void medWizardManager::VmeSelected( mafNode* node )
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
    //else we enable/disable menu depending on wizard accept funcition
    EnableWizardMenus();
  }
}

//----------------------------------------------------------------------------
void medWizardManager::WizzardStop()
//----------------------------------------------------------------------------
{
  //Stopping wizard execution
  m_RunningWizard=NULL;

  //notify to to logic
  Notify(WIZARD_RUN_TERMINATED);
}

//----------------------------------------------------------------------------
void medWizardManager::EnableWizardMenus( bool CanEnable /*= true*/ )
//----------------------------------------------------------------------------
{
  if(m_MenuBar)
  {
    for(int i=0; i<m_NumWizard; i++)
    {
      //enabling/disabling menu depending on wizard accept function
      //and can enable parameter, when can enable is false all menu will be disabled
      medWizard *wizard = m_WizardList[i];
      if(m_MenuBar->FindItem(wizard->GetId()))
        m_MenuBar->Enable(wizard->GetId(),CanEnable && wizard->Accept(m_Selected)); 
    }
  }
  //Disabling toolbar
  if(m_ToolBar) EnableToolbar(CanEnable);
}

//----------------------------------------------------------------------------
void medWizardManager::OnEvent( mafEventBase *maf_event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case WIZARD_RUN_TERMINATED:
     {
       m_RunningWizard=NULL;
       EnableWizardMenus();
       //Forward up event to logic
       //Used for other menu and toolbar enabling
       mafEventMacro(*e);
     }
    break;
    default:
      //All event form wizard (like vme select/show, view request, ecc...)
      //will be forwared up to logic
      mafEventMacro(*e);
      break;
    } // end switch case
  }
}

//----------------------------------------------------------------------------
void medWizardManager::WizardContinue(int opSuccess)
//----------------------------------------------------------------------------
{
  //Called from logic on operation done to continue the execution 
  //of the wizard, opSuccess is false if user abort the operation
  if (m_RunningWizard)
    m_RunningWizard->ContinueExecution(opSuccess);
  else 
    mafLogMessage("Error no wizard running");
}


//----------------------------------------------------------------------------
void medWizardManager::Notify( int msg )
//----------------------------------------------------------------------------
{
  //notify operation to logic
  mafEventMacro(mafEvent(this,msg));  
}

//----------------------------------------------------------------------------
void medWizardManager::EnableToolbar( bool CanEnable /*= true*/ )
//----------------------------------------------------------------------------
{
  //Enabling toolbar function 
  //this function will be updated on wizard-toolbar creation
}
