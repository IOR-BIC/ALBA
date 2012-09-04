/*=========================================================================

 Program: MAF2Medical
 Module: medVect3d
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
  for (int i=0;i<m_WizardList.size();i++)
    delete m_WizardList[i];

  m_WizardList.clear();
  
}

void medWizardManager::WizardAdd( medWizard *wizard, wxString menuPath /*= ""*/ )
{
  m_WizardList.push_back(wizard);
  wizard->SetMenuPath(menuPath);
  wizard->SetId(m_NumWizard+WIZARD_START);
  wizard->SetListener(this);
  m_NumWizard++;
}

void medWizardManager::FillMenu( wxMenu* wizardMenu )
{
  int submenu_id = 1;

  for(int i=0; i<m_NumWizard; i++)
  {
    medWizard *wizard = m_WizardList[i];
    int GGTEST=wizard->GetId();
    if (wizard->GetMenuPath() != "")
    {
      wxMenu *sub_menu = NULL;
      
      wxString wizardPath = "";
      wxStringTokenizer path_tkz(wizard->GetMenuPath(), "/");
      while ( path_tkz.HasMoreTokens() )
      {
        wizardPath = path_tkz.GetNextToken();
        int item = wizardMenu->FindItem(_(wizardPath));
        if (item != wxNOT_FOUND)
        {
          wxMenuItem *menu_item = wizardMenu->FindItem(item);
          if (menu_item)
            sub_menu = menu_item->GetSubMenu();
        }
        else
        {
          if (sub_menu)
          {
            wizardMenu = sub_menu;
          }
          sub_menu = new wxMenu;
          wizardMenu->Append(submenu_id++,_(wizardPath),sub_menu);
        }
      }

      if(sub_menu)
        sub_menu->Append(wizard->GetId(), _(wizard->GetLabel()), _(wizard->GetLabel()));
      else
        mafLogMessage(_("error in FillMenu"));
    }
    else
    {
      wizardMenu->Append(wizard->GetId(), _(wizard->GetLabel()), _(wizard->GetLabel()));
    }
  }

}

void medWizardManager::WizardRun( int wizardId )
{
  int index = wizardId - WIZARD_START;
  if(index >=0 && index <m_NumWizard) 
    WizardRun(m_WizardList[index]);
}

void medWizardManager::WizardRun( medWizard *wizard, void *wizard_param /*= NULL*/ )
{
  
  EnableWizardMenus(false);

  m_RunningWizard=wizard;
  if (wizard_param != NULL)
  {
    m_RunningWizard->SetParameters(wizard_param);
  }
  
  Notify(WIZARD_RUN_STARTING); 

  m_RunningWizard->SetSelectedVME(m_Selected);
  m_RunningWizard->Execute();

}

void medWizardManager::VmeSelected( mafNode* node )
{
  m_Selected=node;
  if (m_RunningWizard)
  {
    m_RunningWizard->SetSelectedVME(node);
  }
  else
  {
    EnableWizardMenus();
  }
}

void medWizardManager::WizzardStop()
{
  m_RunningWizard=NULL;

  //STOP EVENT
}

void medWizardManager::EnableWizardMenus( bool CanEnable /*= true*/ )
{
  mafOp *o = NULL;
  if(m_MenuBar)
  {
    for(int i=0; i<m_NumWizard; i++)
    {
      medWizard *wizard = m_WizardList[i];
      if(m_MenuBar->FindItem(wizard->GetId()))
        m_MenuBar->Enable(wizard->GetId(),CanEnable && wizard->Accept(m_Selected)); 
    }
  }
  if(m_ToolBar) EnableToolbar(CanEnable);
}

void medWizardManager::OnEvent( mafEventBase *maf_event )
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case WIZARD_RUN_TERMINATED:
     {
       m_RunningWizard=NULL;
       EnableWizardMenus();
       //Forward up event for toolbar activation
       Notify(WIZARD_RUN_TERMINATED);
     }
    break;
    default:
      mafEventMacro(*e);
      break;
    } // end switch case
  }
}

void medWizardManager::WizardContinue(int opSuccess)
{
  if (m_RunningWizard)
    m_RunningWizard->ContinueExecution(opSuccess);
  else 
    mafLogMessage("Error no wizard running");
}


//----------------------------------------------------------------------------
void medWizardManager::Notify( int msg )
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,msg));  
}

void medWizardManager::EnableToolbar( bool CanEnable /*= true*/ )
{

}
