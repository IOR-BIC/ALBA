/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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
#include "medLogicWithManagers.h"
#include "medGUIContextualMenu.h"
#include "mafGUIMDIChild.h"
#include "mafViewManager.h"
#include "medGUIMDIFrame.h"
#include "mafOpManager.h"
#include "medWizardManager.h"
#include "mafGUIApplicationSettings.h"
#include "mafView.h"

#ifdef MAF_USE_VTK
  #include "mafInteractionManager.h"
#endif


//----------------------------------------------------------------------------
medLogicWithManagers::medLogicWithManagers()
: mafLogicWithManagers( new medGUIMDIFrame("maf", wxDefaultPosition, wxSize(800, 600)) )
//----------------------------------------------------------------------------
{
  //Set default values
  m_UseWizardManager  = false;
  m_WizardRunning = false;
  m_WizardManager = NULL;
}

//----------------------------------------------------------------------------
medLogicWithManagers::~medLogicWithManagers()
//----------------------------------------------------------------------------
{
  //free mem
  if(m_WizardManager)
    delete m_WizardManager;
}

//----------------------------------------------------------------------------
void medLogicWithManagers::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  // Create and visualize the contextual menu for the active vme
  medGUIContextualMenu *contextMenu = new medGUIContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
  cppDEL(contextMenu);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{
		  case ABOUT_APPLICATION:
			{
        // trap the ABOUT_APPLICATION event and shows the about window with the application infos
				wxString message = m_AppTitle.GetCStr();
				message += _(" Application ");
				message += m_Revision;
				wxMessageBox(message, "About Application");
				mafLogMessage(wxString::Format("%s",m_Revision.GetCStr()));
			}
		 break;
     case MENU_WIZARD:
      //The event from the application menu
      if(m_WizardManager) 
        m_WizardManager->WizardRun(e->GetArg());
     break;
     case WIZARD_RUN_STARTING:
      {
        //Manage start event from the wizard lock window close button
        //and disabling toolbar
        mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(false);
        WizardRunStarting();
      }
     break; 
     case WIZARD_RUN_TERMINATED:
      {
        //Manage end event from the wizard unlock window close button
        //and enabling toolbar
        mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
        if (c != NULL)
          c->SetAllowCloseWindow(true);
        WizardRunTerminated();
        UpdateFrameTitle();
      }
    break; 
    case WIZARD_UPDATE_WINDOW_TITLE:
       {
         UpdateFrameTitle();
       }
    break;
    case WIZARD_REQUIRED_VIEW:
       {
         //The wizard requires a specific view
         //searching on open views or open a new one
         mafView *view;
         const char *viewStr=e->GetString()->GetCStr();
         
         view=m_ViewManager->GetFromList(viewStr);
         if (view)
           m_ViewManager->Activate(view);
         else
           m_ViewManager->ViewCreate(viewStr);
       }
    break;
    case WIZARD_RUN_OP:
      {
        //Running an op required from the wizard
        mafString *tmp=e->GetString();
        mafLogMessage("wiz starting :%s",tmp->GetCStr());
        m_CancelledBeforeOpStarting=true;
        UpdateFrameTitle();
        m_OpManager->OpRun(*(e->GetString()));
        if (m_CancelledBeforeOpStarting)
        {
          m_CancelledBeforeOpStarting=false;
           m_WizardManager->WizardContinue(false);
        }
        
      }
    break;
    case WIZARD_PAUSE:
      {
        UpdateFrameTitle();
        m_OpManager->OpRun(e->GetOp());
      }
      break;
    case OP_RUN_STARTING:
      {
        mafLogMessage("run starting");
        m_CancelledBeforeOpStarting=false;
        mafLogicWithManagers::OnEvent(maf_event);
      }
    break;
    case OP_RUN_TERMINATED:
      {
        //if the operation was started from the wizard we continue the wizard execution
        if (m_WizardManager && m_WizardRunning)
        {
          m_WizardManager->WizardContinue(e->GetArg());
        }
        //else we manage the operation end by unlock the close button and so on
        else
        {
          mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
          if (c != NULL)
            c->SetAllowCloseWindow(true);
          OpRunTerminated();
        }
      }
      break; 
		default:
      //Call parent event manager
			mafLogicWithManagers::OnEvent(maf_event);
			break; 
		} // end switch case
	} // end if SafeDowncast
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafOp *op, wxString menuPath /*= ""*/, bool canUndo /*= true*/, mafGUISettings *setting /*= NULL*/ )
//----------------------------------------------------------------------------
{
  //plug functions needs to be redefined to avoid overload ambiguity
  mafLogicWithManagers::Plug(op,menuPath,canUndo,setting);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( mafView* view, bool visibleInMenu /*= true*/ )
//----------------------------------------------------------------------------
{
  //plug functions needs to be redefined to avoid overload ambiguity
  mafLogicWithManagers::Plug(view,visibleInMenu);
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Plug( medWizard *wizard, wxString menuPath /*= ""*/ )
//----------------------------------------------------------------------------
{
  //Plugging the wizard    
  if(m_WizardManager)
  {
    m_WizardManager->WizardAdd(wizard, menuPath);
  }
  else 
  {
    mafLogMessage("Enable wizard pluggin to plug wizards"); 
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Configure();

  //Setting wizard specific data
  if(m_UseWizardManager)
  {
    m_WizardManager = new medWizardManager();
    m_WizardManager->SetListener(this);
    m_WizardManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  mafLogicWithManagers::Show();

  //setting gui pointers to the Wizard Manager
  if(m_WizardManager)
  {
    if(m_MenuBar)
    {
      m_WizardManager->FillMenu(m_WizardMenu);
      m_WizardManager->SetMenubar(m_MenuBar);
    }
    if(m_ToolBar)
      m_WizardManager->SetToolbar(m_ToolBar);
  }

}

//----------------------------------------------------------------------------
void medLogicWithManagers::HandleException()
//----------------------------------------------------------------------------
{
  int answare = wxMessageBox(_("Do you want to try to save the unsaved work ?"), _("Fatal Exception!!"), wxYES_NO|wxCENTER);
  if(answare == wxYES)
  {
    OnFileSaveAs();
    
    if (m_OpManager->Running())
      m_OpManager->StopCurrentOperation();
  }
  OnQuit();
}

//----------------------------------------------------------------------------
void medLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  if (m_WizardManager && m_WizardRunning)
    return;

  mafLogicWithManagers::OnQuit();
}


//----------------------------------------------------------------------------
void medLogicWithManagers::VmeSelected( mafNode *vme )
//----------------------------------------------------------------------------
{
  //if a wizard manager was plugged we tell it about vme selection
  if(m_WizardManager)  
    m_WizardManager->VmeSelected(vme);
  mafLogicWithManagers::VmeSelected(vme);
}


void medLogicWithManagers::CreateMenu()
{
  mafLogicWithManagers::CreateMenu();

  //add the wizard menu between "operation" and "settings"
  if (m_UseWizardManager)
  {
    m_WizardMenu = new wxMenu;
    m_MenuBar->Insert(4,m_WizardMenu, _("&Wizard"));
  }
}

//----------------------------------------------------------------------------
void medLogicWithManagers::WizardRunStarting()
//----------------------------------------------------------------------------
{
  //Disabling menu, toolbars and selection by interacion manager and sidebar
  m_WizardRunning=true;

  EnableMenuAndToolbar(false);
  // currently mafInteraction is strictly dependent on VTK (marco)
  #ifdef MAF_USE_VTK
    if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
  #endif
    if(m_SideBar)    m_SideBar->EnableSelect(false);

}

//----------------------------------------------------------------------------
void medLogicWithManagers::WizardRunTerminated()
//----------------------------------------------------------------------------
{
  //Enabling menu, toolbars and selection by interacion manager and sidebar
  m_WizardRunning=false;

  EnableMenuAndToolbar(true);
  // currently mafInteraction is strictly dependent on VTK (marco)
  #ifdef MAF_USE_VTK
    if(m_InteractionManager) 
      m_InteractionManager->EnableSelect(true);
  #endif
    if(m_SideBar)
      m_SideBar->EnableSelect(true);

}


//----------------------------------------------------------------------------
void medLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  if (m_WizardRunning)
  {
    wxString title(m_AppTitle);
    title += "   " + m_WizardManager->GetDescription();
    m_Win->SetTitle(title);
  }
  else 
    mafLogicWithManagers::UpdateFrameTitle();
}

